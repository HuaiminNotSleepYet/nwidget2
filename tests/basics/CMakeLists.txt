add_executable(test_basics
    test_basics.cpp
)

set_target_properties(test_basics PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(test_basics PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test

    nwidget::nwidget2
)