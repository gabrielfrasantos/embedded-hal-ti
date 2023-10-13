#include "hal_tiva/synchronous_tiva/SynchronousSpiMaster.hpp"
#include "infra/util/BitLogic.hpp"

namespace hal::tiva
{
    namespace
    {
        constexpr const uint32_t SSI_CR0_SCR_M = 0x0000FF00;  // SSI Serial Clock Rate
        constexpr const uint32_t SSI_CR0_SPH_SPO_M = 0x000000C0;  // SSI Serial Clock Rate
        constexpr const uint32_t SSI_CR0_SPH = 0x00000080;  // SSI Serial Clock Phase
        constexpr const uint32_t SSI_CR0_SPO = 0x00000040;  // SSI Serial Clock Polarity
        constexpr const uint32_t SSI_CR0_FRF_M = 0x00000030;  // SSI Frame Format Select
        constexpr const uint32_t SSI_CR0_FRF_MOTO = 0x00000000;  // Freescale SPI Frame Format
        constexpr const uint32_t SSI_CR0_FRF_TI = 0x00000010;  // Synchronous Serial Frame Format
        constexpr const uint32_t SSI_CR0_DSS_M = 0x0000000F;  // SSI Data Size Select
        constexpr const uint32_t SSI_CR0_DSS_4 = 0x00000003;  // 4-bit data
        constexpr const uint32_t SSI_CR0_DSS_5 = 0x00000004;  // 5-bit data
        constexpr const uint32_t SSI_CR0_DSS_6 = 0x00000005;  // 6-bit data
        constexpr const uint32_t SSI_CR0_DSS_7 = 0x00000006;  // 7-bit data
        constexpr const uint32_t SSI_CR0_DSS_8 = 0x00000007;  // 8-bit data
        constexpr const uint32_t SSI_CR0_DSS_9 = 0x00000008;  // 9-bit data
        constexpr const uint32_t SSI_CR0_DSS_10 = 0x00000009;  // 10-bit data
        constexpr const uint32_t SSI_CR0_DSS_11 = 0x0000000A;  // 11-bit data
        constexpr const uint32_t SSI_CR0_DSS_12 = 0x0000000B;  // 12-bit data
        constexpr const uint32_t SSI_CR0_DSS_13 = 0x0000000C;  // 13-bit data
        constexpr const uint32_t SSI_CR0_DSS_14 = 0x0000000D;  // 14-bit data
        constexpr const uint32_t SSI_CR0_DSS_15 = 0x0000000E;  // 15-bit data
        constexpr const uint32_t SSI_CR0_DSS_16 = 0x0000000F;  // 16-bit data
        constexpr const uint32_t SSI_CR0_SCR_S = 8;
        
        constexpr const uint32_t SSI_CR1_EOM = 0x00000800;  // Stop Frame (End of Message)
        constexpr const uint32_t SSI_CR1_FSSHLDFRM = 0x00000400;  // FSS Hold Frame
        constexpr const uint32_t SSI_CR1_HSCLKEN = 0x00000200;  // High Speed Clock Enable
        constexpr const uint32_t SSI_CR1_DIR = 0x00000100;  // SSI Direction of Operation
        constexpr const uint32_t SSI_CR1_MODE_M = 0x000000C0;  // SSI Mode
        constexpr const uint32_t SSI_CR1_MODE_LEGACY = 0x00000000;  // Legacy SSI mode
        constexpr const uint32_t SSI_CR1_MODE_BI = 0x00000040;  // Bi-SSI mode
        constexpr const uint32_t SSI_CR1_MODE_QUAD = 0x00000080;  // Quad-SSI Mode
        constexpr const uint32_t SSI_CR1_EOT = 0x00000010;  // End of Transmission
        constexpr const uint32_t SSI_CR1_MS = 0x00000004;  // SSI Master/Slave Select
        constexpr const uint32_t SSI_CR1_SSE = 0x00000002;  // SSI Synchronous Serial Port Enable
        constexpr const uint32_t SSI_CR1_LBM = 0x00000001;  // SSI Loopback Mode

        constexpr const uint32_t SSI_DR_DATA_M = 0x0000FFFF;  // SSI Receive/Transmit Data
        constexpr const uint32_t SSI_DR_DATA_S = 0;

        constexpr const uint32_t SSI_SR_BSY = 0x00000010;  // SSI Busy Bit
        constexpr const uint32_t SSI_SR_RFF = 0x00000008;  // SSI Receive FIFO Full
        constexpr const uint32_t SSI_SR_RNE = 0x00000004;  // SSI Receive FIFO Not Empty
        constexpr const uint32_t SSI_SR_TNF = 0x00000002;  // SSI Transmit FIFO Not Full
        constexpr const uint32_t SSI_SR_TFE = 0x00000001;  // SSI Transmit FIFO Empty

        constexpr const uint32_t SSI_CPSR_CPSDVSR_M = 0x000000FF;  // SSI Clock Prescale Divisor
        constexpr const uint32_t SSI_CPSR_CPSDVSR_S = 0;

        constexpr const uint32_t SSI_IM_EOTIM = 0x00000040;  // End of Transmit Interrupt Mask
        constexpr const uint32_t SSI_IM_DMATXIM = 0x00000020;  // SSI Transmit DMA Interrupt Mask
        constexpr const uint32_t SSI_IM_DMARXIM = 0x00000010;  // SSI Receive DMA Interrupt Mask
        constexpr const uint32_t SSI_IM_TXIM = 0x00000008;  // SSI Transmit FIFO Interrupt Mask
        constexpr const uint32_t SSI_IM_RXIM = 0x00000004;  // SSI Receive FIFO Interrupt Mask
        constexpr const uint32_t SSI_IM_RTIM = 0x00000002;  // SSI Receive Time-Out Interrupt Mask
        constexpr const uint32_t SSI_IM_RORIM = 0x00000001;  // SSI Receive Overrun Interrupt Mask

        constexpr const uint32_t SSI_RIS_EOTRIS = 0x00000040;  // End of Transmit Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_DMATXRIS = 0x00000020;  // SSI Transmit DMA Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_DMARXRIS = 0x00000010;  // SSI Receive DMA Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_TXRIS = 0x00000008;  // SSI Transmit FIFO Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_RXRIS = 0x00000004;  // SSI Receive FIFO Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_RTRIS = 0x00000002;  // SSI Receive Time-Out Raw Interrupt Status
        constexpr const uint32_t SSI_RIS_RORRIS = 0x00000001;  // SSI Receive Overrun Raw Interrupt Status

        constexpr const uint32_t SSI_MIS_EOTMIS = 0x00000040;  // End of Transmit Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_DMATXMIS = 0x00000020;  // SSI Transmit DMA Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_DMARXMIS = 0x00000010;  // SSI Receive DMA Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_TXMIS = 0x00000008;  // SSI Transmit FIFO Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_RXMIS = 0x00000004;  // SSI Receive FIFO Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_RTMIS = 0x00000002;  // SSI Receive Time-Out Masked Interrupt Status
        constexpr const uint32_t SSI_MIS_RORMIS = 0x00000001;  // SSI Receive Overrun Masked Interrupt Status

        constexpr const uint32_t SSI_ICR_EOTIC = 0x00000040;  // End of Transmit Interrupt Clear
        constexpr const uint32_t SSI_ICR_DMATXIC = 0x00000020;  // SSI Transmit DMA Interrupt Clear
        constexpr const uint32_t SSI_ICR_DMARXIC = 0x00000010;  // SSI Receive DMA Interrupt Clear
        constexpr const uint32_t SSI_ICR_RTIC = 0x00000002;  // SSI Receive Time-Out Interrupt Clear
        constexpr const uint32_t SSI_ICR_RORIC = 0x00000001;  // SSI Receive Overrun Interrupt Clear

        constexpr const uint32_t SSI_DMACTL_TXDMAE = 0x00000002;  // Transmit DMA Enable
        constexpr const uint32_t SSI_DMACTL_RXDMAE = 0x00000001;  // Receive DMA Enable

        constexpr const uint32_t SSI_PP_FSSHLDFRM = 0x00000008;  // FSS Hold Frame Capability
        constexpr const uint32_t SSI_PP_MODE_M = 0x00000006;  // Mode of Operation
        constexpr const uint32_t SSI_PP_MODE_LEGACY = 0x00000000;  // Legacy SSI mode
        constexpr const uint32_t SSI_PP_MODE_ADVBI = 0x00000002;  // Legacy mode, Advanced SSI mode and Bi-SSI mode enabled
        constexpr const uint32_t SSI_PP_MODE_ADVBIQUAD = 0x00000004;  // Legacy mode, Advanced mode, Bi-SSI and Quad-SSI mode enabled
        constexpr const uint32_t SSI_PP_HSCLK = 0x00000001;  // High Speed Capability

        constexpr const uint32_t SSI_CC_CS_M = 0x0000000F;  // SSI Baud Clock Source
        constexpr const uint32_t SSI_CC_CS_SYSPLL = 0x00000000;  // System clock (based on clocksource and divisor factor)
        constexpr const uint32_t SSI_CC_CS_PIOSC = 0x00000005;  // PIOSC

        constexpr uint32_t phase_polarity(bool phase1st, bool polarityLow)
        {
            uint32_t phasePolarity = 0;

            if (!phase1st)
                phasePolarity |= SSI_CR0_SPH;

            if (!polarityLow)
                phasePolarity |= SSI_CR0_SPO;

            return phasePolarity;
        }

        constexpr std::array<uint32_t, 4> peripheralSsiArray =
        {{
            SSI0_BASE,
            SSI1_BASE,
            SSI2_BASE,
            SSI3_BASE,
        }};

        const infra::MemoryRange<SSI0_Type* const> peripheralSsi = infra::ReinterpretCastMemoryRange<SSI0_Type* const>(infra::MakeRange(peripheralSsiArray));

        extern "C" uint32_t SystemCoreClock;
    }

    SynchronousSpiMaster::SynchronousSpiMaster(uint8_t aSpiIndex, GpioPin& clock, GpioPin& miso, GpioPin& mosi, const Config& config)
        : ssiIndex(aSpiIndex)
        , clock(clock, PinConfigPeripheral::spiClock)
        , miso(miso, PinConfigPeripheral::spiMiso)
        , mosi(mosi, PinConfigPeripheral::spiMosi)
    {
        ssiArray = peripheralSsi;

        EnableClock();

        auto max = SystemCoreClock / config.baudRate;
        uint32_t div = 0;
        uint32_t scr = 0;
        do
        {
            div += 2;
            scr = (max / div) - 1;
        }
        while(scr > 255);

        ssiArray[ssiIndex]->CR1 &=~ SSI_CR1_SSE; /* Disable SPI */
        ssiArray[ssiIndex]->CC = SSI_CC_CS_SYSPLL; /* SSI clock is sourced by main system clock  */
        ssiArray[ssiIndex]->CR1 &=~ SSI_CR1_MS; /* Enable master mode */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_DSS_M) | SSI_CR0_DSS_8; /* Configure number of bits */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_FRF_M) | SSI_CR0_FRF_MOTO; /* Configure to SPI freescale format */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_SPH_SPO_M) | phase_polarity(config.phase1st, config.polarityLow); /* Configure SPI phase/polarity */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_SCR_M) | ((scr & 0xFF) << SSI_CR0_SCR_S); /* Sets clock rate */
        ssiArray[ssiIndex]->CPSR = (ssiArray[ssiIndex]->CPSR & ~SSI_CPSR_CPSDVSR_M) | div & 0xff; /* Sets prescaler */

        ssiArray[ssiIndex]->CR1 |= SSI_CR1_SSE; /* Enable SPI */

        while((ssiArray[ssiIndex]->SR & SSI_SR_RNE))
        {
        }
    }

    SynchronousSpiMaster::~SynchronousSpiMaster()
    {
        ssiArray[ssiIndex]->CR1 &=~ SSI_CR1_SSE; /* Disable SPI */
        DisableClock();
    }

    void SynchronousSpiMaster::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, Action nextAction)
    {
        really_assert(sendData.size() == receiveData.size() || sendData.empty() || receiveData.empty());

        for (std::size_t i = 0; i < sendData.size(); i++)
        {
            Send(sendData.empty() ? 0 : sendData[i]);
            auto data = Receive();

            if (!receiveData.empty())
                receiveData[i] = data;
        }

        while (ssiArray[ssiIndex]->SR & SSI_SR_BSY)
        {
        }
    }

    void SynchronousSpiMaster::Send(uint8_t data)
    {
        while(!(ssiArray[ssiIndex]->SR & SSI_SR_TNF))
        {
        }

        ssiArray[ssiIndex]->DR = data;
    }

    uint8_t SynchronousSpiMaster::Receive()
    {
        while(!(ssiArray[ssiIndex]->SR & SSI_SR_RNE))
        {
        }
        
        return ssiArray[ssiIndex]->DR;
    }

    void SynchronousSpiMaster::EnableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCSSI, true, ssiIndex);

        while (!infra::IsBitSet(SYSCTL->PRSSI, ssiIndex))
        {
        }
    }

    void SynchronousSpiMaster::DisableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCSSI, false, ssiIndex);
    }
}
