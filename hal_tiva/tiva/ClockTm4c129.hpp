#ifndef HAL_TI_CLOCK_TM4C129_HPP
#define HAL_TI_CLOCK_TM4C129_HPP

#include DEVICE_HEADER
#include <cstdint>

namespace hal::tiva
{
    enum class crystalFrequency : uint32_t
    {
        _5_MHz = 0x00000240,
        _6_MHz = 0x000002C0,
        _8_MHz = 0x00000380,
        _10_MHz = 0x00000400,
        _12_MHz = 0x00000440,
        _16_MHz = 0x00000540,
        _18_MHz = 0x000005C0,
        _20_MHz = 0x00000600,
        _24_MHz = 0x00000640,
        _25_MHz = 0x00000680,
    };

    enum class oscillatorSource : uint32_t
    {
        main,
        internal,
        lowFrequency,
        rtc,
    };

    enum class systemClockVco : uint32_t
    {
        _160_MHz = 0xF0000000,
        _240_MHz = 0xF1000000,
    };

    void ConfigureClock(uint32_t frequency, crystalFrequency hseValue, oscillatorSource oscSource = oscillatorSource::main, systemClockVco systemClockVco = systemClockVco::_240_MHz, bool usesPll = true);
}

#endif
