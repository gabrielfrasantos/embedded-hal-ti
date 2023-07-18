#include "hal_tiva/tiva/SpiMaster.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"

namespace hal::tiva
{
    namespace
    {
        constexpr const uint32_t SSI_CR0_SCR_M = 0x0000FF00;  // SSI Serial Clock Rate
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

        constexpr std::array<IRQn_Type, 4> peripheralIrqSsiArray =
        {{
            SSI0_IRQn,
            SSI1_IRQn,
            SSI2_IRQn,
            SSI3_IRQn,
        }};

        const infra::MemoryRange<SSI0_Type* const> peripheralSsi = infra::ReinterpretCastMemoryRange<SSI0_Type* const>(infra::MakeRange(peripheralSsiArray));

        extern "C" uint32_t SystemCoreClock;
    }

    SpiMaster::SpiMaster(uint8_t aSpiIndex, GpioPin& clock, GpioPin& miso, GpioPin& mosi, const Config& config, GpioPin& slaveSelect)
        : ssiIndex(aSpiIndex)
        , clock(clock, PinConfigPeripheral::spiClock)
        , miso(miso, PinConfigPeripheral::spiMiso)
        , mosi(mosi, PinConfigPeripheral::spiMosi)
        , slaveSelect(slaveSelect, PinConfigPeripheral::spiSlaveSelect)
    {
        ssiArray = peripheralSsi;
        irqArray = infra::MakeRange(peripheralIrqSsiArray);

        EnableClock();
/*        EnableClockSpi(ssiIndex);

        SPI_HandleTypeDef spiHandle;
        spiHandle.Instance = peripheralSpi[ssiIndex];
        spiHandle.Init.Mode = SPI_MODE_MASTER;
        spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
        spiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
        spiHandle.Init.CLKPolarity = config.polarityLow ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH;
        spiHandle.Init.CLKPhase = config.phase1st ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE;
        spiHandle.Init.NSS = SPI_NSS_SOFT;
        spiHandle.Init.BaudRatePrescaler = config.baudRatePrescaler;
        spiHandle.Init.FirstBit = config.msbFirst ? SPI_FIRSTBIT_MSB : SPI_FIRSTBIT_LSB;
        spiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
        spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        spiHandle.Init.CRCPolynomial = 1;
#if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32F7) || defined(STM32WB)
        spiHandle.Init.CRCLength = SPI_CRC_LENGTH_8BIT;
        spiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif
        HAL_SPI_Init(&spiHandle);

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32F7)
        peripheralSpi[ssiIndex]->CR2 |= SPI_RXFIFO_THRESHOLD_QF;
#endif

        __HAL_SPI_ENABLE(&spiHandle);*/

        ssiArray[ssiIndex]->CR1 &=~ SSI_CR1_SSE; /* Disable SPI */

        auto max = SystemCoreClock / config.baudRate;
        uint32_t div = 0;
        uint32_t scr = 0;
        do
        {
            div += 2;
            scr = (max / div) - 1;
        }
        while(scr > 255);

        ssiArray[ssiIndex]->CC = SSI_CC_CS_SYSPLL; /* SSI clock is sourced by main system clock  */
        ssiArray[ssiIndex]->CR1 &=~ SSI_CR1_MS; /* Enable master mode */
        ssiArray[ssiIndex]->CR1 |= SSI_CR1_EOT; /* Enable end of transmission */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_DSS_M) | SSI_CR0_DSS_8; /* Configure number of bits */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_FRF_M) | SSI_CR0_FRF_MOTO; /* Configure to SPI freescale format */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_SCR_M) | phase_polarity(config.phase1st, config.polarityLow); /* Configure SPI phase/polarity */
        ssiArray[ssiIndex]->CR0 = (ssiArray[ssiIndex]->CR0 & ~SSI_CR0_SCR_S) | ((scr & 0xFF) << 8); /* Sets clock rate */
        ssiArray[ssiIndex]->CPSR = (ssiArray[ssiIndex]->CPSR & ~SSI_CPSR_CPSDVSR_M) | div; /* Sets prescaler */

        ssiArray[ssiIndex]->CR1 |= SSI_CR1_SSE; /* Enable SPI */

    }

    SpiMaster::~SpiMaster()
    {
        DisableClock();
    }

    void SpiMaster::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& onDone)
    {
        this->onDone = onDone;
        if (chipSelectConfigurator && !continuedSession)
            chipSelectConfigurator->StartSession();
        continuedSession = nextAction == SpiAction::continueSession;

        assert(sendData.size() == receiveData.size() || sendData.empty() || receiveData.empty());
        this->sendData = sendData;
        this->receiveData = receiveData;
        sending = !sendData.empty();
        receiving = !receiveData.empty();

        if (!sending)
            dummyToSend = receiveData.size();
        if (!receiving)
            dummyToReceive = sendData.size();

        really_assert(!spiInterruptRegistration);
        spiInterruptRegistration.Emplace(irqArray[ssiIndex], [this]()
            {
                HandleInterrupt();
            });

        ssiArray[ssiIndex]->IM |= SSI_IM_TXIM | SSI_IM_RXIM | SSI_IM_RORIM; /* Enable RX/TX interrupt */
    }

    void SpiMaster::SetChipSelectConfigurator(ChipSelectConfigurator& configurator)
    {
        chipSelectConfigurator = &configurator;
    }

    void SpiMaster::SetCommunicationConfigurator(CommunicationConfigurator& configurator)
    {
        communicationConfigurator = &configurator;
        communicationConfigurator->ActivateConfiguration();
    }

    void SpiMaster::ResetCommunicationConfigurator()
    {
        if (communicationConfigurator)
            communicationConfigurator->DeactivateConfiguration();

        communicationConfigurator = nullptr;
    }

    void SpiMaster::HandleInterrupt()
    {
        uint32_t status = ssiArray[ssiIndex]->SR;

        if ((status & SSI_RIS_RORRIS))
        {
            ssiArray[ssiIndex]->ICR |= SSI_ICR_RORIC;

            really_assert(status & SSI_RIS_RORRIS);
        }

        if ((status & SSI_RIS_RXRIS))
        {
            if (dummyToReceive != 0)
            {
                (void)ssiArray[ssiIndex]->DR;
                --dummyToReceive;
            }
            else if (receiving)
            {
                receiveData.front() = ssiArray[ssiIndex]->DR;
                receiveData.pop_front();
            }

            receiving &= !receiveData.empty();

            if (dummyToReceive == 0 && !receiving)
                ssiArray[ssiIndex]->IM &= ~SSI_IM_RXIM;
        }

        if ((status & SSI_RIS_TXRIS) != 0)
        {
            if (dummyToSend != 0)
            {
                reinterpret_cast<volatile uint8_t&>(ssiArray[ssiIndex]->DR) = 0;
                --dummyToSend;
            }
            else if (sending)
            {
                reinterpret_cast<volatile uint8_t&>(ssiArray[ssiIndex]->DR) = sendData.front();
                sendData.pop_front();
            }

            sending &= !sendData.empty();

            // After the first transmit, disable interrupt on transmit buffer empty,
            // so that a receive is done before each transmit
            ssiArray[ssiIndex]->IM &= ~SSI_IM_TXIM;
        }

        spiInterruptRegistration->ClearPending();

        if (!sending && !receiving && dummyToSend == 0 && dummyToReceive == 0)
        {
            spiInterruptRegistration = infra::none;
            if (chipSelectConfigurator && !continuedSession)
                chipSelectConfigurator->EndSession();
            infra::EventDispatcher::Instance().Schedule([this]()
                {
                    onDone();
                });
        }
    }

    void SpiMaster::EnableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCSSI, true, ssiIndex);

        while (!infra::IsBitSet(SYSCTL->PRSSI, ssiIndex))
        {
        }
    }

    void SpiMaster::DisableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCSSI, false, ssiIndex);
    }
}
