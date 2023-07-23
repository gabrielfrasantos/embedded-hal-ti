add_library(hal_tiva.cortex ${HAL_TI_EXCLUDE_FROM_ALL} STATIC)
install(TARGETS hal_tiva.cortex EXPORT halTivaTargets)

target_include_directories(hal_tiva.cortex PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(hal_tiva.cortex PUBLIC
    hal.interfaces
    ti.hal_driver
    infra.event
)

target_sources(hal_tiva.cortex PRIVATE
    EventDispatcherCortex.cpp
    EventDispatcherCortex.hpp
    InterruptCortex.cpp
    InterruptCortex.hpp
    Reset.cpp
    Reset.hpp
    SystemTick.cpp
    SystemTick.hpp
    SystemTickTimerService.cpp
    SystemTickTimerService.hpp
    TimeKeeper.cpp
    TimeKeeper.hpp
)
