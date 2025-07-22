enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

function(add_nwidget_test name src)
    add_executable(${name} ${src})

    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

    target_link_libraries(${name} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
    )

    add_test(NAME ${name} COMMAND ${name})
    set_target_properties(${name} PROPERTIES
        PASS_REGULAR_EXPRESSION "0 failed"
    )
endfunction()

add_nwidget_test(test_metaobj test_metaobj.cpp)
add_nwidget_test(test_builder test_builder.cpp)
add_nwidget_test(test_binding test_binding.cpp)
