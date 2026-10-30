set(Gui_ControllerDevice_Sources
    uart_console_controller.h
    uart_console_controller.cpp
)

add_library(Gui_ControllerDevice_Lib STATIC ${Gui_ControllerDevice_Sources})

set_target_properties(Gui_ControllerDevice_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_ControllerDevice_Lib
    PUBLIC Visil_Includes
        Gui_View_Lib
        Gui_AppContext_Lib
        Gui_Model_Lib
        Core_Lib
        Devices_Lib
        Gui_Utils_Lib
)
