set(Gui_View_Sources
    mainwindow.h
    mainwindow.cpp
    memory_block_view.h
    memory_block_view.cpp
    memory_block_delegate.h
    memory_block_delegate.cpp
    theme_manager.h
    theme_manager.cpp
    memory_word_view.h
    memory_word_view.cpp
    memory_word_delegate.h
    memory_word_delegate.cpp
    memory_word_table_header.h
    memory_word_table_header.cpp
    register_file_view.h
    register_file_view.cpp
    register_file_delegate.h
    register_file_delegate.cpp
    memory_instruction_view.h
    memory_instruction_view.cpp
    memory_instruction_delegate.h
    memory_instruction_delegate.cpp
    # debugger_panel.h
    # debugger_panel.cpp

    assets/resources_styles.qrc
    assets/resources_icons.qrc
)

add_library(Gui_View_Lib STATIC ${Gui_View_Sources})

set_target_properties(Gui_View_Lib PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(Gui_View_Lib
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Gui_Common_Lib
)

target_include_directories(Gui_View_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
