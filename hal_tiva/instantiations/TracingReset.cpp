#include "hal_tiva/instantiations/TracingReset.hpp"

namespace instatiations
{
    TracingReset::TracingReset(services::Tracer& tracer)
        : tracingReset(reset, tracer)
    {}
}
