set(Gui_AppContext_Sources
    app_models.h
)

add_library(Gui_AppContext_Lib INTERFACE ${Gui_AppContext_Sources})

set_target_properties(Gui_AppContext_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_AppContext_Lib
    INTERFACE
        Qt${QT_VERSION_MAJOR}::Core
        Gui_Model_Lib
)

target_include_directories(Gui_AppContext_Lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
