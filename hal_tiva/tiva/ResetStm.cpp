#include "hal_tiva/tiva/ResetStm.hpp"
#include DEVICE_HEADER

namespace hal
{
    void ResetStm::ResetModule(const char* resetReason)
    {
        NVIC_SystemReset();
    }
}
