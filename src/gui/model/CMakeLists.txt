set(Gui_Model_Sources
    memory_block_model.h
    memory_block_model.cpp
    memory_word_model.h
    memory_word_model.cpp
    register_file_model.h
    register_file_model.cpp
    memory_instruction_model.h
    memory_instruction_model.cpp
)

add_library(Gui_Model_Lib STATIC ${Gui_Model_Sources})

set_target_properties(Gui_Model_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_Model_Lib
   PUBLIC
   Visil_Includes
   Core_Lib
   Devices_Lib
   Gui_Common_Lib
   Qt${QT_VERSION_MAJOR}::Core
)
