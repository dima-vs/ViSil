set(Devices_Sources
    ram.h
    ram.cpp
)

add_library(Devices_Lib STATIC ${Devices_Sources})

target_link_libraries(Devices_Lib
    PUBLIC Core_Lib
)

target_include_directories(Devices_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
