add_executable(hal_tiva.blink_nucleo144 Main.cpp)
emil_build_for(hal_tiva.blink_nucleo144 TARGET_MCU_VENDOR st PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES)

target_compile_definitions(hal_tiva.blink_nucleo144 PUBLIC
    NUCLEO=Nucleo144Ui
)

target_link_libraries(hal_tiva.blink_nucleo144 PRIVATE
    services.util
    hal_tiva.instantiations
)

hal_ti_target_default_linker_scripts(hal_tiva.blink_nucleo144)
hal_ti_target_default_init(hal_tiva.blink_nucleo144)

emil_generate_artifacts(TARGET hal_tiva.blink_nucleo144 LST MAP BIN HEX)

######################################################################

add_executable(hal_tiva.blink_nucleo64 Main.cpp)
emil_build_for(hal_tiva.blink_nucleo64 TARGET_MCU_VENDOR st PREREQUISITE_BOOL HAL_TI_BUILD_EXAMPLES)

target_compile_definitions(hal_tiva.blink_nucleo64 PUBLIC
    NUCLEO=Nucleo64Ui
)

target_link_libraries(hal_tiva.blink_nucleo64 PRIVATE
    services.util
    hal_tiva.instantiations
)

hal_ti_target_default_linker_scripts(hal_tiva.blink_nucleo64)
hal_ti_target_default_init(hal_tiva.blink_nucleo64)

emil_generate_artifacts(TARGET hal_tiva.blink_nucleo64 LST MAP BIN HEX)
