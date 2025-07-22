cmake_minimum_required(VERSION 3.5)

project(Gallery VERSION 0.1 LANGUAGES CXX)

add_executable(Gallery
    gallery.h
    gallery.cpp
    main.cpp
)

target_link_libraries(Gallery PRIVATE
    nwidget
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

include(GNUInstallDirs)
install(TARGETS Gallery
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
