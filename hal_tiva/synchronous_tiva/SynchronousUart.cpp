#include "hal_tiva/synchronous_tiva/SynchronousUart.hpp"
#include "infra/util/BitLogic.hpp"

namespace hal::tiva
{
    namespace
    {
        // NOLINTBEGIN
        constexpr const uint32_t UART_O_DR = 0x00000000; // UART Data
        constexpr const uint32_t UART_O_RSR = 0x00000004; // UART Receive Status/Error Clear
        constexpr const uint32_t UART_O_ECR = 0x00000004; // UART Receive Status/Error Clear
        constexpr const uint32_t UART_O_FR = 0x00000018; // UART Flag
        constexpr const uint32_t UART_O_ILPR = 0x00000020; // UART IrDA Low-Power Register
        constexpr const uint32_t UART_O_IBRD = 0x00000024; // UART Integer Baud-Rate Divisor
        constexpr const uint32_t UART_O_FBRD = 0x00000028; // UART Fractional Baud-Rate Divisor
        constexpr const uint32_t UART_O_LCRH = 0x0000002C; // UART Line Control
        constexpr const uint32_t UART_O_CTL = 0x00000030; // UART Control
        constexpr const uint32_t UART_O_IFLS = 0x00000034; // UART Interrupt FIFO Level Select
        constexpr const uint32_t UART_O_IM = 0x00000038; // UART Interrupt Mask
        constexpr const uint32_t UART_O_RIS = 0x0000003C; // UART Raw Interrupt Status
        constexpr const uint32_t UART_O_MIS = 0x00000040; // UART Masked Interrupt Status
        constexpr const uint32_t UART_O_ICR = 0x00000044; // UART Interrupt Clear
        constexpr const uint32_t UART_O_DMACTL = 0x00000048; // UART DMA Control
        constexpr const uint32_t UART_O_9BITADDR = 0x000000A4; // UART 9-Bit Self Address
        constexpr const uint32_t UART_O_9BITAMASK = 0x000000A8; // UART 9-Bit Self Address Mask
        constexpr const uint32_t UART_O_PP = 0x00000FC0; // UART Peripheral Properties
        constexpr const uint32_t UART_O_CC = 0x00000FC8; // UART Clock Configuration

        constexpr const uint32_t UART_DR_OE = 0x00000800; // UART Overrun Error
        constexpr const uint32_t UART_DR_BE = 0x00000400; // UART Break Error
        constexpr const uint32_t UART_DR_PE = 0x00000200; // UART Parity Error
        constexpr const uint32_t UART_DR_FE = 0x00000100; // UART Framing Error
        constexpr const uint32_t UART_DR_DATA_M = 0x000000FF; // Data Transmitted or Received
        constexpr const uint32_t UART_DR_DATA_S = 0;

        constexpr const uint32_t UART_RSR_OE = 0x00000008; // UART Overrun Error
        constexpr const uint32_t UART_RSR_BE = 0x00000004; // UART Break Error
        constexpr const uint32_t UART_RSR_PE = 0x00000002; // UART Parity Error
        constexpr const uint32_t UART_RSR_FE = 0x00000001; // UART Framing Error

        constexpr const uint32_t UART_ECR_DATA_M = 0x000000FF; // Error Clear
        constexpr const uint32_t UART_ECR_DATA_S = 0;

        constexpr const uint32_t UART_FR_RI = 0x00000100; // Ring Indicator
        constexpr const uint32_t UART_FR_TXFE = 0x00000080; // UART Transmit FIFO Empty
        constexpr const uint32_t UART_FR_RXFF = 0x00000040; // UART Receive FIFO Full
        constexpr const uint32_t UART_FR_TXFF = 0x00000020; // UART Transmit FIFO Full
        constexpr const uint32_t UART_FR_RXFE = 0x00000010; // UART Receive FIFO Empty
        constexpr const uint32_t UART_FR_BUSY = 0x00000008; // UART Busy
        constexpr const uint32_t UART_FR_DCD = 0x00000004; // Data Carrier Detect
        constexpr const uint32_t UART_FR_DSR = 0x00000002; // Data Set Ready
        constexpr const uint32_t UART_FR_CTS = 0x00000001; // Clear To Send

        constexpr const uint32_t UART_ILPR_ILPDVSR_M = 0x000000FF; // IrDA Low-Power Divisor
        constexpr const uint32_t UART_ILPR_ILPDVSR_S = 0;

        constexpr const uint32_t UART_IBRD_DIVINT_M = 0x0000FFFF; // Integer Baud-Rate Divisor
        constexpr const uint32_t UART_IBRD_DIVINT_S = 0;

        constexpr const uint32_t UART_FBRD_DIVFRAC_M = 0x0000003F; // Fractional Baud-Rate Divisor
        constexpr const uint32_t UART_FBRD_DIVFRAC_S = 0;

        constexpr const uint32_t UART_LCRH_SPS = 0x00000080; // UART Stick Parity Select
        constexpr const uint32_t UART_LCRH_WLEN_M = 0x00000060; // UART Word Length
        constexpr const uint32_t UART_LCRH_WLEN_5 = 0x00000000; // 5 bits (default)
        constexpr const uint32_t UART_LCRH_WLEN_6 = 0x00000020; // 6 bits
        constexpr const uint32_t UART_LCRH_WLEN_7 = 0x00000040; // 7 bits
        constexpr const uint32_t UART_LCRH_WLEN_8 = 0x00000060; // 8 bits
        constexpr const uint32_t UART_LCRH_FEN = 0x00000010; // UART Enable FIFOs
        constexpr const uint32_t UART_LCRH_STP2 = 0x00000008; // UART Two Stop Bits Select
        constexpr const uint32_t UART_LCRH_EPS = 0x00000004; // UART Even Parity Select
        constexpr const uint32_t UART_LCRH_PEN = 0x00000002; // UART Parity Enable
        constexpr const uint32_t UART_LCRH_BRK = 0x00000001; // UART Send Break

        constexpr const uint32_t UART_CTL_CTSEN = 0x00008000; // Enable Clear To Send
        constexpr const uint32_t UART_CTL_RTSEN = 0x00004000; // Enable Request to Send
        constexpr const uint32_t UART_CTL_RTS = 0x00000800; // Request to Send
        constexpr const uint32_t UART_CTL_DTR = 0x00000400; // Data Terminal Ready
        constexpr const uint32_t UART_CTL_RXE = 0x00000200; // UART Receive Enable
        constexpr const uint32_t UART_CTL_TXE = 0x00000100; // UART Transmit Enable
        constexpr const uint32_t UART_CTL_LBE = 0x00000080; // UART Loop Back Enable
        constexpr const uint32_t UART_CTL_HSE = 0x00000020; // High-Speed Enable
        constexpr const uint32_t UART_CTL_EOT = 0x00000010; // End of Transmission
        constexpr const uint32_t UART_CTL_SMART = 0x00000008; // ISO 7816 Smart Card Support
        constexpr const uint32_t UART_CTL_SIRLP = 0x00000004; // UART SIR Low-Power Mode
        constexpr const uint32_t UART_CTL_SIREN = 0x00000002; // UART SIR Enable
        constexpr const uint32_t UART_CTL_UARTEN = 0x00000001; // UART Enable

        constexpr const uint32_t UART_IFLS_RX_M = 0x00000038; // UART Receive Interrupt FIFO Level Select
        constexpr const uint32_t UART_IFLS_RX1_8 = 0x00000000; // RX FIFO >= 1/8 full
        constexpr const uint32_t UART_IFLS_RX2_8 = 0x00000008; // RX FIFO >= 1/4 full
        constexpr const uint32_t UART_IFLS_RX4_8 = 0x00000010; // RX FIFO >= 1/2 full (default)
        constexpr const uint32_t UART_IFLS_RX6_8 = 0x00000018; // RX FIFO >= 3/4 full
        constexpr const uint32_t UART_IFLS_RX7_8 = 0x00000020; // RX FIFO >= 7/8 full
        constexpr const uint32_t UART_IFLS_TX_M = 0x00000007; // UART Transmit Interrupt FIFO Level Select
        constexpr const uint32_t UART_IFLS_TX1_8 = 0x00000000; // TX FIFO <= 1/8 full
        constexpr const uint32_t UART_IFLS_TX2_8 = 0x00000001; // TX FIFO <= 1/4 full
        constexpr const uint32_t UART_IFLS_TX4_8 = 0x00000002; // TX FIFO <= 1/2 full (default)
        constexpr const uint32_t UART_IFLS_TX6_8 = 0x00000003; // TX FIFO <= 3/4 full
        constexpr const uint32_t UART_IFLS_TX7_8 = 0x00000004; // TX FIFO <= 7/8 full

        constexpr const uint32_t UART_IM_DMATXIM = 0x00020000; // Transmit DMA Interrupt Mask
        constexpr const uint32_t UART_IM_DMARXIM = 0x00010000; // Receive DMA Interrupt Mask
        constexpr const uint32_t UART_IM_9BITIM = 0x00001000; // 9-Bit Mode Interrupt Mask
        constexpr const uint32_t UART_IM_EOTIM = 0x00000800; // End of Transmission Interrupt Mask
        constexpr const uint32_t UART_IM_OEIM = 0x00000400; // UART Overrun Error Interrupt Mask
        constexpr const uint32_t UART_IM_BEIM = 0x00000200; // UART Break Error Interrupt Mask
        constexpr const uint32_t UART_IM_PEIM = 0x00000100; // UART Parity Error Interrupt Mask
        constexpr const uint32_t UART_IM_FEIM = 0x00000080; // UART Framing Error Interrupt Mask
        constexpr const uint32_t UART_IM_RTIM = 0x00000040; // UART Receive Time-Out Interrupt Mask
        constexpr const uint32_t UART_IM_TXIM = 0x00000020; // UART Transmit Interrupt Mask
        constexpr const uint32_t UART_IM_RXIM = 0x00000010; // UART Receive Interrupt Mask
        constexpr const uint32_t UART_IM_DSRMIM = 0x00000008; // UART Data Set Ready Modem Interrupt Mask
        constexpr const uint32_t UART_IM_DCDMIM = 0x00000004; // UART Data Carrier Detect Modem Interrupt Mask
        constexpr const uint32_t UART_IM_CTSMIM = 0x00000002; // UART Clear to Send Modem Interrupt Mask
        constexpr const uint32_t UART_IM_RIMIM = 0x00000001; // UART Ring Indicator Modem Interrupt Mask

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

        constexpr const uint32_t UART_MIS_DMATXMIS = 0x00020000; // Transmit DMA Masked Interrupt Status
        constexpr const uint32_t UART_MIS_DMARXMIS = 0x00010000; // Receive DMA Masked Interrupt Status
        constexpr const uint32_t UART_MIS_9BITMIS = 0x00001000; // 9-Bit Mode Masked Interrupt Status
        constexpr const uint32_t UART_MIS_EOTMIS = 0x00000800; // End of Transmission Masked Interrupt Status
        constexpr const uint32_t UART_MIS_OEMIS = 0x00000400; // UART Overrun Error Masked Interrupt Status
        constexpr const uint32_t UART_MIS_BEMIS = 0x00000200; // UART Break Error Masked Interrupt Status
        constexpr const uint32_t UART_MIS_PEMIS = 0x00000100; // UART Parity Error Masked Interrupt Status
        constexpr const uint32_t UART_MIS_FEMIS = 0x00000080; // UART Framing Error Masked Interrupt Status
        constexpr const uint32_t UART_MIS_RTMIS = 0x00000040; // UART Receive Time-Out Masked Interrupt Status
        constexpr const uint32_t UART_MIS_TXMIS = 0x00000020; // UART Transmit Masked Interrupt Status
        constexpr const uint32_t UART_MIS_RXMIS = 0x00000010; // UART Receive Masked Interrupt Status
        constexpr const uint32_t UART_MIS_DSRMIS = 0x00000008; // UART Data Set Ready Modem Masked Interrupt Status
        constexpr const uint32_t UART_MIS_DCDMIS = 0x00000004; // UART Data Carrier Detect Modem Masked Interrupt Status
        constexpr const uint32_t UART_MIS_CTSMIS = 0x00000002; // UART Clear to Send Modem Masked Interrupt Status
        constexpr const uint32_t UART_MIS_RIMIS = 0x00000001; // UART Ring Indicator Modem Masked Interrupt Status

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

        constexpr const uint32_t UART_DMACTL_DMAERR = 0x00000004; // DMA on Error
        constexpr const uint32_t UART_DMACTL_TXDMAE = 0x00000002; // Transmit DMA Enable
        constexpr const uint32_t UART_DMACTL_RXDMAE = 0x00000001; // Receive DMA Enable

        constexpr const uint32_t UART_9BITADDR_9BITEN = 0x00008000; // Enable 9-Bit Mode
        constexpr const uint32_t UART_9BITADDR_ADDR_M = 0x000000FF; // Self Address for 9-Bit Mode
        constexpr const uint32_t UART_9BITADDR_ADDR_S = 0;

        constexpr const uint32_t UART_9BITAMASK_MASK_M = 0x000000FF; // Self Address Mask for 9-Bit Mode
        constexpr const uint32_t UART_9BITAMASK_MASK_S = 0;

        constexpr const uint32_t UART_PP_MSE = 0x00000008; // Modem Support Extended
        constexpr const uint32_t UART_PP_MS = 0x00000004; // Modem Support
        constexpr const uint32_t UART_PP_NB = 0x00000002; // 9-Bit Support
        constexpr const uint32_t UART_PP_SC = 0x00000001; // Smart Card Support

        constexpr const uint32_t UART_CC_CS_M = 0x0000000F; // UART Baud Clock Source
        constexpr const uint32_t UART_CC_CS_SYSCLK = 0x00000000; // System clock (based on clock source and divisor factor)
        constexpr const uint32_t UART_CC_CS_PIOSC = 0x00000005; // PIOSC
        // NOLINTEND

        const std::array<uint32_t, 13> baudRateTiva{ {
            600,
            1200,
            2400,
            4800,
            9600,
            19200,
            38600,
            56700,
            115200,
            230400,
            460800,
            921000,
        } };

        const std::array<uint32_t, 4> parityTiva{ { 0x0, 0x6, 0x2 } };

        const std::array<uint32_t, 3> stopBitsTiva{ { 0x0, 0x8 } };

        constexpr std::array<uint32_t, 8> peripheralUartArray =
        {{
            UART0_BASE,
            UART1_BASE,
            UART2_BASE,
            UART3_BASE,
            UART4_BASE,
            UART5_BASE,
            UART6_BASE,
            UART7_BASE,
        }};

        const infra::MemoryRange<UART0_Type* const> peripheralUart = infra::ReinterpretCastMemoryRange<UART0_Type* const>(infra::MakeRange(peripheralUartArray));
    }

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

    SynchronousUartSendOnly::SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, const Config& config)
        : SynchronousUartSendOnly(aUartIndex, uartTx, uartTx, {false, false}, config)
    {}

    SynchronousUartSendOnly::SynchronousUartSendOnly(uint8_t aUartIndex, GpioPin& uartTx, GpioPin& uartRts, HwFlowControl flowControl, const Config& config)
        : uartIndex(aUartIndex)
        , uartTx(uartTx, PinConfigPeripheral::uartTx)
    {
        uartArray = peripheralUart;
        EnableClock();
        Initialization(config);
    }

    SynchronousUartSendOnly::~SynchronousUartSendOnly()
    {
        DisableUart();
        DisableClock();
    }

    void SynchronousUartSendOnly::SendData(infra::ConstByteRange data)
    {
        for (auto character : data)
            Transmit(character);
    }

    bool SynchronousUartSendOnly::ReceiveData(infra::ByteRange)
    {
        return false;
    }

    void SynchronousUartSendOnly::Transmit(uint8_t data) const
    {
        while (uartArray[uartIndex]->FR & UART_FR_TXFF) { }
        uartArray[uartIndex]->DR = data;
    }

    void SynchronousUartSendOnly::Initialization(const Config& config) const
    {
        bool is_hse = baudRateTiva.at(static_cast<uint8_t>(config.baudrate)) * 16 > SystemCoreClock;
        uint32_t baudrate = is_hse ? baudRateTiva.at(static_cast<uint8_t>(config.baudrate)) / 2 : baudRateTiva.at(static_cast<uint8_t>(config.baudrate));
        uint32_t div = (((SystemCoreClock * 8) / baudrate) + 1) / 2;
        uint32_t lcrh = parityTiva.at(static_cast<uint8_t>(config.parity));
        lcrh |= stopBitsTiva.at(static_cast<uint8_t>(config.stopbits));
        lcrh |= UART_LCRH_WLEN_8;

        DisableUart();
        uartArray[uartIndex]->CC = UART_CC_CS_SYSCLK;
        uartArray[uartIndex]->CTL = (uartArray[uartIndex]->CTL & ~UART_CTL_HSE) | (is_hse ? UART_CTL_HSE : 0);
        uartArray[uartIndex]->IBRD = div / 64;
        uartArray[uartIndex]->FBRD = div % 64;
        uartArray[uartIndex]->LCRH = lcrh;
        uartArray[uartIndex]->FR = 0;
        uartArray[uartIndex]->IFLS = UART_IFLS_TX7_8;
        EnableUart();
    }

    void SynchronousUartSendOnly::DisableUart() const
    {
        while (uartArray[uartIndex]->FR & UART_FR_BUSY) { }
        uartArray[uartIndex]->LCRH &=~ UART_LCRH_FEN;
        uartArray[uartIndex]->CTL &=~ UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
    }

    void SynchronousUartSendOnly::EnableUart() const
    {
        uartArray[uartIndex]->LCRH |= UART_LCRH_FEN;
        uartArray[uartIndex]->CTL |= UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
    }

    void SynchronousUartSendOnly::EnableClock() const
    {
        infra::ReplaceBit(SYSCTL->RCGCUART, true, uartIndex);

        while (!infra::IsBitSet(SYSCTL->PRUART, uartIndex))
        {
        }
    }

    void SynchronousUartSendOnly::DisableClock() const
    {
        infra::ReplaceBit(SYSCTL->RCGCUART, false, uartIndex);
    }
}
