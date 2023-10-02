function(add_hal_driver target_name manufacturer manufacturer_family)

    add_library(${target_name} STATIC)

    install(TARGETS ${target_name} EXPORT halTivaTargets)

    file(GLOB manufacturer_include RELATIVE ${CMAKE_CURRENT_LIST_DIR} CMSIS/Device/${manufacturer}/${manufacturer_family}*)

    target_include_directories(${target_name} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/CMSIS/Core/Include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${manufacturer_include}/Include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/CMSIS/Core/Include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${manufacturer_include}/Include>"
    )

    file(GLOB sources_in RELATIVE ${CMAKE_CURRENT_LIST_DIR}
        CMSIS/Core/Include/*.h
        CMSIS/Device/${manufacturer}/${manufacturer_family}/Include/*.h
        CMSIS/Device/${manufacturer}/${manufacturer_family}/Source/Templates/system_*.c
    )

    set(sources)
    foreach(source ${sources_in})
        if (NOT ${source} MATCHES "_template.")
            list(APPEND sources ${source})
        endif()
    endforeach()

    target_sources(${target_name} PRIVATE
        ${sources}
    )

    file(GLOB startup_source
        CMSIS/Device/${manufacturer}/${manufacturer_family}/Source/Templates/gcc/startup_${manufacturer_family}.c
    )

    if (startup_source)
        set_target_properties(${target_name} PROPERTIES HAL_TI_STARTUP_SOURCE ${startup_source})
    endif()

endfunction()

define_property(TARGET PROPERTY HAL_TI_STARTUP_SOURCE BRIEF_DOCS "Startup source" FULL_DOCS "The source file that contains the startup and interrupt vector code.")
