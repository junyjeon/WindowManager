cmake_minimum_required(VERSION 3.10)
project(WindowManager)

# 크로스 컴파일 설정
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 소스 파일 목록
set(SOURCES
    src/main.cpp
    src/WindowManager.cpp
)

# 헤더 파일 디렉토리
include_directories(include)

# 실행 파일 생성 (Windows GUI 애플리케이션)
add_executable(WindowManager WIN32 ${SOURCES})

# Windows 라이브러리 링크
target_link_libraries(WindowManager
    -luser32
    -lgdi32
    -lshell32
    -lole32
    -luuid
    -ldwmapi
)

# 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
