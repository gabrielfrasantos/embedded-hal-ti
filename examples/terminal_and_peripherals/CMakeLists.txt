add_executable(hal_tiva.terminal_and_peripherals Main.cpp)
emil_build_for(hal_tiva.terminal_and_peripherals TARGET_MCU_VENDOR ti PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES)

target_link_libraries(hal_tiva.terminal_and_peripherals PRIVATE
    services.util
    #hal_tiva.instantiations
)

hal_ti_target_default_linker_scripts(hal_tiva.terminal_and_peripherals)
hal_ti_target_default_init(hal_tiva.terminal_and_peripherals)

emil_generate_artifacts(TARGET hal_tiva.terminal_and_peripherals LST MAP BIN HEX)
