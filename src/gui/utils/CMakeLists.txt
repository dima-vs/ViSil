set(Gui_Utils_Sources
    status_formatter.h
    status_formatter.cpp
)

add_library(Gui_Utils_Lib STATIC ${Gui_Utils_Sources})

set_target_properties(Gui_Utils_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_Utils_Lib
    PUBLIC Visil_Includes
        Core_Lib
        Qt6::Core
)
