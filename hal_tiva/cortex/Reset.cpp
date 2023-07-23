#include "hal_tiva/cortex/Reset.hpp"
#include DEVICE_HEADER

namespace hal::cortex
{
    void Reset::ResetModule(const char* resetReason)
    {
        NVIC_SystemReset();
    }
}
