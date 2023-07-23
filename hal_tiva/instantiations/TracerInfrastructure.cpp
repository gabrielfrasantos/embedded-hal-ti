#include "hal_tiva/instantiations/TracerInfrastructure.hpp"

namespace instantiations
{
    TracerInfrastructure::Configuration::Configuration(uint8_t index, hal::tiva::GpioPin& tx, hal::tiva::GpioPin& rx)
        : index(index)
        , tx(tx)
        , rx(rx)
    {}

    TracerInfrastructure::TracerInfrastructure(const Configuration& configuration, bool loggingEnabled)
        : traceUart(configuration.index, configuration.tx)
        , traceWriter(traceUart)
        , alwaysEnabledTracerOutputStream(traceWriter)
        , tracerOutputStream(GetStreamWriter(loggingEnabled), infra::noFail)
        , alwaysEnabledTracer(alwaysEnabledTracerOutputStream)
        , tracer(tracerOutputStream)
    {
        tracer.Trace() << "----------------------------------------------------------";
    }

    infra::StreamWriter& TracerInfrastructure::GetStreamWriter(bool loggingEnabled)
    {
        if (loggingEnabled)
            return traceWriter;
        else
            return dummyWriter;
    }
}
