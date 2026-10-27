find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(Gui_Sources
    main.cpp
)

qt_add_executable(ViSil_GUI
    ${Gui_Sources}
)

add_subdirectory(common)
add_subdirectory(model)
add_subdirectory(view)
add_subdirectory(app_context)
add_subdirectory(controller)

set_target_properties(ViSil_GUI PROPERTIES
    AUTOMOC ON
)

target_link_libraries(ViSil_GUI
    PRIVATE
        Gui_View_Lib
        Gui_Model_Lib
        Gui_AppContext_Lib
        Gui_Controller_Lib
        Arch_RV32_Lib
)
