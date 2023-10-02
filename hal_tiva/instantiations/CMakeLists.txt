add_library(hal_tiva.instantiations ${HAL_TI_EXCLUDE_FROM_ALL} STATIC)
install(TARGETS hal_tiva.instantiations EXPORT halTivaTargets)

target_include_directories(hal_tiva.instantiations PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(hal_tiva.instantiations PUBLIC
    hal_tiva.cortex
    hal_tiva.tiva
    hal_tiva.synchronous_tiva
    infra.event
    infra.timer
    services.tracer
)

target_sources(hal_tiva.instantiations PRIVATE
    EventInfrastructure.cpp
    EventInfrastructure.hpp
    LaunchPadBsp.hpp
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C123>:LaunchPadBspEkTm4c123g.hpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:LaunchPadBspEkTm4c1294.hpp>
    TracerInfrastructure.cpp
    TracerInfrastructure.hpp
    TracingReset.cpp
    TracingReset.hpp
)
