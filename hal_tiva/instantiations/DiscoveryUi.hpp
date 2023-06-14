#ifndef HAL_TI_DISCOVERY_UI_HPP
#define HAL_TI_DISCOVERY_UI_HPP

#include "hal_tiva/tiva/GpioStm.hpp"

namespace main_
{
    struct Discovery32f746Ui
    {
        hal::GpioPinStm userButtonPin{ hal::Port::I, 11 };
        hal::GpioPinStm ledGreen{ hal::Port::I, 1 };
    };
}

#endif
