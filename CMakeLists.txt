cmake_minimum_required(VERSION 3.23)
project(MCLC)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CLI11 REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 23)

if(WIN32)
        set(CMAKE_CXX_FLAGS "-std=c++23 -fcoroutines -fpermissive -lwsock32 -lws2_32 -lwinmm")
else()
        set(CMAKE_CXX_FLAGS "-std=c++23 -fcoroutines -fpermissive -Wreturn-type -Wunused-result")
endif()

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp)

file(GLOB_RECURSE CURRENT_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.hpp)

include_directories(include)
include_directories(src)

add_executable(MCLC ${SOURCE_FILES} ${CURRENT_HEADERS}
        src/coremain.cpp)
if(WIN32)
        target_link_libraries(MCLC PRIVATE OpenSSL::applink)
endif()
target_link_libraries(MCLC PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        CLI11::CLI11)