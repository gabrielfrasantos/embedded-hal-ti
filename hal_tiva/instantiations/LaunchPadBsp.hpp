#ifndef HAL_TI_LAUNCH_PAD_HPP
#define HAL_TI_LAUNCH_PAD_HPP

#if defined(TM4C123)
#include "hal_tiva/instantiations/LaunchPadBspEkTm4c123g.hpp"
#include "hal_tiva/tiva/ClockTm4c123.hpp"
#include "hal_tiva/tiva/PinoutTableDefaultTm4c123.hpp"
#elif defined(TM4C129)
#include "hal_tiva/instantiations/LaunchPadBspEkTm4c1294.hpp"
#include "hal_tiva/tiva/ClockTm4c129.hpp"
#include "hal_tiva/tiva/PinoutTableDefaultTm4c129.hpp"
#else
#error "MCU family not defined or invalid [TM4C123 | TM4C129]!"
#endif

namespace instantiations
{
#if defined(TM4C123)
    struct LaunchPad
    {
        LaunchPad()
        {
            hal::tiva::ConfigureClock(clock.crystal, clock.oscSource);
        }

        hal::tiva::GpioPin& DebugLed()
        {
            return ui.ledGreen;
        }

        LaunchPadClock clock;
        LaunchPadUi ui;
    };
#elif defined(TM4C129)
    struct LaunchPad
    {
        LaunchPad()
        {
            hal::tiva::ConfigureClock(clock.frequency, clock.hseValue, clock.oscSource, clock.systemClockVco, clock.usesPll);
        }

        hal::tiva::GpioPin& DebugLed()
        {
            return ui.led1;
        }

        LaunchPadClock clock;
        LaunchPadUi ui;
    };
#else
#error "MCU family not defined or invalid [TM4C123 | TM4C129]!"
#endif
}

#endif
