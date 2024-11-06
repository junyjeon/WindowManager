#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <windows.h>
#include <vector>
#include <map>
#include <string>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // 초기화 및 실행
    bool Initialize();
    void Run();
    void Shutdown();

    // 창 관리 기능
    void SnapWindowToPosition(HWND hwnd, int position);
    void ShowGrid(bool show);
    void SaveLayout(const std::string& name);
    void LoadLayout(const std::string& name);
    void SaveWindowState(HWND hwnd);
    void RestoreWindowState(HWND hwnd);

private:
    // 그리드 시스템
    struct Grid {
        int rows;
        int cols;
        bool visible;
        float opacity;
    };

    // 창 상태
    struct WindowState {
        RECT position;
        bool isMaximized;
    };

    // 멤버 변수
    Grid m_grid;
    std::map<HWND, WindowState> m_windowStates;
    std::map<std::string, std::vector<WindowState>> m_savedLayouts;
    bool m_isRunning;

    // 내부 헬퍼 함수
    void ProcessHotkeys();
    void HandleWindowDrag(HWND hwnd);
    void UpdateGridDisplay();
    bool IsSnapPosition(POINT pt, int& position);
    void RegisterHotkeys();
    void UnregisterHotkeys();
};

// 창 위치 상수
enum SnapPosition {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER,
    TOP_CENTER,
    LEFT_CENTER,
    RIGHT_CENTER
};

#endif // WINDOW_MANAGER_H
