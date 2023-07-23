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
#    AnalogToDigitalPinStm.cpp
#    AnalogToDigitalPinStm.hpp
#    CanStm.cpp
#    CanStm.hpp
    $<$<STREQUAL:${TARGET_MCU},tm4c123gh6pm>:DefaultClockEkTm4c123gxl.cpp>
    $<$<STREQUAL:${TARGET_MCU},tm4c123gh6pm>:DefaultClockEkTm4c123gxl.hpp>
#    DigitalToAnalogPinStm.cpp
#    DigitalToAnalogPinStm.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:DmaStm.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:DmaStm.hpp>
#    FlashInternalStm.cpp
#    FlashInternalStm.hpp
    Gpio.cpp
    Gpio.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:I2cStm.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:I2cStm.hpp>
    PinoutTableDefault.cpp
    PinoutTableDefault.hpp
    Pwm.cpp
    Pwm.hpp
    QuadratureEncoder.cpp
    QuadratureEncoder.hpp
#    SpiDataSizeConfiguratorStm.cpp
#    SpiDataSizeConfiguratorStm.hpp
    SpiMaster.cpp
    SpiMaster.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiMasterStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiMasterStmDma.hpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiSlaveStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiSlaveStmDma.hpp>
    Uart.cpp
    Uart.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStmDma.hpp>
#    $<$<OR:$<STREQUAL:${TARGET_MCU},stm32wb55>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32f7xx>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32g4xx>>:UartStmDuplexDma.cpp>
#    $<$<OR:$<STREQUAL:${TARGET_MCU},stm32wb55>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32f7xx>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32g4xx>>:UartStmDuplexDma.hpp>
#    UsbLinkLayerStm.cpp
#    UsbLinkLayerStm.hpp
    UniqueDeviceId.cpp
    UniqueDeviceId.hpp
#    WatchDogStm.cpp
#    WatchDogStm.hpp
)
