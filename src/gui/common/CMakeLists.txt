set(Gui_Common_Sources
    memory_word_types.h
    register_file_types.h
)

add_library(Gui_Common_Lib INTERFACE ${Gui_Common_Sources})


target_link_libraries(Gui_Common_Lib
   INTERFACE
   Core_Lib
   Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(Gui_Common_Lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
