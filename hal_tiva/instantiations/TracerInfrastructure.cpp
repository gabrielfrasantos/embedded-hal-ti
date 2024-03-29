#include "hal_tiva/instantiations/TracerInfrastructure.hpp"

namespace instantiations
{
    TracerInfrastructure::Configuration::Configuration(uint8_t index, hal::tiva::GpioPin& tx)
        : index(index)
        , tx(tx)
    {}

    TracerInfrastructure::TracerInfrastructure(const Configuration& configuration, bool loggingEnabled)
        : traceUart(configuration.index, configuration.tx)
        , traceWriter(traceUart)
        , alwaysEnabledTracerOutputStream(traceWriter)
        , tracerOutputStream(GetStreamWriter(loggingEnabled), infra::noFail)
        , alwaysEnabledTracer(alwaysEnabledTracerOutputStream)
        , tracer(tracerOutputStream)
    {}

    infra::StreamWriter& TracerInfrastructure::GetStreamWriter(bool loggingEnabled)
    {
        if (loggingEnabled)
            return traceWriter;
        else
            return dummyWriter;
    }
}
