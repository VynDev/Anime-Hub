cmake_minimum_required(VERSION 3.5)

project(AnimeHub LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

set(SOURCE_DIR source)

set(PROJECT_SOURCES
        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/AnimeHub.cpp
        ${SOURCE_DIR}/AnimeHub.h
        ${SOURCE_DIR}/AnimeHub.ui
        ${SOURCE_DIR}/Anime.cpp
        ${SOURCE_DIR}/Anime.h
        ${SOURCE_DIR}/AnimePreviewUI.cpp
        ${SOURCE_DIR}/AnimePreviewUI.h
        ${SOURCE_DIR}/AnimePreviewUI.ui
        ${SOURCE_DIR}/SettingsUI.cpp
        ${SOURCE_DIR}/SettingsUI.h
        ${SOURCE_DIR}/SettingsUI.ui
        ${SOURCE_DIR}/Settings.cpp
        ${SOURCE_DIR}/Settings.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AnimeHub
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(AnimeHub SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(AnimeHub
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(AnimeHub PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(AnimeHub PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_include_directories(AnimeHub PRIVATE /home/vyn/Development/Projects/Qt/AnimeHub/libs/json-parser/include)
target_link_libraries(AnimeHub PRIVATE -L/home/vyn/Development/Projects/Qt/AnimeHub/libs/json-parser/bin)
target_link_libraries(AnimeHub PRIVATE -ljson-parser)
