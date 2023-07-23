#include "hal_tiva/cortex/SystemTick.hpp"
#include DEVICE_HEADER

namespace hal::cortex
{
    SystemTick::SystemTick(const infra::Function<void()>& callback, infra::Duration tickDuration)
        : callback(callback)
    {
        Register(SysTick_IRQn);
        SysTick->LOAD = SystemCoreClock / (1000000000 / std::chrono::duration_cast<std::chrono::nanoseconds>(tickDuration).count()) - 1ul;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0;
    }

    void SystemTick::Invoke()
    {
        callback();
    }
}
