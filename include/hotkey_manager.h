#pragma once
#include <windows.h>
#include <map>
#include <functional>

// 핫키 ID 정의
enum class HotkeyId {
    SnapLeft = 1,
    SnapRight,
    SnapTop,
    SnapBottom,
    SnapTopLeft,
    SnapTopRight,
    SnapBottomLeft,
    SnapBottomRight,
    SnapCenter,
    ToggleGrid,
    ResetWindow
};

class HotkeyManager {
public:
    static HotkeyManager& getInstance();

    // 초기화 및 정리
    bool initialize();
    void cleanup();

    // 핫키 등록/해제
    bool registerHotkeys();
    void unregisterHotkeys();

    // 핫키 이벤트 처리
    void handleHotkey(int id);

    // 사용자 정의 핫키 설정
    bool setHotkey(HotkeyId id, UINT modifiers, UINT key);
    
private:
    HotkeyManager();
    ~HotkeyManager();
    
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

    struct HotkeyInfo {
        UINT modifiers;
        UINT key;
    };

    std::map<HotkeyId, HotkeyInfo> m_hotkeyMap;
    bool m_initialized;

    void initializeDefaultHotkeys();
};
