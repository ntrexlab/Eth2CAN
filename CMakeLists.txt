cmake_minimum_required(VERSION 3.0.0)
project(eth2can VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# 소스 파일과 헤더 파일의 디렉토리 설정
set(SOURCES
    src/main.cpp
)

# 헤더 파일이 있는 디렉토리 설정
include_directories(src/include)

add_executable(eth2can ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
