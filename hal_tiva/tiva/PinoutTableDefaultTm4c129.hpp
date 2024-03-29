#ifndef HAL_PINOUT_TABLE_DEFAULT_TM4C129_HPP
#define HAL_PINOUT_TABLE_DEFAULT_TM4C129_HPP

#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/util/MemoryRange.hpp"

namespace hal::tiva
{
    extern const infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTableDefault;
    extern const infra::MemoryRange<const Gpio::AnalogPinPosition> analogTableDefault;

    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableUart;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableSpi;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableI2c;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableQei;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableCan;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTablePwm;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableUsb;
    extern const infra::MemoryRange<const Gpio::PinoutTable> pinoutTableEthernet;
}

#endif
