cmake_minimum_required(VERSION 3.12)

project(MCDayNightSwitchButton VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(MCDayNightSwitchButton
    mcdaynightswitchbutton.cpp
    main.cpp
)

target_link_libraries(MCDayNightSwitchButton
PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    nwidget
)
