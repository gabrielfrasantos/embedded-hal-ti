add_library(hal_tiva.instantiations ${HAL_TI_EXCLUDE_FROM_ALL} STATIC)
install(TARGETS hal_tiva.instantiations EXPORT halTivaTargets)

target_include_directories(hal_tiva.instantiations PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(hal_tiva.instantiations PUBLIC
    hal_tiva.tiva
    hal_tiva.synchronous_tiva
    infra.event
    infra.timer
    services.tracer
)

target_sources(hal_tiva.instantiations PRIVATE
    DiscoveryTracerInfrastructure.hpp
    DiscoveryUi.hpp
    NucleoTracerInfrastructure.hpp
    NucleoUi.hpp
    StmEventInfrastructure.cpp
    StmEventInfrastructure.hpp
    StmTracerInfrastructure.cpp
    StmTracerInfrastructure.hpp
    TracingResetStm.cpp
    TracingResetStm.hpp
)
