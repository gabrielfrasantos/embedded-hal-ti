#ifndef HAL_TI_LAUNCH_PAD_EK_TM4C123G_HPP
#define HAL_TI_LAUNCH_PAD_EK_TM4C123G_HPP

#include "hal_tiva/tiva/ClockTm4c123.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"
#include "services/util/Terminal.hpp"

namespace instantiations
{
    struct LaunchPadClock
    {
        hal::tiva::crystalFrequency crystal = hal::tiva::crystalFrequency::_16_MHz;
        hal::tiva::oscillatorSource oscSource = oscillatorSource::main;
        hal::tiva::systemClockDivider systemClockDivisor{ 2,5 };
        bool usesPll = true;
    };

    struct LaunchPadUi
    {
        hal::tiva::GpioPin ledRed{ hal::tiva::Port::F, 1, hal::tiva::Drive::Up };
        hal::tiva::GpioPin ledBlue{ hal::tiva::Port::F, 2, hal::tiva::Drive::Up };
        hal::tiva::GpioPin ledGreen{ hal::tiva::Port::F, 3, hal::tiva::Drive::Up };

        hal::tiva::GpioPin sw1{ hal::tiva::Port::F, 4 };
        hal::tiva::GpioPin sw2{ hal::tiva::Port::F, 0 };
    };

    struct LaunchPadTerminalAndTracer
    {
        hal::tiva::GpioPin traceUartTx{ hal::tiva::Port::A, 1 };

        TracerInfrastructure tracerInfrastructure{ { 0, traceUartTx } };
        services::Tracer& tracer{ tracerInfrastructure.tracer };
    };
}

#endif
