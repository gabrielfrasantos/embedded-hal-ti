#include "hal_tiva/cortex/TimeKeeper.hpp"

namespace hal::cortex
{
    TimeKeeper::TimeKeeper(infra::Duration duration, SystemTickTimerService& timeService)
        : duration(duration)
        , timeService(timeService)
    {
        Reset();
    }

    bool TimeKeeper::Timeout()
    {
        return timeService.Now() > endTime;
    }

    void TimeKeeper::Reset()
    {
        endTime = timeService.Now() + duration;
    }
}
