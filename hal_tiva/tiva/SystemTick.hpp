#ifndef HAL_TI_SYSTEM_TICK_TIVA_HPP
#define HAL_TI_SYSTEM_TICK_TIVA_HPP

#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "infra/timer/Timer.hpp"

namespace hal
{
    class SystemTick
        : private InterruptHandler
    {
    public:
        SystemTick(const infra::Function<void()>& callback, infra::Duration tickDuration = std::chrono::milliseconds(1));

    private:
        void Invoke() override;

    private:
        infra::Function<void()> callback;
    };
}

#endif
