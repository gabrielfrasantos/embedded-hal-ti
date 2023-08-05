#include "hal_tiva/instantiations/EventInfrastructure.hpp"
#include "hal_tiva/instantiations/LaunchPadBsp.hpp"
#include "services/util/DebugLed.hpp"
#include "hal_tiva/tiva/Clock.hpp"

int main()
{
    hal::tiva::ConfigureClock(hal::tiva::crystalFrequency::_16_MHz, hal::tiva::oscillatorSource::main);

    static instantiations::EventInfrastructure eventInfrastructure;
    static instantiations::LaunchPadUi ui;
    static services::DebugLed debugLed(ui.ledBlue, std::chrono::milliseconds(100), std::chrono::milliseconds(1400));

    eventInfrastructure.Run();
    __builtin_unreachable();
}
