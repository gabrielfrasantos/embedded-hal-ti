#ifndef HAL_TI_TRACER_INFRASTRUCTURE_HPP
#define HAL_TI_TRACER_INFRASTRUCTURE_HPP

#include "hal_tiva/tiva/Gpio.hpp"
#include "hal_tiva/synchronous_tiva/SynchronousUart.hpp"
#include "services/tracer/StreamWriterOnSynchronousSerialCommunication.hpp"
#include "services/tracer/TracerWithDateTime.hpp"

namespace instantiations
{
    struct TracerInfrastructure
    {
        struct Configuration
        {
            Configuration(uint8_t index, hal::tiva::GpioPin& tx, hal::tiva::GpioPin& rx);

            uint8_t index;
            hal::tiva::GpioPin& tx;
            hal::tiva::GpioPin& rx;
        };

        TracerInfrastructure(const Configuration& configuration, bool loggingEnabled = true);
        infra::StreamWriter& GetStreamWriter(bool loggingEnabled);

        hal::tiva::SynchronousUartSendOnly traceUart;

        infra::StreamWriterDummy dummyWriter;
        services::StreamWriterOnSynchronousSerialCommunication traceWriter;
        infra::TextOutputStream::WithErrorPolicy alwaysEnabledTracerOutputStream;
        infra::TextOutputStream::WithErrorPolicy tracerOutputStream;
        services::TracerWithDateTime alwaysEnabledTracer;
        services::TracerWithDateTime tracer;
    };
}

#endif
