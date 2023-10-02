include(hal_driver.cmake)

add_hal_driver(ti.hal_driver_${TARGET_MCU_FAMILY} TI ${TARGET_MCU_FAMILY})
emil_build_for(ti.hal_driver_${TARGET_MCU_FAMILY} TARGET_MCU_FAMILY ${TARGET_MCU_FAMILY} PREREQUISITE_BOOL HAL_TI_STANDALONE)

target_compile_definitions(ti.hal_driver_${TARGET_MCU_FAMILY} PUBLIC
    DEVICE_HEADER="TIVA.h"
    ${TARGET_MCU_FAMILY}
    $<$<STREQUAL:${TARGET_MCU},tm4c123gh6pm>:TM4C123GH6PM>
    $<$<STREQUAL:${TARGET_MCU},tm4c1294ncpdt>:TM4C1294NCPDT>
)

add_library(ti.hal_driver ${HAL_TI_EXCLUDE_FROM_ALL} INTERFACE)

install(TARGETS ti.hal_driver EXPORT halTivaTargets)

if (TARGET_MCU_VENDOR STREQUAL ti)
    target_link_libraries(ti.hal_driver INTERFACE
        ti.hal_driver_${TARGET_MCU_FAMILY}
    )
endif()

function(hat_ti_target_linker_scripts)
    set(singleArgs TARGET)
    set(multiArgs LINKER_SCRIPTS)

    cmake_parse_arguments(PARSE_ARGV 0 HAL_TI "" "${singleArgs}" "${multiArgs}")

    foreach(script ${HAL_TI_LINKER_SCRIPTS})
        cmake_path(IS_RELATIVE script is_relative)
        if (is_relative)
            target_link_options(${HAL_TI_TARGET} PUBLIC LINKER:-T${CMAKE_CURRENT_LIST_DIR}/${script})
        else()
            target_link_options(${HAL_TI_TARGET} PUBLIC LINKER:-T${script})
        endif()
    endforeach()

    set_property(TARGET ${HAL_TI_TARGET} PROPERTY INTERFACE_LINK_DEPENDS ${HAL_TI_LINKER_SCRIPTS})
endfunction()

function(hal_ti_target_default_linker_scripts target)
    hat_ti_target_linker_scripts(TARGET ${target} LINKER_SCRIPTS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ldscripts/mem_${TARGET_MCU}.ld ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ldscripts/sections.ld)
endfunction()

emil_exclude_directory_from_clang_format(.)
