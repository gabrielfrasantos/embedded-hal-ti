#include "hal_tiva/instantiations/EventInfrastructure.hpp"

namespace instantiations
{
    EventInfrastructure::EventInfrastructure(infra::Duration tickDuration)
        : systemTick(tickDuration)
    {}

    void EventInfrastructure::Run()
    {
        eventDispatcher.Run();
    }
}
