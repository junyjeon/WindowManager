#include "../include/WindowManager.h"
#include <dwmapi.h>

WindowManager::WindowManager() : m_isRunning(false) {
    m_grid.rows = 12;
    m_grid.cols = 12;
    m_grid.visible = false;
    m_grid.opacity = 0.5f;
}

WindowManager::~WindowManager() {
    Shutdown();
}

bool WindowManager::Initialize() {
    RegisterHotkeys();
    m_isRunning = true;
    return true;
}

void WindowManager::Shutdown() {
    UnregisterHotkeys();
    m_isRunning = false;
}

void WindowManager::SnapWindowToPosition(HWND hwnd, int position) {
    if (!IsWindow(hwnd)) return;

    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    
    int width = (workArea.right - workArea.left) / 2;
    int height = (workArea.bottom - workArea.top) / 2;
    
    RECT newPos = {0};
    
    switch (position) {
        case TOP_LEFT:
            newPos = {workArea.left, workArea.top, workArea.left + width, workArea.top + height};
            break;
        case TOP_RIGHT:
            newPos = {workArea.left + width, workArea.top, workArea.right, workArea.top + height};
            break;
        case BOTTOM_LEFT:
            newPos = {workArea.left, workArea.top + height, workArea.left + width, workArea.bottom};
            break;
        case BOTTOM_RIGHT:
            newPos = {workArea.left + width, workArea.top + height, workArea.right, workArea.bottom};
            break;
        case CENTER:
            newPos = {
                workArea.left + (workArea.right - workArea.left) / 4,
                workArea.top + (workArea.bottom - workArea.top) / 4,
                workArea.right - (workArea.right - workArea.left) / 4,
                workArea.bottom - (workArea.bottom - workArea.top) / 4
            };
            break;
        case TOP_CENTER:
            newPos = {
                workArea.left + width / 2,
                workArea.top,
                workArea.right - width / 2,
                workArea.top + height
            };
            break;
        case LEFT_CENTER:
            newPos = {
                workArea.left,
                workArea.top + height / 2,
                workArea.left + width,
                workArea.bottom - height / 2
            };
            break;
        case RIGHT_CENTER:
            newPos = {
                workArea.right - width,
                workArea.top + height / 2,
                workArea.right,
                workArea.bottom - height / 2
            };
            break;
    }
    
    SetWindowPos(hwnd, NULL, 
                newPos.left, newPos.top,
                newPos.right - newPos.left, newPos.bottom - newPos.top,
                SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowManager::ShowGrid(bool show) {
    m_grid.visible = show;
    // TODO: 그리드 오버레이 표시 구현
}

void WindowManager::SaveLayout(const std::string& name) {
    std::vector<WindowState> layout;
    // 현재 활성화된 모든 창의 상태를 저장
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        if (IsWindowVisible(hwnd) && !IsIconic(hwnd)) {
            WindowState state;
            GetWindowRect(hwnd, &state.position);
            state.isMaximized = IsZoomed(hwnd);
            ((std::vector<WindowState>*)lParam)->push_back(state);
        }
        return TRUE;
    }, (LPARAM)&layout);
    
    m_savedLayouts[name] = layout;
}

void WindowManager::LoadLayout(const std::string& name) {
    auto it = m_savedLayouts.find(name);
    if (it != m_savedLayouts.end()) {
        // TODO: 저장된 레이아웃 복원 구현
    }
}

void WindowManager::SaveWindowState(HWND hwnd) {
    WindowState state;
    GetWindowRect(hwnd, &state.position);
    state.isMaximized = IsZoomed(hwnd);
    m_windowStates[hwnd] = state;
}

void WindowManager::RestoreWindowState(HWND hwnd) {
    auto it = m_windowStates.find(hwnd);
    if (it != m_windowStates.end()) {
        const WindowState& state = it->second;
        if (state.isMaximized) {
            ShowWindow(hwnd, SW_MAXIMIZE);
        } else {
            SetWindowPos(hwnd, NULL,
                        state.position.left, state.position.top,
                        state.position.right - state.position.left,
                        state.position.bottom - state.position.top,
                        SWP_NOZORDER);
        }
    }
}

void WindowManager::RegisterHotkeys() {
    // Windows + 방향키
    RegisterHotKey(NULL, 1, MOD_WIN, VK_LEFT);  // 왼쪽
    RegisterHotKey(NULL, 2, MOD_WIN, VK_RIGHT); // 오른쪽
    RegisterHotKey(NULL, 3, MOD_WIN, VK_UP);    // 위
    RegisterHotKey(NULL, 4, MOD_WIN, VK_DOWN);  // 아래

    // Windows + Shift + 방향키
    RegisterHotKey(NULL, 5, MOD_WIN | MOD_SHIFT, VK_LEFT);  // 좌상단
    RegisterHotKey(NULL, 6, MOD_WIN | MOD_SHIFT, VK_RIGHT); // 우상단
    RegisterHotKey(NULL, 7, MOD_WIN | MOD_SHIFT, VK_UP);    // 상단 중앙
    RegisterHotKey(NULL, 8, MOD_WIN | MOD_SHIFT, VK_DOWN);  // 하단 중앙

    // 기타 단축키
    RegisterHotKey(NULL, 9, MOD_WIN, 'G');  // 그리드 토글
    RegisterHotKey(NULL, 10, MOD_WIN, 'R'); // 창 크기 초기화
}

void WindowManager::UnregisterHotkeys() {
    for (int i = 1; i <= 10; i++) {
        UnregisterHotKey(NULL, i);
    }
}

void WindowManager::ProcessHotkeys() {
    // 메시지 루프에서 WM_HOTKEY 처리
}

void WindowManager::HandleWindowDrag(HWND hwnd) {
    // TODO: 드래그 처리 구현
}

void WindowManager::UpdateGridDisplay() {
    // TODO: 그리드 업데이트 구현
}

bool WindowManager::IsSnapPosition(POINT pt, int& position) {
    // TODO: 스냅 위치 감지 구현
    return false;
}
