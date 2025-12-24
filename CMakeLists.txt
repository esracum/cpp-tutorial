cmake_minimum_required(VERSION 3.5)

project(Object_Detect VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. QT PAKETLERİNİ BUL ---
# Network modülünü buraya ekledik
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network LinguistTools REQUIRED)

# --- 2. OPENCV PAKETİNİ BUL ---
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
find_package(OpenCV 4.10 REQUIRED)

set(TS_FILES Object_Detect_tr_TR.ts)

set(PROJECT_SOURCES
        main.cpp
        objectdetect.cpp
        objectdetect.h
        objectdetect.ui
        ${TS_FILES}
)

# --- 3. EXECUTABLE OLUŞTUR ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Object_Detect
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Object_Detect SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Object_Detect
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# --- 4. KÜTÜPHANELERİ BAĞLA ---
# OpenCV ve Qt Network burada sisteme dahil ediliyor
target_link_libraries(Object_Detect PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    ${OpenCV_LIBRARIES}
)

set_target_properties(Object_Detect PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Object_Detect)
endif()
