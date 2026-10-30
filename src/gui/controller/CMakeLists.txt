add_subdirectory(devices)

set(Gui_Controller_Sources
    execution_flow_controller.h
    execution_flow_controller.cpp
    simulation_thread.h
    simulation_thread.cpp
    file_controller.h
    file_controller.cpp
)

add_library(Gui_Controller_Lib STATIC ${Gui_Controller_Sources})

set_target_properties(Gui_Controller_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_Controller_Lib
    PUBLIC Visil_Includes
        Gui_View_Lib
        Gui_AppContext_Lib
        Gui_Model_Lib
        Core_Lib
        Devices_Lib
        Gui_Utils_Lib
        Gui_ControllerDevice_Lib
)
