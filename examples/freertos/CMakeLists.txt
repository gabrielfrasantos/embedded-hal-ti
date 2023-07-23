add_executable(hal_tiva.freertos Main.cpp)
emil_build_for(hal_tiva.freertos TARGET_MCU_VENDOR ti PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES_FREERTOS)

target_link_libraries(hal_tiva.freertos PRIVATE
    #hal_tiva.instantiations
    osal.thread
    osal.freertos_system_time
)

hal_ti_target_default_linker_scripts(hal_tiva.freertos)
hal_ti_target_default_init(hal_tiva.freertos)

emil_generate_artifacts(TARGET hal_tiva.freertos LST MAP BIN HEX)
