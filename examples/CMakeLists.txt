set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Test)

if(${QT_VERSION_MAJOR} EQUAL 6)
    add_subdirectory(Gallery)
endif()
add_subdirectory(MCDayNightSwitchButton)
