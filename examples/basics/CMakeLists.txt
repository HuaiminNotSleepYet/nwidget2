find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets
    Test
)

add_executable(example_basics
    main.cpp
    mainwindow.cpp
    mainwindow.h

    # ${WIN32_RESOURCES}
    # ${RESOURCES}
)

set_target_properties(example_basics PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    WIN32_EXECUTABLE true
)
if(MSVC)
    target_link_options(example_basics PRIVATE

    )
endif()

target_link_libraries(example_basics
    PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets

    nwidget::nwidget2
)

target_include_directories(example_basics
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
) 