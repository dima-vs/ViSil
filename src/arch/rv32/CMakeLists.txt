set(RV32_Sources
    register_file.h
    register_file.cpp
    decoder.h
    decoder.cpp
    instruction.h
    execution_unit.h
    execution_unit.cpp
    cpu.h
    cpu.cpp
    rv32_disassembler.h
    rv32_disassembler.cpp
)

add_library(Arch_RV32_Lib STATIC ${RV32_Sources})

target_link_libraries(Arch_RV32_Lib
    PUBLIC Core_Lib
    Visil_Includes
)
