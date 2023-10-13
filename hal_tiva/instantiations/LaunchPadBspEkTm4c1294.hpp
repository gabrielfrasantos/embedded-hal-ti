#ifndef HAL_TI_LAUNCH_PAD_EK_TM4C1294_HPP
#define HAL_TI_LAUNCH_PAD_EK_TM4C1294_HPP

#include "hal_tiva/tiva/ClockTm4c129.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"
#include "services/util/Terminal.hpp"

namespace instantiations
{
    struct LaunchPadClock
    {
        uint32_t frequency = 120000000;
        hal::tiva::crystalFrequency hseValue = hal::tiva::crystalFrequency::_25_MHz;
        hal::tiva::oscillatorSource oscSource = hal::tiva::oscillatorSource::main;
        hal::tiva::systemClockVco systemClockVco = hal::tiva::systemClockVco::_240_MHz;
        bool usesPll = true;
    };

    struct LaunchPadUi
    {
        hal::tiva::GpioPin led1{ hal::tiva::Port::N, 1, hal::tiva::Drive::Up };
        hal::tiva::GpioPin led2{ hal::tiva::Port::N, 0, hal::tiva::Drive::Up };

        hal::tiva::GpioPin sw1{ hal::tiva::Port::J, 0 };
        hal::tiva::GpioPin sw2{ hal::tiva::Port::J, 1 };
    };

    struct LaunchPadTerminalAndTracer
    {
        hal::tiva::GpioPin traceUartTx{ hal::tiva::Port::A, 1 };

        TracerInfrastructure tracerInfrastructure{ { 0, traceUartTx } };
        services::Tracer& tracer{ tracerInfrastructure.tracer };
    };
}

#endif
