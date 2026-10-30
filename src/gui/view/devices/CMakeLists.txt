set(Gui_ViewDevice_Sources
    uart_console_view.h
    uart_console_view.cpp
)

add_library(Gui_ViewDevice_Lib STATIC ${Gui_ViewDevice_Sources})

set_target_properties(Gui_ViewDevice_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_ViewDevice_Lib
    PUBLIC Visil_Includes
        Qt${QT_VERSION_MAJOR}::Widgets
        Gui_Common_Lib
)

