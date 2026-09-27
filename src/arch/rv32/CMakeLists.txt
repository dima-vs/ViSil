set(RV32_Sources
    register_file.h
    register_file.cpp
)

add_library(RV32_Lib STATIC ${RV32_Sources})

target_include_directories(RV32_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
