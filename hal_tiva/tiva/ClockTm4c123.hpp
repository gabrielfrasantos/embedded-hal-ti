#ifndef HAL_TI_CLOCK_TM4C123_HPP
#define HAL_TI_CLOCK_TM4C123_HPP

#include DEVICE_HEADER
#include <cstdint>

namespace hal::tiva
{
    enum class crystalFrequency : uint32_t
    {
        _5_MHz = 0x09,
        _6_MHz = 0x0B,
        _8_MHz = 0x0E,
        _10_MHz = 0x10,
        _12_MHz = 0x11,
        _16_MHz = 0x15,
        _18_MHz = 0x17,
        _20_MHz = 0x18,
        _24_MHz = 0x19,
        _25_MHz = 0x1A,
    };

    enum class oscillatorSource : uint32_t
    {
        main = 0,
        precisionInternal = 0x1,
        precisionInternalDivBy4 = 0x2,
        lowFrequencyInternal = 0x3,
    };

    struct systemClockDivider
    {
        uint8_t integer;
        uint8_t fractional;
    };

    void ConfigureClock(crystalFrequency hseValue, oscillatorSource oscSource = oscillatorSource::main, systemClockDivider systemClockDivisor = {2,5}, bool usesPll = true);
}

#endif
