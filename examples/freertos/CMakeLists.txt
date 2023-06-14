add_executable(hal_tiva.freertos_nucleo144 Main.cpp)
emil_build_for(hal_tiva.freertos_nucleo144 TARGET_MCU_VENDOR st PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES_FREERTOS)

target_link_libraries(hal_tiva.freertos_nucleo144 PRIVATE
    hal_tiva.instantiations
    osal.thread
    osal.freertos_system_time
)

hal_ti_target_default_linker_scripts(hal_tiva.freertos_nucleo144)
hal_ti_target_default_init(hal_tiva.freertos_nucleo144)

emil_generate_artifacts(TARGET hal_tiva.freertos_nucleo144 LST MAP BIN HEX)
