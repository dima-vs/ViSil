set(Gui_Model_Sources
    memory_block_model.h
    memory_block_model.cpp
    memory_word_model.h
    memory_word_model.cpp
)

add_library(Gui_Model_Lib STATIC ${Gui_Model_Sources})

set_target_properties(Gui_Model_Lib PROPERTIES
    AUTOMOC ON
)

target_link_libraries(Gui_Model_Lib
   PUBLIC
   Core_Lib
   Devices_Lib
   Gui_Common_Lib
   Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(Gui_Model_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
