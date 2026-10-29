set(Gui_AppContext_Sources
    app_models.h
)

add_library(Gui_AppContext_Lib INTERFACE ${Gui_AppContext_Sources})

set_target_properties(Gui_AppContext_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_AppContext_Lib
    INTERFACE
        Visil_Includes
        Qt${QT_VERSION_MAJOR}::Core
        Gui_Model_Lib
)
