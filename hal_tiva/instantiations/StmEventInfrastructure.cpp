#include "hal_tiva/instantiations/StmEventInfrastructure.hpp"
#include "generated/tiva/PinoutTableDefault.hpp"

namespace main_
{
    StmEventInfrastructure::StmEventInfrastructure(infra::Duration tickDuration)
        : gpio(hal::pinoutTableDefaultStm, hal::analogTableDefaultStm)
        , systemTick(tickDuration)
    {}

    void StmEventInfrastructure::Run()
    {
        eventDispatcher.Run();
    }
}
