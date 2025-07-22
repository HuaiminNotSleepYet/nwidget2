cmake_minimum_required(VERSION 3.12)

project(MCDayNightSwitchButton VERSION 0.1.0 LANGUAGES CXX)

add_executable(MCDayNightSwitchButton
    mcdaynightswitchbutton.cpp
    main.cpp
)

target_link_libraries(MCDayNightSwitchButton
PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    nwidget
)
