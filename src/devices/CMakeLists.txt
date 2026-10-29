set(Devices_Sources
    ram.h
    ram.cpp
    uart.h
    uart.cpp
)

add_library(Devices_Lib STATIC ${Devices_Sources})

target_link_libraries(Devices_Lib
    PUBLIC Core_Lib Visil_Includes
)
