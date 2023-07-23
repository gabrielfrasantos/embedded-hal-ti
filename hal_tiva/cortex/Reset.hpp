#ifndef HAL_RESET_TIVA_HPP
#define HAL_RESET_TIVA_HPP

#include "hal/interfaces/Reset.hpp"

namespace hal::cortex
{
    class Reset
        : public hal::Reset
    {
    public:
        virtual void ResetModule(const char* resetReason) override;
    };
}

#endif
