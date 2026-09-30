set(RV32_Sources
    register_file.h
    register_file.cpp
    decoder.h
    decoder.cpp
    instruction.h
)

add_library(Arch_RV32_Lib STATIC ${RV32_Sources})

target_include_directories(Arch_RV32_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
