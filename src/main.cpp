#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include "window_manager.h"
#include "hotkey_manager.h"

#define WM_TRAYICON (WM_USER + 1)
#define IDI_TRAYICON 1
#define IDM_EXIT 100

NOTIFYICONDATA nid = {0};
HWND hwnd;
HMENU hPopMenu;

// 디버그 메시지 출력
void ShowDebugMessage(const TCHAR* message) {
    MessageBox(NULL, message, _T("디버그"), MB_OK);
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            ShowDebugMessage(_T("WM_CREATE 시작"));

            // 트레이 아이콘 설정
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = IDI_TRAYICON;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON;
            nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            _tcscpy_s(nid.szTip, _T("Window Manager"));
            
            if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
                ShowDebugMessage(_T("트레이 아이콘 생성 실패"));
                return -1;
            }

            // 팝업 메뉴
            hPopMenu = CreatePopupMenu();
            InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("종료"));

            // 기본 핫키 등록 (Win + 1)
            if (!RegisterHotKey(hwnd, 1, MOD_WIN, '1')) {
                ShowDebugMessage(_T("테스트 핫키 등록 실패"));
                return -1;
            }

            ShowDebugMessage(_T("초기화 완료"));
            return 0;
        }

        case WM_HOTKEY: {
            ShowDebugMessage(_T("핫키 감지됨"));
            HWND foreground = GetForegroundWindow();
            if (foreground) {
                // 테스트: 창을 화면 왼쪽으로 이동
                RECT workArea;
                SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
                SetWindowPos(foreground, NULL, 
                           workArea.left, workArea.top,
                           (workArea.right - workArea.left) / 2,
                           workArea.bottom - workArea.top,
                           SWP_NOZORDER);
            }
            break;
        }

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
            UnregisterHotKey(hwnd, 1);  // 핫키 해제
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShowDebugMessage(_T("프로그램 시작"));

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("WindowManager");
    
    if (!RegisterClassEx(&wc)) {
        ShowDebugMessage(_T("윈도우 클래스 등록 실패"));
        return FALSE;
    }

    // 윈도우 생성
    hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        _T("WindowManager"),
        _T("Window Manager"),
        WS_OVERLAPPED,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1, 1,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        ShowDebugMessage(_T("윈도우 생성 실패"));
        return FALSE;
    }

    ShowWindow(hwnd, SW_HIDE);
    ShowDebugMessage(_T("메시지 루프 시작"));

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
