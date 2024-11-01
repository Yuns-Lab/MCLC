# Launcher Modules

add_library(Launcher SHARED)

# Core Modules

add_library(Core SHARED
        core/Logger.cpp
        core/Logger.h
)

# Network Modules

add_library(Network SHARED
        network/Downloader.h
        network/utils/HttpUtil.cpp
        network/utils/HttpUtil.h
        network/Downloader.cpp
)

# Linking Libraries

target_link_libraries(Network Core
        Qt6::Network
        Qt6::Concurrent
        Qt6::Core
        OpenSSL::SSL
        OpenSSL::Crypto)
target_link_libraries(Launcher Qt6::Core)
target_link_libraries(Core Qt6::Core)

# Executable

add_library(MCLC SHARED MCLC.cpp)
target_link_libraries(MCLC PUBLIC Core Network Launcher)

# Post build on Windows

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Concurrent Network)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
    foreach (LIB_CXX libc++ libunwind libgcc_s_seh-1 libstdc++-6 libwinpthread-1)
        if(EXISTS "${CMAKE_PREFIX_PATH}/bin/${LIB_CXX}.dll")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/${LIB_CXX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endif ()
    endforeach (LIB_CXX)
endif ()