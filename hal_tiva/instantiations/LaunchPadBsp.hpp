#ifndef HAL_TI_LAUNCH_PAD_UI_HPP
#define HAL_TI_LAUNCH_PAD_UI_HPP

#include "hal_tiva/tiva/Gpio.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"

namespace instantiations
{
    struct LaunchPadUi
    {
        hal::tiva::GpioPin ledRed{ hal::Port::F, 1 };
        hal::tiva::GpioPin ledBlue{ hal::Port::F, 2 };
        hal::tiva::GpioPin ledGreen{ hal::Port::F, 3 };

        hal::tiva::GpioPin sw1{ hal::Port::F, 4 };
        hal::tiva::GpioPin sw2{ hal::Port::F, 0 };
    };

    struct LaunchPadTracer
    {
        hal::tiva::GpioPin traceUartTx{ hal::Port::A, 1 };
        hal::tiva::GpioPin traceUartRx{ hal::Port::A, 0 };

        main_::TracerInfrastructure tracerInfrastructure{ { 0, traceUartTx, traceUartRx } };
        services::Tracer& tracer{ tracerInfrastructure.tracer };
    };
}

#endif
