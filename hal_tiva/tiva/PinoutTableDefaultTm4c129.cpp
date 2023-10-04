#include "hal_tiva/tiva/PinoutTableDefaultTm4c129.hpp"

namespace hal::tiva
{
    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableUartRxPins =
    {{
        { 0, Port::A, 0, 0x01 },
        { 1, Port::Q, 4, 0x01 },
        { 1, Port::B, 0, 0x01 },
        { 2, Port::A, 6, 0x01 },
        { 2, Port::D, 4, 0x01 },
        { 3, Port::A, 4, 0x01 },
        { 3, Port::J, 0, 0x01 },
        { 4, Port::A, 2, 0x01 },
        { 4, Port::K, 0, 0x01 },
        { 5, Port::C, 6, 0x01 },
        { 6, Port::P, 0, 0x01 },
        { 7, Port::C, 4, 0x01 },
    }};

    constexpr std::array<const Gpio::PinPosition, 11> pinoutTableUartTxPins =
    {{
        { 0, Port::A, 1, 0x01 },
        { 1, Port::B, 1, 0x01 },
        { 2, Port::A, 7, 0x01 },
        { 2, Port::D, 5, 0x01 },
        { 3, Port::A, 5, 0x01 },
        { 3, Port::J, 1, 0x01 },
        { 4, Port::A, 3, 0x01 },
        { 4, Port::K, 1, 0x01 },
        { 5, Port::C, 7, 0x01 },
        { 6, Port::P, 1, 0x01 },
        { 7, Port::C, 5, 0x01 },
    }};

    constexpr std::array<const Gpio::PinPosition, 8> pinoutTableUartRtsPins =
    {{
        { 0, Port::B, 5, 0x01 },
        { 0, Port::H, 0, 0x01 },
        { 1, Port::E, 0, 0x01 },
        { 1, Port::N, 0, 0x01 },
        { 2, Port::D, 6, 0x01 },
        { 2, Port::N, 2, 0x02 },
        { 3, Port::P, 4, 0x01 },
        { 3, Port::N, 4, 0x02 },
    }};

    constexpr std::array<const Gpio::PinPosition, 9> pinoutTableUartCtsPins =
    {{
        { 0, Port::B, 4, 0x01 },
        { 0, Port::H, 1, 0x01 },
        { 0, Port::M, 4, 0x01 },
        { 1, Port::N, 1, 0x01 },
        { 1, Port::P, 3, 0x01 },
        { 2, Port::D, 7, 0x01 },
        { 2, Port::N, 3, 0x02 },
        { 3, Port::P, 5, 0x01 },
        { 3, Port::N, 5, 0x02 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableUartArray =
    {{
        {
            PinConfigPeripheral::uartRx,
            pinoutTableUartRxPins,
            Drive::Up,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::uartTx,
            pinoutTableUartTxPins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::uartRts,
            pinoutTableUartRtsPins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::uartCts,
            pinoutTableUartCtsPins,
            Drive::Up,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        }
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableUart = pinoutTableUartArray;

    constexpr std::array<const Gpio::PinPosition, 5> pinoutTableSpiClkPins =
    {{
        { 0, Port::A, 2, 0x0f },
        { 1, Port::B, 5, 0x0f },
        { 2, Port::D, 3, 0x0f },
        { 3, Port::Q, 0, 0x0e },
        { 3, Port::F, 3, 0x0e },
    }};

    constexpr std::array<const Gpio::PinPosition, 5> pinoutTableSpiFssPins =
    {{
        { 0, Port::A, 3, 0x0f },
        { 1, Port::B, 4, 0x0f },
        { 2, Port::D, 2, 0x0f },
        { 3, Port::Q, 1, 0x0e },
        { 3, Port::F, 2, 0x0e },
    }};

    constexpr std::array<const Gpio::PinPosition, 5> pinoutTableSpiRxPins =
    {{
        { 0, Port::A, 4, 0x0f },
        { 1, Port::E, 4, 0x0f },
        { 2, Port::D, 1, 0x0f },
        { 3, Port::Q, 2, 0x0e },
        { 3, Port::F, 1, 0x0e },
    }};

    constexpr std::array<const Gpio::PinPosition, 5> pinoutTableSpiTxPins =
    {{
        { 0, Port::A, 5, 0x0f },
        { 1, Port::E, 5, 0x0f },
        { 2, Port::D, 0, 0x0f },
        { 3, Port::Q, 3, 0x0e },
        { 3, Port::F, 0, 0x0e },
    }};

    constexpr std::array<const Gpio::PinoutTable, 4> pinoutTableSpiArray =
    {{
        {
            PinConfigPeripheral::spiClock,
            pinoutTableSpiClkPins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::spiSlaveSelect,
            pinoutTableSpiFssPins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::spiMiso,
            pinoutTableSpiRxPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::spiMosi,
            pinoutTableSpiTxPins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableSpi = pinoutTableSpiArray;

    constexpr std::array<const Gpio::PinPosition, 12> pinoutTableI2cSclPins =
    {{
        { 0, Port::B, 2, 0x02 },
        { 1, Port::G, 0, 0x02 },
        { 2, Port::L, 0, 0x02 },
        { 2, Port::P, 5, 0x02 },
        { 2, Port::N, 5, 0x03 },
        { 3, Port::K, 5, 0x02 },
        { 4, Port::K, 6, 0x02 },
        { 5, Port::B, 0, 0x02 },
        { 5, Port::B, 4, 0x02 },
        { 6, Port::A, 6, 0x02 },
        { 7, Port::D, 0, 0x02 },
        { 7, Port::A, 4, 0x02 },
    }};

    constexpr std::array<const Gpio::PinPosition, 11> pinoutTableI2cSdaPins =
    {{
        { 0, Port::B, 3, 0x02 },
        { 1, Port::G, 1, 0x02 },
        { 2, Port::L, 0, 0x02 },
        { 2, Port::N, 4, 0x03 },
        { 3, Port::K, 5, 0x02 },
        { 4, Port::K, 7, 0x02 },
        { 5, Port::B, 1, 0x02 },
        { 5, Port::B, 5, 0x02 },
        { 6, Port::A, 7, 0x02 },
        { 7, Port::D, 1, 0x02 },
        { 7, Port::A, 5, 0x02 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 2> pinoutTableI2cArray =
    {{
        {
            PinConfigPeripheral::i2cScl,
            pinoutTableI2cSclPins,
            Drive::None,
            hal::PinConfigType::triState,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::i2cSda,
            pinoutTableI2cSdaPins,
            Drive::OpenDrain,
            hal::PinConfigType::triState,
            Current::_2mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableI2c = pinoutTableI2cArray;

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableQeiIndexPins =
    {{
        { 0, Port::L, 3, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableQeiPhaseAPins =
    {{
        { 0, Port::L, 1, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTableQeiPhaseBPins =
    {{
        { 0, Port::L, 2, 0x06 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 3> pinoutTableQeiArray =
    {{
        {
            PinConfigPeripheral::qeiIndex,
            pinoutTableQeiIndexPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::qeiPhaseA,
            pinoutTableQeiPhaseAPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::qeiPhaseB,
            pinoutTableQeiPhaseBPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableQei = pinoutTableQeiArray;

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableCanRxPins =
    {{
        { 0, Port::A, 0, 0x07 },
        { 1, Port::B, 0, 0x07 },
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableCanTxPins =
    {{
        { 0, Port::A, 1, 0x07 },
        { 1, Port::B, 1, 0x07 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 2> pinoutTableCanArray =
    {{
        {
            PinConfigPeripheral::canRx,
            pinoutTableCanRxPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::canTx,
            pinoutTableCanTxPins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableCan = pinoutTableCanArray;

    constexpr std::array<const Gpio::PinPosition, 4> pinoutTablePwmFaultPins =
    {{
        { 0, Port::F, 0, 0x06 },
        { 0, Port::K, 6, 0x06 },
        { 0, Port::K, 7, 0x06 },
        { 0, Port::L, 0, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel0Pins =
    {{
        { 0, Port::F, 0, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel1Pins =
    {{
        { 0, Port::F, 1, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel2Pins =
    {{
        { 0, Port::F, 2, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel3Pins =
    {{
        { 0, Port::F, 3, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel4Pins =
    {{
        { 0, Port::G, 0, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel5Pins =
    {{
        { 0, Port::G, 1, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel6Pins =
    {{
        { 0, Port::K, 4, 0x06 },
    }};

    constexpr std::array<const Gpio::PinPosition, 1> pinoutTablePwmChannel7Pins =
    {{
        { 0, Port::K, 5, 0x06 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 9> pinoutTablePwmArray =
    {{
        {
            PinConfigPeripheral::pwmFault,
            pinoutTablePwmFaultPins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel0,
            pinoutTablePwmChannel0Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel1,
            pinoutTablePwmChannel1Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel2,
            pinoutTablePwmChannel2Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel3,
            pinoutTablePwmChannel3Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel4,
            pinoutTablePwmChannel4Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel5,
            pinoutTablePwmChannel5Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel6,
            pinoutTablePwmChannel6Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
        {
            PinConfigPeripheral::pwmChannel7,
            pinoutTablePwmChannel7Pins,
            Drive::Up,
            hal::PinConfigType::output,
            Current::_2mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTablePwm = pinoutTablePwmArray;

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableEthernetLed0Pins =
    {{
        { 0, Port::F, 0, 0x05 },
        { 0, Port::K, 4, 0x05 },
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableEthernetLed1Pins =
    {{
        { 0, Port::F, 4, 0x05 },
        { 0, Port::K, 6, 0x05 },
    }};

    constexpr std::array<const Gpio::PinPosition, 2> pinoutTableEthernetLed2Pins =
    {{
        { 0, Port::F, 1, 0x05 },
        { 0, Port::K, 5, 0x05 },
    }};

    constexpr std::array<const Gpio::PinoutTable, 3> pinoutTableEthernetArray =
    {{
        {
            PinConfigPeripheral::ethernetLed0,
            pinoutTableEthernetLed0Pins,
            Drive::None,
            hal::PinConfigType::input,
            Current::_8mA,
            true,
        },
        {
            PinConfigPeripheral::ethernetLed1,
            pinoutTableEthernetLed1Pins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_8mA,
            true,
        },
        {
            PinConfigPeripheral::ethernetLed2,
            pinoutTableEthernetLed2Pins,
            Drive::None,
            hal::PinConfigType::output,
            Current::_8mA,
            true,
        },
    }};

    constexpr infra::MemoryRange<const Gpio::PinoutTable> pinoutTableEthernet = pinoutTableEthernetArray;

    constexpr std::array<const infra::MemoryRange<const Gpio::PinoutTable>, 6> pinoutTableDefaultArray = { pinoutTableUart, pinoutTableSpi, pinoutTableI2c, pinoutTableCan, pinoutTablePwm, pinoutTableQei };
    constexpr infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTableDefault = pinoutTableDefaultArray;
    constexpr std::array<const Gpio::AnalogPinPosition, 20> analogTableDefaultArray =
    {{
        { Type::adc, Port::E, 3,  0 },
        { Type::adc, Port::E, 2,  1 },
        { Type::adc, Port::E, 1,  2 },
        { Type::adc, Port::E, 0,  3 },
        { Type::adc, Port::D, 7,  4 },
        { Type::adc, Port::D, 6,  5 },
        { Type::adc, Port::D, 5,  6 },
        { Type::adc, Port::D, 4,  7 },
        { Type::adc, Port::E, 5,  8 },
        { Type::adc, Port::E, 4,  9 },
        { Type::adc, Port::B, 4, 10 },
        { Type::adc, Port::B, 5, 11 },
        { Type::adc, Port::D, 3, 12 },
        { Type::adc, Port::D, 2, 13 },
        { Type::adc, Port::D, 1, 14 },
        { Type::adc, Port::D, 0, 15 },
        { Type::adc, Port::K, 0, 16 },
        { Type::adc, Port::K, 1, 17 },
        { Type::adc, Port::K, 2, 18 },
        { Type::adc, Port::K, 3, 19 },
    }};
    constexpr infra::MemoryRange<const Gpio::AnalogPinPosition> analogTableDefault = analogTableDefaultArray;
}
