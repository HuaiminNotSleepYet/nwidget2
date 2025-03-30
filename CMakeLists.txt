cmake_minimum_required(VERSION 3.10)

project(nwidget2 LANGUAGES CXX)

# set build type to release if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# option test_enable
option(nwidget2_ENABLE_TEST "Enable test" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets
    Core5Compat Charts Svg SvgWidgets OpenGL
    Network QuickWidgets Quick QuickControls2
    Test
)

add_library(nwidget2 INTERFACE)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(nwidget::nwidget2 ALIAS nwidget2)

#Set target properties
target_include_directories(nwidget2 INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (nwidget2_ENABLE_TEST)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    message(STATUS "Enable test")
    add_subdirectory(tests)
endif()