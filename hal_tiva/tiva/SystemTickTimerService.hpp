#ifndef HAL_TI_SYSTEM_TICK_TIMER_SERVICE_HPP
#define HAL_TI_SYSTEM_TICK_TIMER_SERVICE_HPP

#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "infra/timer/TickOnInterruptTimerService.hpp"
#include "infra/util/InterfaceConnector.hpp"

namespace hal
{
    class SystemTickTimerService
        : public infra::InterfaceConnector<SystemTickTimerService>
        , public infra::TickOnInterruptTimerService
        , private InterruptHandler
    {
    public:
        SystemTickTimerService(infra::Duration tickDuration = std::chrono::milliseconds(1), uint32_t id = infra::systemTimerServiceId);

        virtual infra::TimePoint Now() const override;

    private:
        void Invoke() override;
    };
}

#endif
