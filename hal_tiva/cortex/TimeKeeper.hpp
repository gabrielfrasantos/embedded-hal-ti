#ifndef CORTEX_TIME_KEEPER_HPP
#define CORTEX_TIME_KEEPER_HPP

#include "hal/synchronous_interfaces/TimeKeeper.hpp"
#include "hal/synchronous_interfaces/TimeService.hpp"
#include "hal_tiva/cortex/SystemTickTimerService.hpp"

namespace hal::cortex
{
    class TimeKeeper
        : public hal::TimeKeeper
    {
    public:
        explicit TimeKeeper(infra::Duration duration, SystemTickTimerService& timeService);

        virtual bool Timeout() override;
        virtual void Reset() override;

    private:
        const infra::Duration duration;
        SystemTickTimerService& timeService;
        infra::TimePoint endTime;
    };
}

#endif
