#ifndef HAL_SPI_MASTER_TIVA_HPP
#define HAL_SPI_MASTER_TIVA_HPP

#include "hal/interfaces/Spi.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/util/AutoResetFunction.hpp"

namespace hal::tiva
{
    class SpiMaster
        : public hal::SpiMaster
    {
    public:
        struct Config
        {
            constexpr Config()
            {}

            bool polarityLow = true;
            bool phase1st = true;
            uint32_t baudRate = 100000;
        };

        SpiMaster(uint8_t aSpiIndex, GpioPin& clock, GpioPin& miso, GpioPin& mosi, const Config& config = Config(), GpioPin& slaveSelect = dummyPin);
        ~SpiMaster();

        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& onDone) override;
        virtual void SetChipSelectConfigurator(ChipSelectConfigurator& configurator) override;
        virtual void SetCommunicationConfigurator(CommunicationConfigurator& configurator) override;
        virtual void ResetCommunicationConfigurator() override;

    private:
        void HandleInterrupt();
        void EnableClock();
        void DisableClock();

    private:
        uint8_t ssiIndex;
        PeripheralPin clock;
        PeripheralPin miso;
        PeripheralPin mosi;
        PeripheralPin slaveSelect;

        infra::MemoryRange<SSI0_Type* const> ssiArray;
        infra::MemoryRange<IRQn_Type const> irqArray;

        infra::AutoResetFunction<void()> onDone;
        ChipSelectConfigurator* chipSelectConfigurator = nullptr;
        CommunicationConfigurator* communicationConfigurator = nullptr;
        infra::Optional<ImmediateInterruptHandler> spiInterruptRegistration;
        infra::ConstByteRange sendData;
        infra::ByteRange receiveData;
        bool sending;
        bool receiving;
        uint32_t dummyToSend = 0;
        uint32_t dummyToReceive = 0;
        bool continuedSession = false;
    };
}

#endif
