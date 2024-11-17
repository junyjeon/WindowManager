#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <map>
#include <vector>
#include <string>
#include <dwmapi.h>
#include <chrono>

#pragma comment(lib, "dwmapi.lib")

#define WM_TRAYICON (WM_USER + 1)
#define IDI_TRAYICON 1
#define IDM_EXIT 100

// 핫키 ID 정의
enum HotkeyIds {
    HK_LEFT = 1001,
    HK_RIGHT = 1002,
    HK_TOP = 1003,
    HK_BOTTOM = 1004,
    HK_FULLSCREEN = 1005,
    HK_TOGGLE_GRID = 1006,
    HK_RESET = 1007
};

// 전역 변수
NOTIFYICONDATA nid = {0};
HWND hwnd;
HMENU hPopMenu;
bool isGridVisible = false;
std::map<std::string, RECT> savedWindowPositions;
int gridSize = 12;
float gridOpacity = 0.3f;

// 연속 키 입력 추적을 위한 변수
struct KeyState {
    int count;
    std::chrono::steady_clock::time_point lastPress;
};
std::map<int, KeyState> keyStates;
const int KEY_TIMEOUT_MS = 500;

// 디버그 메시지 출력
void ShowDebugMessage(const TCHAR* message) {
    OutputDebugString(message);
    OutputDebugString(_T("\n"));
}

// 핫키 등록 함수
bool RegisterAppHotkey(HWND hwnd, int id, UINT modifiers, UINT vk, const TCHAR* description) {
    UnregisterHotKey(hwnd, id);
    if (!RegisterHotKey(hwnd, id, modifiers, vk)) {
        TCHAR buffer[256];
        _stprintf_s(buffer, _T("핫키 등록 실패: %s (Error: %d)"), description, GetLastError());
        MessageBox(NULL, buffer, _T("오류"), MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

// 창 위치 조정 함수
void SnapWindow(HWND targetWindow, int position) {
    if (!targetWindow || !IsWindow(targetWindow)) return;

    // 현재 모니터의 작업 영역 가져오기
    HMONITOR hMonitor = MonitorFromWindow(targetWindow, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &mi);
    RECT workArea = mi.rcWork;

    int screenWidth = workArea.right - workArea.left;
    int screenHeight = workArea.bottom - workArea.top;
    RECT newPos = workArea;

    // 좌우 키 처리
    if (position == HK_LEFT || position == HK_RIGHT) {
        auto now = std::chrono::steady_clock::now();
        auto& state = keyStates[position];
        
        // 타임아웃 체크
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - state.lastPress).count();
        if (elapsed > KEY_TIMEOUT_MS) {
            state.count = -1;
        }

        // 비율 설정 (1/2 → 1/3 → 1/4 → 3/4)
        float ratios[] = {0.5f, 0.33f, 0.25f, 0.75f};
        state.count = (state.count + 1) % 4;
        state.lastPress = now;

        int newWidth = static_cast<int>(screenWidth * ratios[state.count]);

        if (position == HK_LEFT) {
            newPos.right = workArea.left + newWidth;
        } else {
            newPos.left = workArea.right - newWidth;
        }

        TCHAR buffer[256];
        _stprintf_s(buffer, _T("단계: %d, 비율: %.3f, 너비: %d (전체: %d)"),
                    state.count + 1, ratios[state.count], newWidth, screenWidth);
        ShowDebugMessage(buffer);
    }
    // 상하 키 처리
    else if (position == HK_TOP || position == HK_BOTTOM) {
        if (position == HK_TOP) {
            newPos.bottom = workArea.top + screenHeight/2;
        } else {
            newPos.top = workArea.top + screenHeight/2;
        }
    }

    // 창 위치 및 크기 설정
    SetWindowPos(targetWindow, NULL,
                newPos.left, newPos.top,
                newPos.right - newPos.left,
                newPos.bottom - newPos.top,
                SWP_NOZORDER);
}

// 그리드 그리기 함수
void DrawGrid(HDC hdc, const RECT& rect) {
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, static_cast<BYTE>(gridOpacity * 255), AC_SRC_ALPHA};
    
    int cellWidth = (rect.right - rect.left) / gridSize;
    int cellHeight = (rect.bottom - rect.top) / gridSize;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 1; i < gridSize; i++) {
        int x = rect.left + i * cellWidth;
        MoveToEx(hdc, x, rect.top, NULL);
        LineTo(hdc, x, rect.bottom);
    }

    for (int i = 1; i < gridSize; i++) {
        int y = rect.top + i * cellHeight;
        MoveToEx(hdc, rect.left, y, NULL);
        LineTo(hdc, rect.right, y);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 트레이 아이콘 설정
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = IDI_TRAYICON;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON;
            nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            _tcscpy_s(nid.szTip, _T("Window Manager"));
            Shell_NotifyIcon(NIM_ADD, &nid);

            // 팝업 메뉴
            hPopMenu = CreatePopupMenu();
            InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("종료"));

            // KeyState 초기화
            keyStates[HK_LEFT] = {-1, std::chrono::steady_clock::now()};
            keyStates[HK_RIGHT] = {-1, std::chrono::steady_clock::now()};

            // 핫키 등록
            bool success = true;
            success &= RegisterAppHotkey(hwnd, HK_LEFT, MOD_CONTROL, VK_LEFT, _T("Ctrl + Left"));
            success &= RegisterAppHotkey(hwnd, HK_RIGHT, MOD_CONTROL, VK_RIGHT, _T("Ctrl + Right"));
            success &= RegisterAppHotkey(hwnd, HK_TOP, MOD_CONTROL, VK_UP, _T("Ctrl + Up"));
            success &= RegisterAppHotkey(hwnd, HK_BOTTOM, MOD_CONTROL, VK_DOWN, _T("Ctrl + Down"));
            success &= RegisterAppHotkey(hwnd, HK_FULLSCREEN, MOD_CONTROL, VK_RETURN, _T("Ctrl + Enter"));
            success &= RegisterAppHotkey(hwnd, HK_TOGGLE_GRID, MOD_CONTROL, 'G', _T("Ctrl + G"));
            success &= RegisterAppHotkey(hwnd, HK_RESET, MOD_CONTROL, 'R', _T("Ctrl + R"));

            if (!success) {
                MessageBox(NULL, _T("일부 핫키 등록 실패"), _T("경고"), MB_OK | MB_ICONWARNING);
            }

            MessageBox(NULL, _T("Window Manager가 시작되었습니다.\n\n"
                               "Ctrl + ←/→: 좌/우 정렬 (연속 입력 시 1/2 → 1/3 → 1/4 → 3/4)\n"
                               "Ctrl + ↑/↓: 상/하 절반\n"
                               "Ctrl + Enter: 전체화면\n"
                               "Ctrl + G: 그리드 표시/숨김\n"
                               "Ctrl + R: 창 크기 초기화"),
                      _T("Window Manager"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        case WM_HOTKEY: {
            int hotkeyId = (int)wParam;
            ShowDebugMessage(_T("핫키 감지"));

            HWND foreground = GetForegroundWindow();
            if (foreground) {
                if (hotkeyId == HK_TOGGLE_GRID) {
                    ShowDebugMessage(_T("그리드 토글"));
                    isGridVisible = !isGridVisible;
                    InvalidateRect(NULL, NULL, TRUE);
                } else if (hotkeyId == HK_RESET) {
                    ShowDebugMessage(_T("창 크기 초기화"));
                    ShowWindow(foreground, SW_RESTORE);
                } else {
                    SnapWindow(foreground, hotkeyId);
                }
            }
            break;
        }

        case WM_PAINT:
            if (isGridVisible) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                RECT workArea;
                SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
                DrawGrid(hdc, workArea);
                EndPaint(hwnd, &ps);
            }
            break;

        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hPopMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON,
                             pt.x, pt.y, 0, hwnd, NULL);
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDM_EXIT) {
                DestroyWindow(hwnd);
            }
            break;

        case WM_DESTROY:
            // 핫키 해제
            for (int i = HK_LEFT; i <= HK_RESET; i++) {
                UnregisterHotKey(hwnd, i);
            }
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 윈도우 클래스 등록
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("WindowManager");
    RegisterClassEx(&wc);

    // 윈도우 생성
    hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        _T("WindowManager"),
        _T("Window Manager"),
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, _T("윈도우 생성 실패"), _T("오류"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 투명도 설정
    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
    ShowWindow(hwnd, SW_SHOW);

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
