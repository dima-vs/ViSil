set(Core_Sources
    cpu_interface.h
    bus_device_interface.h
    system_bus.h
    system_bus.cpp
    common.h
    bus_chain_node.h
    execution_env_interface.h
    simulation_runner.h
    simulation_runner.cpp
    endian_utils.h
    disassembler_interface.h
)

add_library(Core_Lib STATIC ${Core_Sources})

target_link_libraries(Core_Lib
    PUBLIC Visil_Includes
)
