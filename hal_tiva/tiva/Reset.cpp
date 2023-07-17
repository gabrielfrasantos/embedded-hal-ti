#include "hal_tiva/tiva/Reset.hpp"
#include DEVICE_HEADER

namespace hal::tiva
{
    void Reset::ResetModule(const char* resetReason)
    {
        NVIC_SystemReset();
    }
}
