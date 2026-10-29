set(Gui_Common_Sources
    memory_word_types.h
    register_file_types.h
    memory_instruction_types.h
)

add_library(Gui_Common_Lib INTERFACE ${Gui_Common_Sources})


target_link_libraries(Gui_Common_Lib
   INTERFACE
   Visil_Includes
   Core_Lib
   Qt${QT_VERSION_MAJOR}::Core
)