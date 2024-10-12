cmake_minimum_required(VERSION 3.29)
project(MCLC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_PREFIX})

find_package(Qt6 COMPONENTS
        Core
        Network
        Concurrent
        REQUIRED)

find_package(OpenSSL REQUIRED)

# MCLC Core
add_subdirectory(src)