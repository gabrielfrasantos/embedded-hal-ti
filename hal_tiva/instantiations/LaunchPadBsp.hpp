#ifndef HAL_TI_LAUNCH_PAD_UI_HPP
#define HAL_TI_LAUNCH_PAD_UI_HPP

#include "hal_tiva/tiva/Gpio.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"
#include "services/util/Terminal.hpp"

namespace instantiations
{
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
        hal::tiva::GpioPin traceUartRx{ hal::tiva::Port::A, 0 };

        TracerInfrastructure tracerInfrastructure{ { 0, traceUartTx, traceUartRx } };
        services::Tracer& tracer{ tracerInfrastructure.tracer };
        //services::TerminalWithCommandsImpl terminal{  };
    };
}

#endif
