#include "hal_tiva/cortex/SystemTickTimerService.hpp"
#include DEVICE_HEADER
#include "hal/interfaces/Gpio.hpp"

extern uint32_t SystemCoreClock;

namespace hal::cortex
{
    SystemTickTimerService::SystemTickTimerService(infra::Duration tickDuration, uint32_t id)
        : infra::TickOnInterruptTimerService(id, tickDuration)
    {
        Register(SysTick_IRQn);
        SysTick->LOAD = SystemCoreClock / (1000000000 / std::chrono::duration_cast<std::chrono::nanoseconds>(tickDuration).count()) - 1ul;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0;
    }

    infra::TimePoint SystemTickTimerService::Now() const
    {
        infra::TimePoint now;
        uint32_t adjust;

        do
        {
            static_cast<void>(SysTick->CTRL);
            now = infra::TickOnInterruptTimerService::Now();
            adjust = SysTick->LOAD - SysTick->VAL;
        } while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == SysTick_CTRL_COUNTFLAG_Msk);

        return now + std::chrono::nanoseconds(static_cast<uint64_t>(adjust) * 1000000000 / SystemCoreClock);
    }

    void SystemTickTimerService::Invoke()
    {
        SystemTickInterrupt();
    }
}

extern "C" uint32_t HAL_GetTick()
{
    if (hal::cortex::SystemTickTimerService::InstanceSet())
        return std::chrono::duration_cast<std::chrono::milliseconds>(hal::cortex::SystemTickTimerService::Instance().Now().time_since_epoch()).count();
    else
        return 0;
}
