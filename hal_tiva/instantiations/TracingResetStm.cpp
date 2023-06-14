#include "hal_tiva/instantiations/TracingResetStm.hpp"

namespace main_
{
    TracingResetStm::TracingResetStm(services::Tracer& tracer)
        : reset(resetStm, tracer)
    {}
}
