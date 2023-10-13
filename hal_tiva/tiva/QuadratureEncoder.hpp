#ifndef HAL_QUADRATURE_ENCODER_TIVA_HPP
#define HAL_QUADRATURE_ENCODER_TIVA_HPP

#include "hal/interfaces/QuadratureEncoder.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/util/Function.hpp"

namespace hal::tiva
{
    class QuadratureEncoder
        : public hal::QuadratureEncoder
    {
    public:
        struct Config
        {
            constexpr Config()
            {}

            uint32_t resolution = 1024;
            uint32_t offset = 0;
            bool invertPhaseA = false;
            bool invertPhaseB = false;
            bool invertIndex = false;
        };

        QuadratureEncoder(uint8_t aQeiIndex, GpioPin& phaseA = dummyPin, GpioPin& phaseB = dummyPin, GpioPin& index = dummyPin, const Config& config = Config());
        ~QuadratureEncoder();

        void Position(uint32_t position) override;
        void Resolution(uint32_t resolution) override;

        uint32_t Position() override;
        uint32_t Resolution() override;

        QuadratureEncoder::MotionDirection Direction() override;
        void Direction(const infra::Function<void(QuadratureEncoder::MotionDirection)>& onDirectionChange) override;

        uint32_t Speed() override;

    private:
        void HandleInterrupt();
        void EnableClock();
        void DisableClock();

    private:
        uint8_t qeiIndex;
        PeripheralPin phaseA;
        PeripheralPin phaseB;
        PeripheralPin index;

        infra::MemoryRange<QEI0_Type* const> qeiArray;
        infra::MemoryRange<IRQn_Type const> irqArray;

        infra::Function<void(MotionDirection)> onDirectionChange;
        infra::Optional<ImmediateInterruptHandler> qeiInterruptRegistration;
    };
}

#endif
