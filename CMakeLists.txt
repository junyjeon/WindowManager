cmake_minimum_required(VERSION 3.10)
project(WindowManager)

# C++ 17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC 컴파일러 옵션
if(MSVC)
    # 유니코드 문자셋 사용
    add_compile_definitions(UNICODE _UNICODE)
    
    # 경고 레벨 설정
    add_compile_options(/W4)
    
    # 멀티프로세서 컴파일 활성화
    add_compile_options(/MP)
    
    # 최적화 옵션 (Release 빌드)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()

# 실행 파일 생성
add_executable(WindowManager WIN32 
    src/simple_manager.cpp
)

# Windows API 라이브러리 링크
target_link_libraries(WindowManager PRIVATE
    user32     # 기본 윈도우 API
    gdi32      # 그래픽스
    shell32    # 쉘 API (시스템 트레이 아이콘)
    dwmapi     # DWM API
)

# 출력 디렉토리 설정
set_target_properties(WindowManager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)
