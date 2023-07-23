#include "hal_tiva/synchronous_tiva/SynchronousUart.hpp"

namespace hal::tiva
{
    SynchronousUart::SynchronousUart(infra::ByteRange readBuffer, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, TimeKeeper& timeKeeper, uint32_t baudrate)
        : SynchronousUart(readBuffer, aUartIndex, uartTx, uartRx, uartTx, uartRx, timeKeeper, {false, false}, baudrate)
    {}

    SynchronousUart::SynchronousUart(infra::ByteRange readBuffer, uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRx, GpioPin& uartRts, GpioPin& uartCts, TimeKeeper& timeKeeper,
        HwFlowControl flowControl, uint32_t baudrate)
        : uartIndex(aUartIndex)
        , uartTx(uartTx, PinConfigPeripheral::uartTx)
        , uartRx(uartRx, PinConfigPeripheral::uartRx)
        , timeKeeper(timeKeeper)
        , readBuffer(readBuffer)
        , contentsBegin(readBuffer.begin())
        , contentsEnd(readBuffer.begin())
    {
        if (flowControl.rtsEnable)
            this->uartRts.Emplace(uartRts, PinConfigPeripheral::uartRts);
        if (flowControl.ctsEnable)   
            this->uartCts.Emplace(uartCts, PinConfigPeripheral::uartCts);
    }

    SynchronousUart::~SynchronousUart()
    {

    }

    void SynchronousUart::SendData(infra::ConstByteRange data)
    {

    }

    bool SynchronousUart::ReceiveData(infra::ByteRange data)
    {
        timeKeeper.Reset();

        for (uint8_t& byte : data)
        {
            while (Empty())
            {
                if (timeKeeper.Timeout())
                    return false;
            }

            byte = *contentsBegin;

            if (contentsBegin == readBuffer.end() - 1)
                contentsBegin = readBuffer.begin();
            else
                ++contentsBegin;
        }

        return true;
    }

    void SynchronousUart::Invoke()
    {

    }

    bool SynchronousUart::Full() const
    {
        return (contentsEnd == readBuffer.end() - 1 || contentsBegin == contentsEnd + 1) && (contentsEnd != readBuffer.end() - 1 || contentsBegin == readBuffer.begin());
    }

    bool SynchronousUart::Empty() const
    {
        return contentsBegin.load() == contentsEnd.load();
    }

    SynchronousUartSendOnly::SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, uint32_t baudrate)
        : SynchronousUartSendOnly(aUartIndex, uartTx, uartTx, {false, false}, baudrate)
    {}

    SynchronousUartSendOnly::SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRts,
        HwFlowControl flowControl, uint32_t baudrate)
        : uartIndex(aUartIndex)
        , uartTx(uartTx, PinConfigPeripheral::uartTx)
    {

    }

    SynchronousUartSendOnly::~SynchronousUartSendOnly()
    {

    }

    void SynchronousUartSendOnly::SendData(infra::ConstByteRange data)
    {

    }

    bool SynchronousUartSendOnly::ReceiveData(infra::ByteRange data)
    {
        return false;
    }

    void SynchronousUartSendOnly::Initialization(HwFlowControl flowControl, uint32_t baudrate)
    {

    }
}
