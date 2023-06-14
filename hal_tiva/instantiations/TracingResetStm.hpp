#ifndef HAL_TI_TRACING_RESET_STM_HPP
#define HAL_TI_TRACING_RESET_STM_HPP

#include "hal_tiva/tiva/ResetStm.hpp"
#include "services/tracer/TracingReset.hpp"

namespace main_
{
    struct TracingResetStm
    {
    private:
        hal::ResetStm resetStm;

    public:
        TracingResetStm(services::Tracer& tracer);

        services::TracingReset reset;
    };
}

#endif
