#include "hal_tiva/instantiations/EventInfrastructure.hpp"
#include "hal_tiva/instantiations/EventInfrastructure.hpp"
#include "hal_tiva/instantiations/LaunchPadBsp.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"
#include "services/util/DebugLed.hpp"

int main()
{
    static instantiations::LaunchPad launchPad;
    static instantiations::EventInfrastructure eventInfrastructure;
    static instantiations::LaunchPadTerminalAndTracer tracer;
    static services::DebugLed debugLed{ launchPad.DebugLed(), std::chrono::milliseconds(100), std::chrono::milliseconds(1400) };

    tracer.tracer.Trace() << "------------";
    tracer.tracer.Trace() << "|   TEST   |";
    tracer.tracer.Trace() << "------------";

    eventInfrastructure.Run();
    __builtin_unreachable();
}
