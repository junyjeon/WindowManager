#include "hotkey_manager.h"
#include "window_manager.h"
#include <sstream>

HotkeyManager& HotkeyManager::getInstance() {
    static HotkeyManager instance;
    return instance;
}

HotkeyManager::HotkeyManager() : m_initialized(false) {
    initializeDefaultHotkeys();
}

HotkeyManager::~HotkeyManager() {
    cleanup();
}

void HotkeyManager::initializeDefaultHotkeys() {
    // 기본 단축키 설정을 MOD_NOREPEAT 플래그 추가
    m_hotkeyMap[HotkeyId::SnapLeft] = {MOD_WIN | MOD_NOREPEAT, VK_LEFT};
    m_hotkeyMap[HotkeyId::SnapRight] = {MOD_WIN | MOD_NOREPEAT, VK_RIGHT};
    m_hotkeyMap[HotkeyId::SnapTop] = {MOD_WIN | MOD_NOREPEAT, VK_UP};
    m_hotkeyMap[HotkeyId::SnapBottom] = {MOD_WIN | MOD_NOREPEAT, VK_DOWN};

    // 모서리/중앙 정렬 단축키
    m_hotkeyMap[HotkeyId::SnapTopLeft] = {MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_NUMPAD7};
    m_hotkeyMap[HotkeyId::SnapTopRight] = {MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_NUMPAD9};
    m_hotkeyMap[HotkeyId::SnapBottomLeft] = {MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_NUMPAD1};
    m_hotkeyMap[HotkeyId::SnapBottomRight] = {MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_NUMPAD3};
    m_hotkeyMap[HotkeyId::SnapCenter] = {MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_NUMPAD5};

    // 기타 기능키 - 충돌 가능성이 적은 키 조합으로 변경
    m_hotkeyMap[HotkeyId::ToggleGrid] = {MOD_ALT | MOD_NOREPEAT, 'G'};
    m_hotkeyMap[HotkeyId::ResetWindow] = {MOD_ALT | MOD_NOREPEAT, 'R'};
}

bool HotkeyManager::initialize() {
    if (m_initialized) return true;
    
    // 이전에 등록된 핫키가 있다면 모두 해제
    unregisterHotkeys();

    // 핫키 등록 시도
    bool success = true;
    std::wstringstream errorMsg;
    
    for (const auto& [id, info] : m_hotkeyMap) {
        if (!RegisterHotKey(NULL, static_cast<int>(id), info.modifiers, info.key)) {
            DWORD error = GetLastError();
            if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
                // 이미 등록된 핫키는 건너뛰기
                continue;
            }
            
            success = false;
            errorMsg << L"핫키 등록 실패 (ID: " << static_cast<int>(id) 
                    << L", Error: " << error << L")\n";
        }
    }

    if (!success) {
        // 에러 메시지 표시
        MessageBoxW(NULL, errorMsg.str().c_str(), L"HotkeyManager 초기화 실패", MB_OK | MB_ICONERROR);
        unregisterHotkeys();
        return false;
    }

    m_initialized = true;
    return true;
}

void HotkeyManager::cleanup() {
    if (!m_initialized) return;
    
    unregisterHotkeys();
    m_initialized = false;
}

void HotkeyManager::unregisterHotkeys() {
    for (const auto& [id, _] : m_hotkeyMap) {
        UnregisterHotKey(NULL, static_cast<int>(id));
    }
}

void HotkeyManager::handleHotkey(int id) {
    auto& windowManager = WindowManager::getInstance();
    HWND foregroundWindow = GetForegroundWindow();

    if (!foregroundWindow) return;

    switch (static_cast<HotkeyId>(id)) {
        case HotkeyId::SnapLeft:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::CenterLeft);
            break;
        case HotkeyId::SnapRight:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::CenterRight);
            break;
        case HotkeyId::SnapTop:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::TopCenter);
            break;
        case HotkeyId::SnapBottom:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::BottomCenter);
            break;
        case HotkeyId::SnapTopLeft:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::TopLeft);
            break;
        case HotkeyId::SnapTopRight:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::TopRight);
            break;
        case HotkeyId::SnapBottomLeft:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::BottomLeft);
            break;
        case HotkeyId::SnapBottomRight:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::BottomRight);
            break;
        case HotkeyId::SnapCenter:
            windowManager.snapWindowToPosition(foregroundWindow, WindowPosition::Center);
            break;
        case HotkeyId::ToggleGrid:
            windowManager.toggleGrid();
            break;
        case HotkeyId::ResetWindow:
            ShowWindow(foregroundWindow, SW_RESTORE);
            break;
    }
}

bool HotkeyManager::setHotkey(HotkeyId id, UINT modifiers, UINT key) {
    if (!m_initialized) return false;

    // 기존 핫키 해제
    UnregisterHotKey(NULL, static_cast<int>(id));

    // 새 핫키 등록
    if (!RegisterHotKey(NULL, static_cast<int>(id), modifiers | MOD_NOREPEAT, key)) {
        // 실패 시 기존 핫키 재등록
        const auto& oldInfo = m_hotkeyMap[id];
        RegisterHotKey(NULL, static_cast<int>(id), oldInfo.modifiers, oldInfo.key);
        return false;
    }

    // 성공 시 맵 업데이트
    m_hotkeyMap[id] = {modifiers | MOD_NOREPEAT, key};
    return true;
}
