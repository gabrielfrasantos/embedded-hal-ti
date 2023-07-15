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
    st.hal_driver
    hal_tiva.cortex
)

target_sources(hal_tiva.tiva PRIVATE
    Dummy.hpp
    Dummy.cpp
#    AnalogToDigitalPinStm.cpp
#    AnalogToDigitalPinStm.hpp
#    CanStm.cpp
#    CanStm.hpp
#    $<$<STREQUAL:${TARGET_MCU},tm4c123g>:DefaultClockEkTm4c123gxl.cpp>
#    $<$<STREQUAL:${TARGET_MCU},tm4c123g>:DefaultClockEkTm4c123gxl.hpp>
#    DigitalToAnalogPinStm.cpp
#    DigitalToAnalogPinStm.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:DmaStm.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:DmaStm.hpp>
#    FlashInternalStm.cpp
#    FlashInternalStm.hpp
#    GpioStm.cpp
#    GpioStm.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:I2cStm.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:I2cStm.hpp>
#    ResetStm.cpp
#    ResetStm.hpp
#    SpiDataSizeConfiguratorStm.cpp
#    SpiDataSizeConfiguratorStm.hpp
#    SpiMasterStm.cpp
#    SpiMasterStm.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiMasterStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiMasterStmDma.hpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiSlaveStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:SpiSlaveStmDma.hpp>
#    SystemTickStm.cpp
#    SystemTickStm.hpp
#    SystemTickTimerService.cpp
#    SystemTickTimerService.hpp
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStm.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStm.hpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStmDma.cpp>
#    $<$<NOT:$<STREQUAL:${TARGET_MCU_FAMILY},stm32g0xx>>:UartStmDma.hpp>
#    $<$<OR:$<STREQUAL:${TARGET_MCU},stm32wb55>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32f7xx>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32g4xx>>:UartStmDuplexDma.cpp>
#    $<$<OR:$<STREQUAL:${TARGET_MCU},stm32wb55>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32f7xx>,$<STREQUAL:${TARGET_MCU_FAMILY},stm32g4xx>>:UartStmDuplexDma.hpp>
#    UsbLinkLayerStm.cpp
#    UsbLinkLayerStm.hpp
#    UniqueDeviceId.cpp
#    UniqueDeviceId.hpp
#    WatchDogStm.cpp
#    WatchDogStm.hpp
)
