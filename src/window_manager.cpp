#include "window_manager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

WindowManager& WindowManager::getInstance() {
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager() : m_initialized(false) {
    m_gridSettings.rows = 12;
    m_gridSettings.cols = 12;
    m_gridSettings.visible = false;
    m_gridSettings.opacity = 0.5f;
}

WindowManager::~WindowManager() {
    cleanup();
}

bool WindowManager::initialize() {
    if (m_initialized) return true;
    
    updateMonitorInfo();
    loadConfig();
    m_initialized = true;
    return true;
}

void WindowManager::cleanup() {
    if (!m_initialized) return;
    
    saveConfig();
    m_windowStates.clear();
    m_savedLayouts.clear();
    m_monitors.clear();
    m_initialized = false;
}

void WindowManager::handleWindowDrag(HWND hwnd, POINT pt) {
    if (!isWindowManageable(hwnd)) return;

    // 현재 모니터 정보 가져오기
    int monitorIndex = getCurrentMonitorIndex(hwnd);
    if (monitorIndex < 0) return;

    // 그리드에 스냅
    if (m_gridSettings.visible) {
        snapWindowToGrid(hwnd, pt);
    }
}

void WindowManager::snapWindowToGrid(HWND hwnd, POINT pt) {
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMonitor, &mi)) return;

    // 작업 영역 크기 계산
    int workWidth = mi.rcWork.right - mi.rcWork.left;
    int workHeight = mi.rcWork.bottom - mi.rcWork.top;

    // 그리드 셀 크기 계산
    int cellWidth = workWidth / m_gridSettings.cols;
    int cellHeight = workHeight / m_gridSettings.rows;

    // 가장 가까운 그리드 라인에 스냅
    int gridX = ((pt.x - mi.rcWork.left + cellWidth / 2) / cellWidth) * cellWidth + mi.rcWork.left;
    int gridY = ((pt.y - mi.rcWork.top + cellHeight / 2) / cellHeight) * cellHeight + mi.rcWork.top;

    // 창 크기 조정
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    SetWindowPos(hwnd, NULL, gridX, gridY, width, height, SWP_NOZORDER | SWP_NOSIZE);
}

void WindowManager::snapWindowToPosition(HWND hwnd, WindowPosition position) {
    RECT windowRect = calculateWindowPosition(hwnd, position);
    SetWindowPos(hwnd, NULL, 
                windowRect.left, windowRect.top,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                SWP_NOZORDER);
}

RECT WindowManager::calculateWindowPosition(HWND hwnd, WindowPosition position) {
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &mi);

    RECT workArea = mi.rcWork;
    int width = (workArea.right - workArea.left) / 2;
    int height = (workArea.bottom - workArea.top) / 2;
    RECT result = {0, 0, width, height};

    switch (position) {
        case WindowPosition::TopLeft:
            result = {workArea.left, workArea.top, workArea.left + width, workArea.top + height};
            break;
        case WindowPosition::TopCenter:
            result = {workArea.left + width/2, workArea.top, workArea.right - width/2, workArea.top + height};
            break;
        case WindowPosition::TopRight:
            result = {workArea.right - width, workArea.top, workArea.right, workArea.top + height};
            break;
        case WindowPosition::CenterLeft:
            result = {workArea.left, workArea.top + height/2, workArea.left + width, workArea.bottom - height/2};
            break;
        case WindowPosition::Center:
            result = {workArea.left + width/2, workArea.top + height/2, 
                     workArea.right - width/2, workArea.bottom - height/2};
            break;
        case WindowPosition::CenterRight:
            result = {workArea.right - width, workArea.top + height/2, workArea.right, workArea.bottom - height/2};
            break;
        case WindowPosition::BottomLeft:
            result = {workArea.left, workArea.bottom - height, workArea.left + width, workArea.bottom};
            break;
        case WindowPosition::BottomCenter:
            result = {workArea.left + width/2, workArea.bottom - height, 
                     workArea.right - width/2, workArea.bottom};
            break;
        case WindowPosition::BottomRight:
            result = {workArea.right - width, workArea.bottom - height, workArea.right, workArea.bottom};
            break;
    }

    return result;
}

void WindowManager::toggleGrid() {
    m_gridSettings.visible = !m_gridSettings.visible;
    // 화면 갱신 요청
    InvalidateRect(NULL, NULL, TRUE);
}

void WindowManager::setGridSize(int rows, int cols) {
    m_gridSettings.rows = rows;
    m_gridSettings.cols = cols;
    if (m_gridSettings.visible) {
        InvalidateRect(NULL, NULL, TRUE);
    }
}

void WindowManager::drawGrid(HDC hdc) {
    if (!m_gridSettings.visible) return;

    HMONITOR hMonitor = MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMonitor, &mi)) return;

    // 반투명 브러시 생성
    BYTE alpha = static_cast<BYTE>(m_gridSettings.opacity * 255);
    HBRUSH hBrush = CreateSolidBrush(RGB(200, 200, 200));
    
    int cellWidth = (mi.rcWork.right - mi.rcWork.left) / m_gridSettings.cols;
    int cellHeight = (mi.rcWork.bottom - mi.rcWork.top) / m_gridSettings.rows;

    // 수직선 그리기
    for (int i = 1; i < m_gridSettings.cols; i++) {
        int x = mi.rcWork.left + i * cellWidth;
        MoveToEx(hdc, x, mi.rcWork.top, NULL);
        LineTo(hdc, x, mi.rcWork.bottom);
    }

    // 수평선 그리기
    for (int i = 1; i < m_gridSettings.rows; i++) {
        int y = mi.rcWork.top + i * cellHeight;
        MoveToEx(hdc, mi.rcWork.left, y, NULL);
        LineTo(hdc, mi.rcWork.right, y);
    }

    DeleteObject(hBrush);
}

void WindowManager::updateMonitorInfo() {
    m_monitors.clear();
    EnumDisplayMonitors(NULL, NULL, 
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(lParam);
            monitors->push_back(hMonitor);
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&m_monitors));
}

int WindowManager::getCurrentMonitorIndex(HWND hwnd) {
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    auto it = std::find(m_monitors.begin(), m_monitors.end(), hMonitor);
    return it != m_monitors.end() ? static_cast<int>(std::distance(m_monitors.begin(), it)) : -1;
}

bool WindowManager::isWindowManageable(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) return false;
    
    // 시스템 창 제외
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW)) return false;
    
    return true;
}

void WindowManager::saveConfig() {
    // 설정 파일에 현재 상태 저장
    std::ofstream file("window_manager.config");
    if (!file) return;

    file << m_gridSettings.rows << " " << m_gridSettings.cols << " "
         << m_gridSettings.opacity << std::endl;
    
    file.close();
}

void WindowManager::loadConfig() {
    // 설정 파일에서 상태 로드
    std::ifstream file("window_manager.config");
    if (!file) return;

    file >> m_gridSettings.rows >> m_gridSettings.cols >> m_gridSettings.opacity;
    file.close();
}
