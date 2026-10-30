set(Gui_ModelDevice_Sources
    uart_console_model.h
    uart_console_model.cpp
)

add_library(Gui_ModelDevice_Lib STATIC ${Gui_ModelDevice_Sources})

set_target_properties(Gui_ModelDevice_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_ModelDevice_Lib
    PUBLIC Visil_Includes
        Qt${QT_VERSION_MAJOR}::Widgets
        Gui_Common_Lib
)

