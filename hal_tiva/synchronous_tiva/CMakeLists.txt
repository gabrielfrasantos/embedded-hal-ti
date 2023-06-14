add_library(hal_tiva.synchronous_tiva ${HAL_TI_EXCLUDE_FROM_ALL} STATIC)
install(TARGETS hal_tiva.synchronous_tiva EXPORT halTivaTargets)

target_include_directories(hal_tiva.synchronous_tiva PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(hal_tiva.synchronous_tiva PUBLIC
    hal.synchronous_interfaces
    hal_tiva.tiva
    services.util
)

target_sources(hal_tiva.synchronous_tiva PRIVATE
    SynchronousGpioStm.cpp
    SynchronousGpioStm.hpp
    SynchronousFlashInternalStm.cpp
    SynchronousFlashInternalStm.hpp
    SynchronousSpiMasterStm.cpp
    SynchronousSpiMasterStm.hpp
    SynchronousUartStm.cpp
    SynchronousUartStm.hpp
    TimeKeeperStm.cpp
    TimeKeeperStm.hpp
)
