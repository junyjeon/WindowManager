#include "../include/WindowManager.h"
#include <windows.h>
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)
#define IDI_TRAYICON 1

WindowManager* g_windowManager = nullptr;
NOTIFYICONDATA g_nid = {0};

// 시스템 트레이 아이콘 초기화
void InitTrayIcon(HWND hwnd) {
    g_nid.cbSize = sizeof(NOTIFYICONDATA);
    g_nid.hWnd = hwnd;
    g_nid.uID = IDI_TRAYICON;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpy(g_nid.szTip, TEXT("Window Manager"));
    Shell_NotifyIcon(NIM_ADD, &g_nid);
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            InitTrayIcon(hwnd);
            return 0;

        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &g_nid);
            PostQuitMessage(0);
            return 0;

        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                // 여기에 트레이 아이콘 메뉴 처리 추가
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 윈도우 클래스 등록
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("WindowManagerClass");
    RegisterClassEx(&wc);

    // 숨겨진 윈도우 생성
    HWND hwnd = CreateWindowEx(
        0,
        TEXT("WindowManagerClass"),
        TEXT("Window Manager"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        return 1;
    }

    // WindowManager 인스턴스 생성 및 초기화
    g_windowManager = new WindowManager();
    if (!g_windowManager->Initialize()) {
        delete g_windowManager;
        return 1;
    }

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 정리
    delete g_windowManager;
    return (int)msg.wParam;
}
