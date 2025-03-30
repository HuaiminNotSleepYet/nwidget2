cmake_minimum_required(VERSION 3.12)

project(nwidget VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(nwidget INTERFACE)

target_include_directories(nwidget
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(GNUInstallDirs)

install(TARGETS nwidget
    EXPORT nwidget-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/nwidget-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/nwidget-config.cmake.in
    ${PROJECT_BINARY_DIR}/nwidget-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nwidget/cmake
)

install(EXPORT nwidget-targets
    FILE nwidget-targets.cmake
    NAMESPACE nwidget::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nwidget/cmake
)

install(FILES
    ${PROJECT_BINARY_DIR}/nwidget-config.cmake
    ${PROJECT_BINARY_DIR}/nwidget-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nwidget/cmake
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/nwidget DESTINATION include)
