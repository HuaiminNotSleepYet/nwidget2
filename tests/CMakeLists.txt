enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

qt_standard_project_setup()

function(add_nwidget_test name src)
    add_executable(${name} ${src})

    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

    target_link_libraries(${name} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
    )

    add_test(NAME ${name} COMMAND ${name})
    set_target_properties(${name} PROPERTIES
        PASS_REGULAR_EXPRESSION "0 failed"
    )
endfunction()

add_nwidget_test(test_metaobj test_metaobj.cpp)
add_nwidget_test(test_builder test_builder.cpp)
add_nwidget_test(test_binding test_binding.cpp)
