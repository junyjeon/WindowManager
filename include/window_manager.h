#pragma once
#include <windows.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

// 창 위치 열거형
enum class WindowPosition {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// 그리드 설정
struct GridSettings {
    int rows = 12;
    int cols = 12;
    bool visible = false;
    float opacity = 0.5f;
};

// 창 레이아웃 정보
struct WindowLayout {
    RECT position;
    bool isMaximized;
    int monitorIndex;
};

class WindowManager {
public:
    static WindowManager& getInstance();
    
    // 초기화 및 정리
    bool initialize();
    void cleanup();

    // 창 관리 기능
    void handleWindowDrag(HWND hwnd, POINT pt);
    void snapWindowToGrid(HWND hwnd, POINT pt);
    void snapWindowToPosition(HWND hwnd, WindowPosition position);
    
    // 그리드 시스템
    void toggleGrid();
    void setGridSize(int rows, int cols);
    void drawGrid(HDC hdc);
    void setGridOpacity(float opacity);
    
    // 레이아웃 관리
    void saveLayout(const std::string& name);
    void loadLayout(const std::string& name);
    void saveWindowState(HWND hwnd);
    void restoreWindowState(HWND hwnd);
    
    // 단축키 처리
    void handleHotkey(int id);
    
    // 다중 모니터 지원
    void updateMonitorInfo();
    int getCurrentMonitorIndex(HWND hwnd);

private:
    WindowManager();  // Singleton
    ~WindowManager();
    
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    // 내부 유틸리티 함수
    RECT calculateWindowPosition(HWND hwnd, WindowPosition position);
    bool isWindowManageable(HWND hwnd);
    void saveConfig();
    void loadConfig();

    // 멤버 변수
    GridSettings m_gridSettings;
    std::map<HWND, WindowLayout> m_windowStates;
    std::map<std::string, std::vector<WindowLayout>> m_savedLayouts;
    std::vector<HMONITOR> m_monitors;
    bool m_initialized;
};
