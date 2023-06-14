#ifndef HAL_TI_STM_TRACER_INFRASTRUCTURE_HPP
#define HAL_TI_STM_TRACER_INFRASTRUCTURE_HPP

#include "hal_tiva/tiva/GpioStm.hpp"
#include "hal_tiva/synchronous_tiva/SynchronousUartStm.hpp"
#include "services/tracer/StreamWriterOnSynchronousSerialCommunication.hpp"
#include "services/tracer/TracerWithDateTime.hpp"

namespace main_
{
    class TimeKeeperStub
        : public hal::TimeKeeper
    {
    public:
        virtual bool Timeout() override
        {
            return false;
        }

        virtual void Reset() override
        {}
    };

    struct StmTracerInfrastructure
    {
        struct Configuration
        {
            Configuration(uint8_t index, hal::GpioPinStm& tx, hal::GpioPinStm& rx);

            uint8_t index;
            hal::GpioPinStm& tx;
            hal::GpioPinStm& rx;
        };

        StmTracerInfrastructure(const Configuration& configuration, bool loggingEnabled = true);
#if defined(STM32WB)
        StmTracerInfrastructure(const Configuration& configuration, hal::SyncLpUart syncLpUart, bool loggingEnabled = true);
#endif
        infra::StreamWriter& GetStreamWriter(bool loggingEnabled);

        TimeKeeperStub timeKeeper;
        hal::SynchronousUartStmSendOnly traceUart;

        infra::StreamWriterDummy dummyWriter;
        services::StreamWriterOnSynchronousSerialCommunication traceWriter;
        infra::TextOutputStream::WithErrorPolicy alwaysEnabledTracerOutputStream;
        infra::TextOutputStream::WithErrorPolicy tracerOutputStream;
        services::TracerWithDateTime alwaysEnabledTracer;
        services::TracerWithDateTime tracer;
    };
}

#endif
