#include "hal_tiva/instantiations/EventInfrastructure.hpp"
#include "hal_tiva/instantiations/LaunchPadBsp.hpp"
#include "hal_tiva/instantiations/TracerInfrastructure.hpp"
#include "services/util/DebugLed.hpp"
#include "hal_tiva/tiva/Clock.hpp"

int main()
{
    hal::tiva::ConfigureClock(hal::tiva::crystalFrequency::_16_MHz, hal::tiva::oscillatorSource::main);

    static instantiations::EventInfrastructure eventInfrastructure;
    static instantiations::LaunchPadUi ui;
    static instantiations::LaunchPadTerminalAndTracer tracer;
    static services::DebugLed debugLed(ui.ledGreen, std::chrono::milliseconds(100), std::chrono::milliseconds(1400));

    tracer.tracer.Trace() << "------------";
    tracer.tracer.Trace() << "|   TEST   |";
    tracer.tracer.Trace() << "------------";

    //static services::TerminalWithCommandsImpl();

    eventInfrastructure.Run();
    __builtin_unreachable();
}
