#ifndef HAL_UART_WITH_DMA_TIVA_HPP
#define HAL_UART_WITH_DMA_TIVA_HPP

#include DEVICE_HEADER
#include "hal_tiva/tiva/Dma.hpp"
#include "hal_tiva/tiva/Uart.hpp"

namespace hal::tiva
{
    class UartWithDma
        : public Uart
    {
    public:
        template<std::size_t RxBufferSize>
        using WithRxBuffer = infra::WithStorage<infra::WithStorage<UartWithDma, std::array<uint8_t, RxBufferSize / 2>>, std::array<uint8_t, RxBufferSize / 2>>;

        UartWithDma(infra::MemoryRange<uint8_t> rxBuffer1, infra::MemoryRange<uint8_t> rxBuffer2, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, DmaChannel& dmaTx, DmaChannel& dmaRx, const Config& config = Config(true, true));
        UartWithDma(infra::MemoryRange<uint8_t> rxBuffer1, infra::MemoryRange<uint8_t> rxBuffer2, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, GpioPin& uartRts, GpioPin& uartCts, DmaChannel& dmaTx, DmaChannel& dmaRx, const Config& config = Config(true, true));
        virtual ~UartWithDma();

        void SendData(infra::MemoryRange<const uint8_t> data, infra::Function<void()> actionOnCompletion = infra::emptyFunction) override;
        void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) override;

    private:
        void Initialize() const;
        void SendData() const;
        void ReceiveData() const;
        void Invoke() override;
        void ProcessDmaRxInterrupt() const;

    private:
        const DmaChannel::Increment inc8Bits = DmaChannel::Increment::_8_bits;
        const DmaChannel::Increment incNone = DmaChannel::Increment::none;
        const DmaChannel::DataSize dataSize = DmaChannel::DataSize::_8_bits;
        const DmaChannel::ArbitrationSize arbSize = DmaChannel::ArbitrationSize::_4_items;
        const DmaChannel::Configuration configTx{ DmaChannel::Type::primary, inc8Bits, incNone, dataSize, arbSize };
        const DmaChannel::Configuration configRxPrimary{ DmaChannel::Type::primary, incNone, inc8Bits, dataSize, arbSize };
        const DmaChannel::Configuration configRxSecondary{ DmaChannel::Type::secondary, incNone, inc8Bits, dataSize, arbSize };
        const std::size_t maxTransferSize = 128;
        infra::MemoryRange<uint8_t> rxBuffer1;
        infra::MemoryRange<uint8_t> rxBuffer2;
        DmaChannel& dmaTx;
        DmaChannel& dmaRx;
        std::size_t bytesSent = 0;
    };
}

#endif
