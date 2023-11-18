#include "hal_tiva/tiva/UartWithDma.hpp"
#include "hal_tiva/tiva/Dma.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"
#include "infra/util/BoundedVector.hpp"
#include "infra/util/MemoryRange.hpp"

namespace hal::tiva
{
    namespace
    {
        constexpr const uint32_t UART_RIS_DMATXRIS = 0x00020000; // Transmit DMA Raw Interrupt Status
        constexpr const uint32_t UART_RIS_DMARXRIS = 0x00010000; // Receive DMA Raw Interrupt Status
        constexpr const uint32_t UART_RIS_9BITRIS = 0x00001000; // 9-Bit Mode Raw Interrupt Status
        constexpr const uint32_t UART_RIS_EOTRIS = 0x00000800; // End of Transmission Raw Interrupt Status
        constexpr const uint32_t UART_RIS_OERIS = 0x00000400; // UART Overrun Error Raw Interrupt Status
        constexpr const uint32_t UART_RIS_BERIS = 0x00000200; // UART Break Error Raw Interrupt Status
        constexpr const uint32_t UART_RIS_PERIS = 0x00000100; // UART Parity Error Raw Interrupt Status
        constexpr const uint32_t UART_RIS_FERIS = 0x00000080; // UART Framing Error Raw Interrupt Status
        constexpr const uint32_t UART_RIS_RTRIS = 0x00000040; // UART Receive Time-Out Raw Interrupt Status
        constexpr const uint32_t UART_RIS_TXRIS = 0x00000020; // UART Transmit Raw Interrupt Status
        constexpr const uint32_t UART_RIS_RXRIS = 0x00000010; // UART Receive Raw Interrupt Status
        constexpr const uint32_t UART_RIS_DSRRIS = 0x00000008; // UART Data Set Ready Modem Raw Interrupt Status
        constexpr const uint32_t UART_RIS_DCDRIS = 0x00000004; // UART Data Carrier Detect Modem Raw Interrupt Status
        constexpr const uint32_t UART_RIS_CTSRIS = 0x00000002; // UART Clear to Send Modem Raw Interrupt Status
        constexpr const uint32_t UART_RIS_RIRIS = 0x00000001; // UART Ring Indicator Modem Raw Interrupt Status

        constexpr const uint32_t UART_ICR_DMATXIC = 0x00020000; // Transmit DMA Interrupt Clear
        constexpr const uint32_t UART_ICR_DMARXIC = 0x00010000; // Receive DMA Interrupt Clear
        constexpr const uint32_t UART_ICR_9BITIC = 0x00001000; // 9-Bit Mode Interrupt Clear
        constexpr const uint32_t UART_ICR_EOTIC = 0x00000800; // End of Transmission Interrupt Clear
        constexpr const uint32_t UART_ICR_OEIC = 0x00000400; // Overrun Error Interrupt Clear
        constexpr const uint32_t UART_ICR_BEIC = 0x00000200; // Break Error Interrupt Clear
        constexpr const uint32_t UART_ICR_PEIC = 0x00000100; // Parity Error Interrupt Clear
        constexpr const uint32_t UART_ICR_FEIC = 0x00000080; // Framing Error Interrupt Clear
        constexpr const uint32_t UART_ICR_RTIC = 0x00000040; // Receive Time-Out Interrupt Clear
        constexpr const uint32_t UART_ICR_TXIC = 0x00000020; // Transmit Interrupt Clear
        constexpr const uint32_t UART_ICR_RXIC = 0x00000010; // Receive Interrupt Clear
        constexpr const uint32_t UART_ICR_DSRMIC = 0x00000008; // UART Data Set Ready Modem Interrupt Clear
        constexpr const uint32_t UART_ICR_DCDMIC = 0x00000004; // UART Data Carrier Detect Modem Interrupt Clear
        constexpr const uint32_t UART_ICR_CTSMIC = 0x00000002; // UART Clear to Send Modem Interrupt Clear
        constexpr const uint32_t UART_ICR_RIMIC = 0x00000001; // UART Ring Indicator Modem Interrupt Clear
    }

    UartWithDma::UartWithDma(infra::MemoryRange<uint8_t> rxBuffer1, infra::MemoryRange<uint8_t> rxBuffer2, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, DmaChannel& dmaTx, DmaChannel& dmaRx, const Config& config)
        : Uart(aUartIndex, uartTx, uartRx, config)
        , rxBuffer1(rxBuffer1)
        , rxBuffer2(rxBuffer2)
        , dmaTx(dmaTx)
        , dmaRx(dmaRx)
    {
        Initialize();
    }

    UartWithDma::UartWithDma(infra::MemoryRange<uint8_t> rxBuffer1, infra::MemoryRange<uint8_t> rxBuffer2, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, GpioPin& uartRts, GpioPin& uartCts, DmaChannel& dmaTx, DmaChannel& dmaRx, const Config& config)
        : Uart(aUartIndex, uartTx, uartRx, uartRts, uartCts, config)
        , rxBuffer1(rxBuffer1)
        , rxBuffer2(rxBuffer2)
        , dmaTx(dmaTx)
        , dmaRx(dmaRx)
    {
        Initialize();
    }

    UartWithDma::~UartWithDma()
    {
        // Disable AND/OR stop DMA....
    }

    void UartWithDma::Initialize() const
    {
        DisableUart();
        SetFifo(Uart::Fifo::_4_8, Uart::Fifo::_4_8);
        EnableDma();
        EnableUart();

        dmaTx.ConfigureChannel(configTx);
        dmaRx.ConfigureChannel(configRxPrimary);
        dmaRx.ConfigureChannel(configRxSecondary);
    }

    void UartWithDma::SendData(infra::MemoryRange<const uint8_t> data, infra::Function<void()> actionOnCompletion)
    {
        transferDataComplete = actionOnCompletion;
        sendData = data;
        sending = true;
        bytesSent = sendData.size() <= maxTransferSize ? sendData.size() : maxTransferSize;

        SendData();
    }

    void UartWithDma::ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived)
    {
        this->dataReceived = dataReceived;

        ReceiveData();
    }

    void UartWithDma::ReceiveData() const
    {
        dmaRx.ConfigureTransfer(DmaChannel::Type::primary, DmaChannel::Transfer::pingPong, &uartArray[uartIndex]->DR, rxBuffer1.begin(), rxBuffer1.size());
        dmaRx.ConfigureTransfer(DmaChannel::Type::secondary, DmaChannel::Transfer::pingPong, &uartArray[uartIndex]->DR, rxBuffer2.begin(), rxBuffer2.size());
        dmaRx.StartTransfer();
    }

    void UartWithDma::SendData() const
    {
        dmaTx.ConfigureTransfer(DmaChannel::Type::primary, DmaChannel::Transfer::basic, infra::ConstCastMemoryRange(sendData).begin(), &uartArray[uartIndex]->DR, bytesSent);
        dmaTx.StartTransfer();
    }

    void UartWithDma::Invoke()
    {
        volatile uint32_t status = uartArray[uartIndex]->RIS;
        uartArray[uartIndex]->ICR |= status;

        if (status & UART_RIS_DMARXRIS) // Data received
            ProcessDmaRxInterrupt();

#if 0
        if (status & UART_RIS_DMATXRIS) // Data transfered to UART fifo

        if (status & UART_RIS_RTRIS) // Rx timeout
#endif

        if (status & UART_RIS_EOTRIS && sending) // End of transmission
        {
            sendData.shrink_from_front_to(sendData.size() - bytesSent);
            bytesSent = sendData.size() <= maxTransferSize ? sendData.size() : maxTransferSize;

            if (!sendData.empty())
                SendData();

            if (sendData.empty())
            {
                sending = false;
                infra::EventDispatcher::Instance().Schedule(transferDataComplete);
                transferDataComplete = nullptr;
            }
        }
    }

    void UartWithDma::ProcessDmaRxInterrupt() const
    {
        if (dmaRx.Mode(DmaChannel::Type::primary) == DmaChannel::Transfer::stop)
        {
            this->dataReceived(rxBuffer1);
            dmaRx.ConfigureTransfer(DmaChannel::Type::primary, DmaChannel::Transfer::pingPong, &uartArray[uartIndex]->DR, rxBuffer1.begin(), rxBuffer1.size());
        }

        if (dmaRx.Mode(DmaChannel::Type::secondary) == DmaChannel::Transfer::stop)
        {
            this->dataReceived(rxBuffer2);
            dmaRx.ConfigureTransfer(DmaChannel::Type::secondary, DmaChannel::Transfer::pingPong, &uartArray[uartIndex]->DR, rxBuffer2.begin(), rxBuffer2.size());
        }
    }
}
