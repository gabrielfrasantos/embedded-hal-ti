#include "hal_tiva/tiva/PinoutTableDefault.hpp"

namespace hal
{
    constexpr std::array<const Gpio::PinPosition, 25> pinoutTableUartRxPins =
    {{
        { 1, Port::A, 10, GPIO_AF7_USART1 }, { 1, Port::B, 7, GPIO_AF7_USART1 }, { 1, Port::B, 15, GPIO_AF4_USART1 }, { 2, Port::A, 3, GPIO_AF7_USART2 },
        { 2, Port::D, 6, GPIO_AF7_USART2 }, { 3, Port::B, 11, GPIO_AF7_USART3 }, { 3, Port::C, 11, GPIO_AF7_USART3 }, { 3, Port::D, 9, GPIO_AF7_USART3 },
        { 4, Port::A, 1, GPIO_AF8_UART4 }, { 4, Port::A, 11, GPIO_AF6_UART4 }, { 4, Port::C, 11, GPIO_AF8_UART4 }, { 4, Port::D, 0, GPIO_AF8_UART4 },
        { 4, Port::H, 14, GPIO_AF8_UART4 }, { 4, Port::I, 9, GPIO_AF8_UART4 }, { 5, Port::B, 5, GPIO_AF1_UART5 }, { 5, Port::B, 8, GPIO_AF7_UART5 },
        { 5, Port::B, 12, GPIO_AF8_UART5 }, { 5, Port::D, 2, GPIO_AF8_UART5 }, { 6, Port::C, 7, GPIO_AF8_USART6 }, { 6, Port::G, 9, GPIO_AF8_USART6 },
        { 7, Port::A, 8, GPIO_AF12_UART7 }, { 7, Port::B, 3, GPIO_AF12_UART7 }, { 7, Port::E, 7, GPIO_AF8_UART7 }, { 7, Port::F, 6, GPIO_AF8_UART7 },
        { 8, Port::E, 0, GPIO_AF8_UART8 }
    }};

    constexpr std::array<const Gpio::PinPosition, 24> pinoutTableUartTxPins =
    {{
        { 1, Port::A, 9, GPIO_AF7_USART1 }, { 1, Port::B, 6, GPIO_AF7_USART1 }, { 1, Port::B, 14, GPIO_AF4_USART1 }, { 2, Port::A, 2, GPIO_AF7_USART2 },
        { 2, Port::D, 5, GPIO_AF7_USART2 }, { 3, Port::B, 10, GPIO_AF7_USART3 }, { 3, Port::C, 10, GPIO_AF7_USART3 }, { 3, Port::D, 8, GPIO_AF7_USART3 },
        { 4, Port::A, 0, GPIO_AF8_UART4 }, { 4, Port::A, 12, GPIO_AF6_UART4 }, { 4, Port::C, 10, GPIO_AF8_UART4 }, { 4, Port::D, 1, GPIO_AF8_UART4 },
        { 4, Port::H, 13, GPIO_AF8_UART4 }, { 5, Port::B, 6, GPIO_AF1_UART5 }, { 5, Port::B, 9, GPIO_AF7_UART5 }, { 5, Port::B, 13, GPIO_AF8_UART5 },
        { 5, Port::C, 12, GPIO_AF8_UART5 }, { 6, Port::C, 6, GPIO_AF8_USART6 }, { 6, Port::G, 14, GPIO_AF8_USART6 }, { 7, Port::A, 15, GPIO_AF12_UART7 },
        { 7, Port::B, 4, GPIO_AF12_UART7 }, { 7, Port::E, 8, GPIO_AF8_UART7 }, { 7, Port::F, 7, GPIO_AF8_UART7 }, { 8, Port::E, 1, GPIO_AF8_UART8 }
    }};

    constexpr std::array<const Gpio::PinPosition, 13> pinoutTableUartRtsPins =
    {{
        { 1, Port::A, 12, GPIO_AF7_USART1 }, { 2, Port::A, 1, GPIO_AF7_USART2 }, { 2, Port::D, 4, GPIO_AF7_USART2 }, { 3, Port::B, 14, GPIO_AF7_USART3 },
        { 3, Port::D, 12, GPIO_AF7_USART3 }, { 4, Port::A, 15, GPIO_AF8_UART4 }, { 4, Port::B, 14, GPIO_AF8_UART4 }, { 5, Port::C, 8, GPIO_AF7_UART5 },
        { 6, Port::G, 8, GPIO_AF8_USART6 }, { 6, Port::G, 12, GPIO_AF8_USART6 }, { 7, Port::E, 9, GPIO_AF8_UART7 }, { 7, Port::F, 8, GPIO_AF8_UART7 },
        { 8, Port::D, 15, GPIO_AF8_UART8 }
    }};

    constexpr std::array<const Gpio::PinPosition, 13> pinoutTableUartCtsPins =
    {{
        { 1, Port::A, 11, GPIO_AF7_USART1 }, { 2, Port::A, 0, GPIO_AF7_USART2 }, { 2, Port::D, 3, GPIO_AF7_USART2 }, { 3, Port::B, 13, GPIO_AF7_USART3 },
        { 3, Port::D, 11, GPIO_AF7_USART3 }, { 4, Port::B, 0, GPIO_AF8_UART4 }, { 4, Port::B, 15, GPIO_AF8_UART4 }, { 5, Port::C, 9, GPIO_AF7_UART5 },
        { 6, Port::G, 13, GPIO_AF8_USART6 }, { 6, Port::G, 15, GPIO_AF8_USART6 }, { 7, Port::E, 10, GPIO_AF8_UART7 }, { 7, Port::F, 9, GPIO_AF8_UART7 },
        { 8, Port::D, 14, GPIO_AF8_UART8 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableUartArray =
    {{
        {
            PinConfigTypeStm::uartRx,
            pinoutTableUartRxPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::uartTx,
            pinoutTableUartTxPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::uartRts,
            pinoutTableUartRtsPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::uartCts,
            pinoutTableUartCtsPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableUart = pinoutTableUartArray;

    constexpr std::array<const Gpio::PinoutTable, 0> pinoutTableLpuartArray =
    {{
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableLpuart = pinoutTableLpuartArray;

    constexpr std::array<const Gpio::PinPosition, 18> pinoutTableSpiClockPins =
    {{
        { 1, Port::A, 5, GPIO_AF5_SPI1 }, { 1, Port::B, 3, GPIO_AF5_SPI1 }, { 1, Port::G, 11, GPIO_AF5_SPI1 }, { 2, Port::A, 9, GPIO_AF5_SPI2 },
        { 2, Port::A, 12, GPIO_AF5_SPI2 }, { 2, Port::B, 10, GPIO_AF5_SPI2 }, { 2, Port::B, 13, GPIO_AF5_SPI2 }, { 2, Port::D, 3, GPIO_AF5_SPI2 },
        { 2, Port::I, 1, GPIO_AF5_SPI2 }, { 3, Port::B, 3, GPIO_AF6_SPI3 }, { 3, Port::C, 10, GPIO_AF6_SPI3 }, { 4, Port::E, 2, GPIO_AF5_SPI4 },
        { 4, Port::E, 12, GPIO_AF5_SPI4 }, { 5, Port::F, 7, GPIO_AF5_SPI5 }, { 5, Port::H, 6, GPIO_AF5_SPI5 }, { 6, Port::A, 5, GPIO_AF8_SPI6 },
        { 6, Port::B, 3, GPIO_AF8_SPI6 }, { 6, Port::G, 13, GPIO_AF5_SPI6 }
    }};

    constexpr std::array<const Gpio::PinPosition, 15> pinoutTableSpiMisoPins =
    {{
        { 1, Port::A, 6, GPIO_AF5_SPI1 }, { 1, Port::B, 4, GPIO_AF5_SPI1 }, { 1, Port::G, 9, GPIO_AF5_SPI1 }, { 2, Port::B, 14, GPIO_AF5_SPI2 },
        { 2, Port::C, 2, GPIO_AF5_SPI2 }, { 2, Port::I, 2, GPIO_AF5_SPI2 }, { 3, Port::B, 4, GPIO_AF6_SPI3 }, { 3, Port::C, 11, GPIO_AF6_SPI3 },
        { 4, Port::E, 5, GPIO_AF5_SPI4 }, { 4, Port::E, 13, GPIO_AF5_SPI4 }, { 5, Port::F, 8, GPIO_AF5_SPI5 }, { 5, Port::H, 7, GPIO_AF5_SPI5 },
        { 6, Port::A, 6, GPIO_AF8_SPI6 }, { 6, Port::B, 4, GPIO_AF8_SPI6 }, { 6, Port::G, 12, GPIO_AF5_SPI6 }
    }};

    constexpr std::array<const Gpio::PinPosition, 18> pinoutTableSpiMosiPins =
    {{
        { 1, Port::A, 7, GPIO_AF5_SPI1 }, { 1, Port::B, 5, GPIO_AF5_SPI1 }, { 1, Port::D, 7, GPIO_AF5_SPI1 }, { 2, Port::B, 15, GPIO_AF5_SPI2 },
        { 2, Port::C, 1, GPIO_AF5_SPI2 }, { 2, Port::C, 3, GPIO_AF5_SPI2 }, { 2, Port::I, 3, GPIO_AF5_SPI2 }, { 3, Port::B, 2, GPIO_AF7_SPI3 },
        { 3, Port::B, 5, GPIO_AF6_SPI3 }, { 3, Port::C, 12, GPIO_AF6_SPI3 }, { 3, Port::D, 6, GPIO_AF5_SPI3 }, { 4, Port::E, 6, GPIO_AF5_SPI4 },
        { 4, Port::E, 14, GPIO_AF5_SPI4 }, { 5, Port::F, 9, GPIO_AF5_SPI5 }, { 5, Port::F, 11, GPIO_AF5_SPI5 }, { 6, Port::A, 7, GPIO_AF8_SPI6 },
        { 6, Port::B, 5, GPIO_AF8_SPI6 }, { 6, Port::G, 14, GPIO_AF5_SPI6 }
    }};

    constexpr std::array<const Gpio::PinPosition, 17> pinoutTableSpiSlaveSelectPins =
    {{
        { 1, Port::A, 4, GPIO_AF5_SPI1 }, { 1, Port::A, 15, GPIO_AF5_SPI1 }, { 1, Port::G, 10, GPIO_AF5_SPI1 }, { 2, Port::A, 11, GPIO_AF5_SPI2 },
        { 2, Port::B, 4, GPIO_AF7_SPI2 }, { 2, Port::B, 9, GPIO_AF5_SPI2 }, { 2, Port::B, 12, GPIO_AF5_SPI2 }, { 2, Port::I, 0, GPIO_AF5_SPI2 },
        { 3, Port::A, 4, GPIO_AF6_SPI3 }, { 3, Port::A, 15, GPIO_AF6_SPI3 }, { 4, Port::E, 4, GPIO_AF5_SPI4 }, { 4, Port::E, 11, GPIO_AF5_SPI4 },
        { 5, Port::F, 6, GPIO_AF5_SPI5 }, { 5, Port::H, 5, GPIO_AF5_SPI5 }, { 6, Port::A, 4, GPIO_AF8_SPI6 }, { 6, Port::A, 15, GPIO_AF7_SPI6 },
        { 6, Port::G, 8, GPIO_AF5_SPI6 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableSpiArray =
    {{
        {
            PinConfigTypeStm::spiClock,
            pinoutTableSpiClockPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::spiMiso,
            pinoutTableSpiMisoPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::spiMosi,
            pinoutTableSpiMosiPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::spiSlaveSelect,
            pinoutTableSpiSlaveSelectPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableSpi = pinoutTableSpiArray;

    constexpr std::array<const Gpio::PinPosition, 7> pinoutTableSaiSckPins =
    {{
        { 1, Port::E, 5, GPIO_AF6_SAI1 }, { 1, Port::F, 8, GPIO_AF6_SAI1 }, { 2, Port::A, 2, GPIO_AF8_SAI2 }, { 2, Port::D, 13, GPIO_AF10_SAI2 },
        { 2, Port::E, 12, GPIO_AF10_SAI2 }, { 2, Port::H, 2, GPIO_AF10_SAI2 }, { 2, Port::I, 5, GPIO_AF10_SAI2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 9> pinoutTableSaiMClockPins =
    {{
        { 1, Port::E, 2, GPIO_AF6_SAI1 }, { 1, Port::F, 7, GPIO_AF6_SAI1 }, { 1, Port::G, 7, GPIO_AF6_SAI1 }, { 2, Port::A, 1, GPIO_AF10_SAI2 },
        { 2, Port::E, 0, GPIO_AF10_SAI2 }, { 2, Port::E, 6, GPIO_AF10_SAI2 }, { 2, Port::E, 14, GPIO_AF10_SAI2 }, { 2, Port::H, 3, GPIO_AF10_SAI2 },
        { 2, Port::I, 4, GPIO_AF10_SAI2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableSaiSdPins =
    {{
        { 1, Port::B, 2, GPIO_AF6_SAI1 }, { 1, Port::C, 1, GPIO_AF6_SAI1 }, { 1, Port::D, 6, GPIO_AF6_SAI1 }, { 1, Port::E, 3, GPIO_AF6_SAI1 },
        { 1, Port::E, 6, GPIO_AF6_SAI1 }, { 1, Port::F, 6, GPIO_AF6_SAI1 }, { 2, Port::A, 0, GPIO_AF10_SAI2 }, { 2, Port::D, 11, GPIO_AF10_SAI2 },
        { 2, Port::E, 11, GPIO_AF10_SAI2 }, { 2, Port::F, 11, GPIO_AF10_SAI2 }, { 2, Port::G, 10, GPIO_AF10_SAI2 }, { 2, Port::I, 6, GPIO_AF10_SAI2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 8> pinoutTableSaiFsPins =
    {{
        { 1, Port::E, 4, GPIO_AF6_SAI1 }, { 1, Port::F, 9, GPIO_AF6_SAI1 }, { 2, Port::A, 12, GPIO_AF8_SAI2 }, { 2, Port::C, 0, GPIO_AF8_SAI2 },
        { 2, Port::D, 12, GPIO_AF10_SAI2 }, { 2, Port::E, 13, GPIO_AF10_SAI2 }, { 2, Port::G, 9, GPIO_AF10_SAI2 }, { 2, Port::I, 7, GPIO_AF10_SAI2 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableSaiArray =
    {{
        {
            PinConfigTypeStm::saiSck,
            pinoutTableSaiSckPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::saiMClock,
            pinoutTableSaiMClockPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::saiSd,
            pinoutTableSaiSdPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::saiFs,
            pinoutTableSaiFsPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableSai = pinoutTableSaiArray;

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableSdClkPins =
    {{
        { 1, Port::C, 12, GPIO_AF12_SDMMC1 }, { 2, Port::D, 6, GPIO_AF11_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableSdCmdPins =
    {{
        { 1, Port::D, 2, GPIO_AF12_SDMMC1 }, { 2, Port::D, 7, GPIO_AF11_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableSdD0Pins =
    {{
        { 1, Port::C, 8, GPIO_AF12_SDMMC1 }, { 2, Port::B, 14, GPIO_AF10_SDMMC2 }, { 2, Port::G, 9, GPIO_AF11_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableSdD1Pins =
    {{
        { 1, Port::C, 9, GPIO_AF12_SDMMC1 }, { 2, Port::B, 15, GPIO_AF10_SDMMC2 }, { 2, Port::G, 10, GPIO_AF11_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableSdD2Pins =
    {{
        { 1, Port::C, 10, GPIO_AF12_SDMMC1 }, { 2, Port::B, 3, GPIO_AF10_SDMMC2 }, { 2, Port::G, 11, GPIO_AF10_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableSdD3Pins =
    {{
        { 1, Port::C, 11, GPIO_AF12_SDMMC1 }, { 2, Port::B, 4, GPIO_AF10_SDMMC2 }, { 2, Port::G, 12, GPIO_AF11_SDMMC2 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 6> pinoutTableSdArray =
    {{
        {
            PinConfigTypeStm::sdClk,
            pinoutTableSdClkPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::sdCmd,
            pinoutTableSdCmdPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::sdD0,
            pinoutTableSdD0Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::sdD1,
            pinoutTableSdD1Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::sdD2,
            pinoutTableSdD2Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::sdD3,
            pinoutTableSdD3Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableSd = pinoutTableSdArray;

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableQuadSpiClockPins =
    {{
        { 0, Port::B, 2, GPIO_AF9_QUADSPI }, { 0, Port::F, 10, GPIO_AF9_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableQuadSpiSlaveSelectPins =
    {{
        { 0, Port::B, 6, GPIO_AF10_QUADSPI }, { 0, Port::B, 10, GPIO_AF9_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableQuadSpiData0Pins =
    {{
        { 0, Port::C, 9, GPIO_AF9_QUADSPI }, { 0, Port::D, 11, GPIO_AF9_QUADSPI }, { 0, Port::F, 8, GPIO_AF10_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableQuadSpiData1Pins =
    {{
        { 0, Port::C, 10, GPIO_AF9_QUADSPI }, { 0, Port::D, 12, GPIO_AF9_QUADSPI }, { 0, Port::F, 9, GPIO_AF10_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableQuadSpiData2Pins =
    {{
        { 0, Port::E, 2, GPIO_AF9_QUADSPI }, { 0, Port::F, 7, GPIO_AF9_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinPosition, 3> pinoutTableQuadSpiData3Pins =
    {{
        { 0, Port::A, 1, GPIO_AF9_QUADSPI }, { 0, Port::D, 13, GPIO_AF9_QUADSPI }, { 0, Port::F, 6, GPIO_AF9_QUADSPI }
    }};

    constexpr std::array<const Gpio::PinoutTable, 6> pinoutTableQuadSpiArray =
    {{
        {
            PinConfigTypeStm::quadSpiClock,
            pinoutTableQuadSpiClockPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::quadSpiSlaveSelect,
            pinoutTableQuadSpiSlaveSelectPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::quadSpiData0,
            pinoutTableQuadSpiData0Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::quadSpiData1,
            pinoutTableQuadSpiData1Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::quadSpiData2,
            pinoutTableQuadSpiData2Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::quadSpiData3,
            pinoutTableQuadSpiData3Pins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableQuadSpi = pinoutTableQuadSpiArray;

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableI2cSclPins =
    {{
        { 1, Port::B, 6, GPIO_AF4_I2C1 }, { 1, Port::B, 8, GPIO_AF4_I2C1 }, { 2, Port::B, 10, GPIO_AF4_I2C2 }, { 2, Port::F, 1, GPIO_AF4_I2C2 },
        { 2, Port::H, 4, GPIO_AF4_I2C2 }, { 3, Port::A, 8, GPIO_AF4_I2C3 }, { 3, Port::H, 7, GPIO_AF4_I2C3 }, { 4, Port::B, 6, GPIO_AF11_I2C4 },
        { 4, Port::B, 8, GPIO_AF1_I2C4 }, { 4, Port::D, 12, GPIO_AF4_I2C4 }, { 4, Port::F, 14, GPIO_AF4_I2C4 }, { 4, Port::H, 11, GPIO_AF4_I2C4 }
    }};

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableI2cSdaPins =
    {{
        { 1, Port::B, 7, GPIO_AF4_I2C1 }, { 1, Port::B, 9, GPIO_AF4_I2C1 }, { 2, Port::B, 11, GPIO_AF4_I2C2 }, { 2, Port::F, 0, GPIO_AF4_I2C2 },
        { 2, Port::H, 5, GPIO_AF4_I2C2 }, { 3, Port::C, 9, GPIO_AF4_I2C3 }, { 3, Port::H, 8, GPIO_AF4_I2C3 }, { 4, Port::B, 7, GPIO_AF11_I2C4 },
        { 4, Port::B, 9, GPIO_AF1_I2C4 }, { 4, Port::D, 13, GPIO_AF4_I2C4 }, { 4, Port::F, 15, GPIO_AF4_I2C4 }, { 4, Port::H, 12, GPIO_AF4_I2C4 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 2> pinoutTableI2cArray =
    {{
        {
            PinConfigTypeStm::i2cScl,
            pinoutTableI2cSclPins,
            Drive::OpenDrain,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::i2cSda,
            pinoutTableI2cSdaPins,
            Drive::OpenDrain,
            Speed::Default,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableI2c = pinoutTableI2cArray;

    constexpr std::array<const Gpio::PinPosition, 31> pinoutTableEthernetPins =
    {{
        { 0, Port::A, 0, GPIO_AF11_ETH }, { 0, Port::A, 1, GPIO_AF11_ETH }, { 0, Port::A, 1, GPIO_AF11_ETH }, { 0, Port::A, 2, GPIO_AF11_ETH },
        { 0, Port::A, 3, GPIO_AF11_ETH }, { 0, Port::A, 7, GPIO_AF11_ETH }, { 0, Port::A, 7, GPIO_AF11_ETH }, { 0, Port::A, 7, GPIO_AF11_ETH },
        { 0, Port::B, 0, GPIO_AF11_ETH }, { 0, Port::B, 1, GPIO_AF11_ETH }, { 0, Port::B, 5, GPIO_AF11_ETH }, { 0, Port::B, 8, GPIO_AF11_ETH },
        { 0, Port::B, 10, GPIO_AF11_ETH }, { 0, Port::B, 11, GPIO_AF11_ETH }, { 0, Port::B, 12, GPIO_AF11_ETH }, { 0, Port::B, 13, GPIO_AF11_ETH },
        { 0, Port::C, 1, GPIO_AF11_ETH }, { 0, Port::C, 2, GPIO_AF11_ETH }, { 0, Port::C, 3, GPIO_AF11_ETH }, { 0, Port::C, 4, GPIO_AF11_ETH },
        { 0, Port::C, 5, GPIO_AF11_ETH }, { 0, Port::E, 2, GPIO_AF11_ETH }, { 0, Port::G, 8, GPIO_AF11_ETH }, { 0, Port::G, 11, GPIO_AF11_ETH },
        { 0, Port::G, 13, GPIO_AF11_ETH }, { 0, Port::G, 14, GPIO_AF11_ETH }, { 0, Port::H, 2, GPIO_AF11_ETH }, { 0, Port::H, 3, GPIO_AF11_ETH },
        { 0, Port::H, 6, GPIO_AF11_ETH }, { 0, Port::H, 7, GPIO_AF11_ETH }, { 0, Port::I, 10, GPIO_AF11_ETH }
    }};

    constexpr std::array<const Gpio::PinoutTable, 1> pinoutTableEthernetArray =
    {{
        {
            PinConfigTypeStm::ethernet,
            pinoutTableEthernetPins,
            Drive::Default,
            Speed::High,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableEthernet = pinoutTableEthernetArray;

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableUsbFsIdPins =
    {{
        { 0, Port::A, 10, GPIO_AF10_OTG_FS }
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableUsbFsDmPins =
    {{
        { 0, Port::A, 11, GPIO_AF10_OTG_FS }
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableUsbFsDpPins =
    {{
        { 0, Port::A, 12, GPIO_AF10_OTG_FS }
    }};

    constexpr std::array<const Gpio::PinoutTable, 3> pinoutTableUsbFsArray =
    {{
        {
            PinConfigTypeStm::usbFsId,
            pinoutTableUsbFsIdPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::usbFsDm,
            pinoutTableUsbFsDmPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::usbFsDp,
            pinoutTableUsbFsDpPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableUsbFs = pinoutTableUsbFsArray;

    constexpr std::array<const Gpio::PinPosition, 9> pinoutTableCanRxPins =
    {{
        { 1, Port::A, 11, GPIO_AF9_CAN1 }, { 1, Port::B, 8, GPIO_AF9_CAN1 }, { 1, Port::D, 0, GPIO_AF9_CAN1 }, { 1, Port::H, 14, GPIO_AF9_CAN1 },
        { 1, Port::I, 9, GPIO_AF9_CAN1 }, { 2, Port::B, 5, GPIO_AF9_CAN2 }, { 2, Port::B, 12, GPIO_AF9_CAN2 }, { 3, Port::A, 8, GPIO_AF11_CAN3 },
        { 3, Port::B, 3, GPIO_AF11_CAN3 }
    }};

    constexpr std::array<const Gpio::PinPosition, 8> pinoutTableCanTxPins =
    {{
        { 1, Port::A, 12, GPIO_AF9_CAN1 }, { 1, Port::B, 9, GPIO_AF9_CAN1 }, { 1, Port::D, 1, GPIO_AF9_CAN1 }, { 1, Port::H, 13, GPIO_AF9_CAN1 },
        { 2, Port::B, 6, GPIO_AF9_CAN2 }, { 2, Port::B, 13, GPIO_AF9_CAN2 }, { 3, Port::A, 15, GPIO_AF11_CAN3 }, { 3, Port::B, 4, GPIO_AF11_CAN3 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 2> pinoutTableCanArray =
    {{
        {
            PinConfigTypeStm::canRx,
            pinoutTableCanRxPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::canTx,
            pinoutTableCanTxPins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableCan = pinoutTableCanArray;

    constexpr std::array<const Gpio::PinPosition, 26> pinoutTableTimerChannel1Pins =
    {{
        { 1, Port::A, 8, GPIO_AF1_TIM1 }, { 1, Port::E, 9, GPIO_AF1_TIM1 }, { 2, Port::A, 0, GPIO_AF1_TIM2 }, { 2, Port::A, 5, GPIO_AF1_TIM2 },
        { 2, Port::A, 15, GPIO_AF1_TIM2 }, { 3, Port::A, 6, GPIO_AF2_TIM3 }, { 3, Port::B, 4, GPIO_AF2_TIM3 }, { 3, Port::C, 6, GPIO_AF2_TIM3 },
        { 4, Port::B, 6, GPIO_AF2_TIM4 }, { 4, Port::D, 12, GPIO_AF2_TIM4 }, { 5, Port::A, 0, GPIO_AF2_TIM5 }, { 5, Port::H, 10, GPIO_AF2_TIM5 },
        { 8, Port::C, 6, GPIO_AF3_TIM8 }, { 8, Port::I, 5, GPIO_AF3_TIM8 }, { 9, Port::A, 2, GPIO_AF3_TIM9 }, { 9, Port::E, 5, GPIO_AF3_TIM9 },
        { 10, Port::B, 8, GPIO_AF3_TIM10 }, { 10, Port::F, 6, GPIO_AF3_TIM10 }, { 11, Port::B, 9, GPIO_AF3_TIM11 }, { 11, Port::F, 7, GPIO_AF3_TIM11 },
        { 12, Port::B, 14, GPIO_AF9_TIM12 }, { 12, Port::H, 6, GPIO_AF9_TIM12 }, { 13, Port::A, 6, GPIO_AF9_TIM13 }, { 13, Port::F, 8, GPIO_AF9_TIM13 },
        { 14, Port::A, 7, GPIO_AF9_TIM14 }, { 14, Port::F, 9, GPIO_AF9_TIM14 }
    }};

    constexpr std::array<const Gpio::PinPosition, 17> pinoutTableTimerChannel2Pins =
    {{
        { 1, Port::A, 9, GPIO_AF1_TIM1 }, { 1, Port::E, 11, GPIO_AF1_TIM1 }, { 2, Port::A, 1, GPIO_AF1_TIM2 }, { 2, Port::B, 3, GPIO_AF1_TIM2 },
        { 3, Port::A, 7, GPIO_AF2_TIM3 }, { 3, Port::B, 5, GPIO_AF2_TIM3 }, { 3, Port::C, 7, GPIO_AF2_TIM3 }, { 4, Port::B, 7, GPIO_AF2_TIM4 },
        { 4, Port::D, 13, GPIO_AF2_TIM4 }, { 5, Port::A, 1, GPIO_AF2_TIM5 }, { 5, Port::H, 11, GPIO_AF2_TIM5 }, { 8, Port::C, 7, GPIO_AF3_TIM8 },
        { 8, Port::I, 6, GPIO_AF3_TIM8 }, { 9, Port::A, 3, GPIO_AF3_TIM9 }, { 9, Port::E, 6, GPIO_AF3_TIM9 }, { 12, Port::B, 15, GPIO_AF9_TIM12 },
        { 12, Port::H, 9, GPIO_AF9_TIM12 }
    }};

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableTimerChannel3Pins =
    {{
        { 1, Port::A, 10, GPIO_AF1_TIM1 }, { 1, Port::E, 13, GPIO_AF1_TIM1 }, { 2, Port::A, 2, GPIO_AF1_TIM2 }, { 2, Port::B, 10, GPIO_AF1_TIM2 },
        { 3, Port::B, 0, GPIO_AF2_TIM3 }, { 3, Port::C, 8, GPIO_AF2_TIM3 }, { 4, Port::B, 8, GPIO_AF2_TIM4 }, { 4, Port::D, 14, GPIO_AF2_TIM4 },
        { 5, Port::A, 2, GPIO_AF2_TIM5 }, { 5, Port::H, 12, GPIO_AF2_TIM5 }, { 8, Port::C, 8, GPIO_AF3_TIM8 }, { 8, Port::I, 7, GPIO_AF3_TIM8 }
    }};

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableTimerChannel4Pins =
    {{
        { 1, Port::A, 11, GPIO_AF1_TIM1 }, { 1, Port::E, 14, GPIO_AF1_TIM1 }, { 2, Port::A, 3, GPIO_AF1_TIM2 }, { 2, Port::B, 11, GPIO_AF1_TIM2 },
        { 3, Port::B, 1, GPIO_AF2_TIM3 }, { 3, Port::C, 9, GPIO_AF2_TIM3 }, { 4, Port::B, 9, GPIO_AF2_TIM4 }, { 4, Port::D, 15, GPIO_AF2_TIM4 },
        { 5, Port::A, 3, GPIO_AF2_TIM5 }, { 5, Port::I, 0, GPIO_AF2_TIM5 }, { 8, Port::C, 9, GPIO_AF3_TIM8 }, { 8, Port::I, 2, GPIO_AF3_TIM8 }
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableTimerArray =
    {{
        {
            PinConfigTypeStm::timerChannel1,
            pinoutTableTimerChannel1Pins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::timerChannel2,
            pinoutTableTimerChannel2Pins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::timerChannel3,
            pinoutTableTimerChannel3Pins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        },
        {
            PinConfigTypeStm::timerChannel4,
            pinoutTableTimerChannel4Pins,
            Drive::Default,
            Speed::Default,
            WeakPull::Default
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableTimer = pinoutTableTimerArray;

    constexpr std::array<const infra::MemoryRange<const Gpio::PinoutTable>, 13> pinoutTableDefaultStmArray = { pinoutTableUart, pinoutTableLpuart, pinoutTableSpi, pinoutTableSai, pinoutTableSd, pinoutTableQuadSpi, pinoutTableI2c, pinoutTableEthernet, pinoutTableUsbFs, pinoutTableCan, pinoutTableTimer, pinoutTableFmc, pinoutTableLcd };
    constexpr infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTableDefaultStm = pinoutTableDefaultStmArray;
    constexpr std::array<const Gpio::AnalogPinPosition, 50> analogTableDefaultStmArray =
    {{
        { 1, Type::adc, Port::A, 0, 0 },
        { 2, Type::adc, Port::A, 0, 0 },
        { 3, Type::adc, Port::A, 0, 0 },
        { 1, Type::adc, Port::A, 1, 1 },
        { 2, Type::adc, Port::A, 1, 1 },
        { 3, Type::adc, Port::A, 1, 1 },
        { 1, Type::adc, Port::A, 2, 2 },
        { 2, Type::adc, Port::A, 2, 2 },
        { 3, Type::adc, Port::A, 2, 2 },
        { 1, Type::adc, Port::A, 3, 3 },
        { 2, Type::adc, Port::A, 3, 3 },
        { 3, Type::adc, Port::A, 3, 3 },
        { 1, Type::adc, Port::A, 4, 4 },
        { 2, Type::adc, Port::A, 4, 4 },
        { 1, Type::dac, Port::A, 4, 1 },
        { 1, Type::adc, Port::A, 5, 5 },
        { 2, Type::adc, Port::A, 5, 5 },
        { 1, Type::dac, Port::A, 5, 2 },
        { 1, Type::adc, Port::A, 6, 6 },
        { 2, Type::adc, Port::A, 6, 6 },
        { 1, Type::adc, Port::A, 7, 7 },
        { 2, Type::adc, Port::A, 7, 7 },
        { 1, Type::adc, Port::B, 0, 8 },
        { 2, Type::adc, Port::B, 0, 8 },
        { 1, Type::adc, Port::B, 1, 9 },
        { 2, Type::adc, Port::B, 1, 9 },
        { 1, Type::adc, Port::C, 0, 10 },
        { 2, Type::adc, Port::C, 0, 10 },
        { 3, Type::adc, Port::C, 0, 10 },
        { 1, Type::adc, Port::C, 1, 11 },
        { 2, Type::adc, Port::C, 1, 11 },
        { 3, Type::adc, Port::C, 1, 11 },
        { 1, Type::adc, Port::C, 2, 12 },
        { 2, Type::adc, Port::C, 2, 12 },
        { 3, Type::adc, Port::C, 2, 12 },
        { 1, Type::adc, Port::C, 3, 13 },
        { 2, Type::adc, Port::C, 3, 13 },
        { 3, Type::adc, Port::C, 3, 13 },
        { 1, Type::adc, Port::C, 4, 14 },
        { 2, Type::adc, Port::C, 4, 14 },
        { 1, Type::adc, Port::C, 5, 15 },
        { 2, Type::adc, Port::C, 5, 15 },
        { 3, Type::adc, Port::F, 3, 9 },
        { 3, Type::adc, Port::F, 4, 14 },
        { 3, Type::adc, Port::F, 5, 15 },
        { 3, Type::adc, Port::F, 6, 4 },
        { 3, Type::adc, Port::F, 7, 5 },
        { 3, Type::adc, Port::F, 8, 6 },
        { 3, Type::adc, Port::F, 9, 7 },
        { 3, Type::adc, Port::F, 10, 8 },
    }};
    constexpr infra::MemoryRange<const Gpio::AnalogPinPosition> analogTableDefaultStm = analogTableDefaultStmArray;
}
