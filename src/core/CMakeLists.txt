set(Core_Sources
    cpu_interface.h
    bus_device_interface.h
    system_bus.h
    system_bus.cpp
    primitives.h
    ram.h
    ram.cpp
    bus_chain_node.h
)

add_library(Core_Lib STATIC ${Core_Sources})

target_include_directories(Core_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
