add_executable(hal_tiva.blink Main.cpp)
emil_build_for(hal_tiva.blink TARGET_MCU_VENDOR ti PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES)

target_link_libraries(hal_tiva.blink PRIVATE
    services.util
    #hal_tiva.instantiations
)

hal_ti_target_default_linker_scripts(hal_tiva.blink)
hal_ti_target_default_init(hal_tiva.blink)

emil_generate_artifacts(TARGET hal_tiva.blink LST MAP BIN HEX)
