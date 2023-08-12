#ifndef SYNCHRONOUS_HAL_SYNCHRONOUS_UART_STM_HPP
#define SYNCHRONOUS_HAL_SYNCHRONOUS_UART_STM_HPP

#include "hal/synchronous_interfaces/SynchronousSerialCommunication.hpp"
#include "hal/synchronous_interfaces/TimeKeeper.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/util/WithStorage.hpp"
#include <atomic>

namespace hal::tiva
{
    class SynchronousUart
        : public SynchronousSerialCommunication
        , private InterruptHandler
    {
    public:
        struct HwFlowControl
        {
            bool rtsEnable = true;
            bool ctsEnable = true;
        };

        template<std::size_t Size>
        using WithStorage = infra::WithStorage<SynchronousUart, std::array<uint8_t, Size>>;

        SynchronousUart(infra::ByteRange readBuffer, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, TimeKeeper& timeKeeper, uint32_t baudrate = 115200);
        SynchronousUart(infra::ByteRange readBuffer, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, GpioPin& uartRts, GpioPin& uartCts, TimeKeeper& timeKeeper,
            HwFlowControl flowControl, uint32_t baudrate = 115200);

        ~SynchronousUart();

        void SendData(infra::ConstByteRange data) override;
        bool ReceiveData(infra::ByteRange data) override;

    private:
        virtual void Invoke() override;

        bool Full() const;
        bool Empty() const;

    private:
        uint8_t uartIndex;
        PeripheralPin uartTx;
        PeripheralPin uartRx;
        infra::Optional<PeripheralPin> uartRts;
        infra::Optional<PeripheralPin> uartCts;
        TimeKeeper& timeKeeper;
        infra::ByteRange readBuffer;
        std::atomic<uint8_t*> contentsBegin;
        std::atomic<uint8_t*> contentsEnd;
    };

    class SynchronousUartSendOnly
        : public SynchronousSerialCommunication
    {
    public:
        struct HwFlowControl
        {
            bool rtsEnable = true;
            bool ctsEnable = true;
        };

        enum class Baudrate : uint32_t
        {
            _600_bps,
            _1200_bps,
            _2400_bps,
            _4800_bps,
            _9600_bps,
            _19200_bps,
            _38600_bps,
            _56700_bps,
            _115200_bps,
            _230400_bps,
            _460800_bps,
            _921000_bps,
        };

        enum class Parity : uint32_t
        {
            none,
            even,
            odd,
        };

        enum class StopBits : uint32_t
        {
            one,
            two,
        };

        enum class NumberOfBytes : uint32_t
        {
            _8_bytes,
            _16_bytes,
        };

        struct Config
        {
            constexpr Config()
            {}

            Baudrate baudrate = Baudrate::_115200_bps;
            Parity parity = Parity::none;
            StopBits stopbits = StopBits::one;
            NumberOfBytes numberOfBytes = NumberOfBytes::_8_bytes;
        };

        SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, const Config& config = Config());
        SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRts, HwFlowControl flowControl, const Config& config = Config());
        ~SynchronousUartSendOnly();

        void SendData(infra::ConstByteRange data) override;
        bool ReceiveData(infra::ByteRange data) override;

    private:
        void Initialization(HwFlowControl flowControl, const Config& config);
        void EnableClock();
        void DisableClock();

        uint8_t uartIndex;
        PeripheralPin uartTx;
        infra::Optional<PeripheralPin> uartRts;
        infra::MemoryRange<UART0_Type* const> uartArray;
    };
}

#endif
