#ifndef SYNCHRONOUS_HAL_SYNCHRONOUS_SPI_MASTER_HPP
#define SYNCHRONOUS_HAL_SYNCHRONOUS_SPI_MASTER_HPP

#include "hal/synchronous_interfaces/SynchronousSpi.hpp"
#include "hal_tiva/tiva/Gpio.hpp"

namespace hal::tiva
{
    class SynchronousSpiMaster
        : public SynchronousSpi
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

        SynchronousSpiMaster(uint8_t aSpiIndex, GpioPin& clock, GpioPin& miso, GpioPin& mosi, const Config& config = Config());
        ~SynchronousSpiMaster();

        // Implementation of SynchronousSpi
        void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, Action nextAction) override;

    private:
        void EnableClock();
        void DisableClock();

        void Send(uint8_t data);
        uint8_t Receive();

    private:
        uint8_t ssiIndex;
        PeripheralPin clock;
        PeripheralPin miso;
        PeripheralPin mosi;

        infra::MemoryRange<SSI0_Type* const> ssiArray;
    };

}

#endif
