add_executable(ViSil_CLI
  main.cpp
)

target_link_libraries(ViSil_CLI
    PUBLIC Arch_RV32_Lib Devices_Lib
    Visil_Includes
)
