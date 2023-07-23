#ifndef HAL_TI_TRACING_RESET_HPP
#define HAL_TI_TRACING_RESET_HPP

#include "hal_tiva/cortex/Reset.hpp"
#include "services/tracer/TracingReset.hpp"

namespace instatiations
{
    struct TracingReset
    {
    private:
        hal::cortex::Reset reset;

    public:
        TracingReset(services::Tracer& tracer);

        services::TracingReset tracingReset;
    };
}

#endif
