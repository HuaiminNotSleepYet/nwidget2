add_executable(example_basics
    main.cpp
    mainwindow.cpp
    mainwindow.hpp

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
)

target_include_directories(example_basics
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
) 