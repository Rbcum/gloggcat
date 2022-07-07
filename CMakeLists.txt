cmake_minimum_required(VERSION 3.5)

project(gloggcat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        connection.h
        cmdlistmodel.h cmdlistmodel.cpp
        cmdlistdelegate.h cmdlistdelegate.cpp
        resources.qrc
)

qt_add_executable(gloggcat
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(gloggcat PRIVATE
    Qt::Widgets
    Qt::Sql
)

set_target_properties(gloggcat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gloggcat)
endif()
