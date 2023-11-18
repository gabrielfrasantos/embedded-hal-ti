add_library(hal_tiva.tiva ${HAL_TI_EXCLUDE_FROM_ALL} STATIC)
install(TARGETS hal_tiva.tiva EXPORT halTivaTargets)

target_include_directories(hal_tiva.tiva PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(hal_tiva.tiva PUBLIC
    hal.interfaces
    infra.timer
    ti.hal_driver
    hal_tiva.cortex
)

target_sources(hal_tiva.tiva PRIVATE
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C123>:ClockTm4c123.cpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C123>:ClockTm4c123.hpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:ClockTm4c129.cpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:ClockTm4c129.hpp>
    Dma.cpp
    Dma.hpp
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:Ethernet.cpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:Ethernet.hpp>
    Gpio.cpp
    Gpio.hpp
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C123>:PinoutTableDefaultTm4c123.cpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C123>:PinoutTableDefaultTm4c123.hpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:PinoutTableDefaultTm4c129.cpp>
    $<$<STREQUAL:${TARGET_MCU_FAMILY},TM4C129>:PinoutTableDefaultTm4c129.hpp>
    Pwm.cpp
    Pwm.hpp
    QuadratureEncoder.cpp
    QuadratureEncoder.hpp
    SpiMaster.cpp
    SpiMaster.hpp
    Uart.cpp
    Uart.hpp
    UartWithDma.cpp
    UartWithDma.hpp
    UniqueDeviceId.cpp
    UniqueDeviceId.hpp
)
