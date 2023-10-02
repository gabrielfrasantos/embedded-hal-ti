#include "hal_tiva/tiva/QuadratureEncoder.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"

#if defined(TM4C129)
#define NUMBER_OF_QEI   1
#else
#define NUMBER_OF_QEI   2
#endif

namespace hal::tiva
{
    namespace
    {
        constexpr const uint32_t QEI_CTL_FILTCNT_M = 0x000F0000;  // Input Filter Prescale Count
        constexpr const uint32_t QEI_CTL_FILTEN = 0x00002000;  // Enable Input Filter
        constexpr const uint32_t QEI_CTL_STALLEN = 0x00001000;  // Stall QEI
        constexpr const uint32_t QEI_CTL_INVI = 0x00000800;  // Invert Index Pulse
        constexpr const uint32_t QEI_CTL_INVB = 0x00000400;  // Invert PhB
        constexpr const uint32_t QEI_CTL_INVA = 0x00000200;  // Invert PhA
        constexpr const uint32_t QEI_CTL_VELDIV_M = 0x000001C0;  // Predivide Velocity
        constexpr const uint32_t QEI_CTL_VELDIV_1 = 0x00000000;  // QEI clock /1
        constexpr const uint32_t QEI_CTL_VELDIV_2 = 0x00000040;  // QEI clock /2
        constexpr const uint32_t QEI_CTL_VELDIV_4 = 0x00000080;  // QEI clock /4
        constexpr const uint32_t QEI_CTL_VELDIV_8 = 0x000000C0;  // QEI clock /8
        constexpr const uint32_t QEI_CTL_VELDIV_16 = 0x00000100;  // QEI clock /16
        constexpr const uint32_t QEI_CTL_VELDIV_32 = 0x00000140;  // QEI clock /32
        constexpr const uint32_t QEI_CTL_VELDIV_64 = 0x00000180;  // QEI clock /64
        constexpr const uint32_t QEI_CTL_VELDIV_128 = 0x000001C0;  // QEI clock /128
        constexpr const uint32_t QEI_CTL_VELEN = 0x00000020;  // Capture Velocity
        constexpr const uint32_t QEI_CTL_RESMODE = 0x00000010;  // Reset Mode
        constexpr const uint32_t QEI_CTL_CAPMODE = 0x00000008;  // Capture Mode
        constexpr const uint32_t QEI_CTL_SIGMODE = 0x00000004;  // Signal Mode
        constexpr const uint32_t QEI_CTL_SWAP = 0x00000002;  // Swap Signals
        constexpr const uint32_t QEI_CTL_ENABLE = 0x00000001;  // Enable QEI
        constexpr const uint32_t QEI_CTL_FILTCNT_S = 16;

        constexpr const uint32_t QEI_STAT_DIRECTION = 0x00000002;  // Direction of Rotation Error Detected

        constexpr const uint32_t QEI_POS_M = 0xFFFFFFFF;  // Current Position Integrator Value
        constexpr const uint32_t QEI_POS_S = 0;

        constexpr const uint32_t QEI_MAXPOS_M = 0xFFFFFFFF;  // Maximum Position Integrator Value
        constexpr const uint32_t QEI_MAXPOS_S = 0;

        constexpr const uint32_t QEI_LOAD_M = 0xFFFFFFFF;  // Velocity Timer Load Value
        constexpr const uint32_t QEI_LOAD_S = 0;

        constexpr const uint32_t QEI_TIME_M = 0xFFFFFFFF;  // Velocity Timer Current Value
        constexpr const uint32_t QEI_TIME_S = 0;

        constexpr const uint32_t QEI_COUNT_M = 0xFFFFFFFF;  // Velocity Pulse Count
        constexpr const uint32_t QEI_COUNT_S = 0;

        constexpr const uint32_t QEI_SPEED_M = 0xFFFFFFFF;  // Velocity
        constexpr const uint32_t QEI_SPEED_S = 0;

        constexpr const uint32_t QEI_INTEN_ERROR = 0x00000008;  // Phase Error Interrupt Enable
        constexpr const uint32_t QEI_INTEN_DIR = 0x00000004;  // Direction Change Interrupt Enable
        constexpr const uint32_t QEI_INTEN_TIMER = 0x00000002;  // Timer Expires Interrupt Enable
        constexpr const uint32_t QEI_INTEN_INDEX = 0x00000001;  // Index Pulse Detected Interrupt Enable

        constexpr const uint32_t QEI_RIS_ERROR = 0x00000008;  // Phase Error Detected
        constexpr const uint32_t QEI_RIS_DIR = 0x00000004;  // Direction Change Detected
        constexpr const uint32_t QEI_RIS_TIMER = 0x00000002;  // Velocity Timer Expired
        constexpr const uint32_t QEI_RIS_INDEX = 0x00000001;  // Index Pulse Asserted

        constexpr const uint32_t QEI_ISC_ERROR = 0x00000008;  // Phase Error Interrupt
        constexpr const uint32_t QEI_ISC_DIR = 0x00000004;  // Direction Change Interrupt
        constexpr const uint32_t QEI_ISC_TIMER = 0x00000002;  // Velocity Timer Expired Interrupt
        constexpr const uint32_t QEI_ISC_INDEX = 0x00000001;  // Index Pulse Interrupt

        constexpr std::array<uint32_t, NUMBER_OF_QEI> peripheralQeiArray =
        {{
            QEI0_BASE,
#if defined(TM4C123)
            QEI1_BASE,
#endif
        }};

        constexpr std::array<IRQn_Type, NUMBER_OF_QEI> peripheralIrqQeiArray =
        {{
            QEI0_IRQn,
#if defined(TM4C123)
            QEI1_IRQn,
#endif
        }};

        const infra::MemoryRange<QEI0_Type* const> peripheralQei = infra::ReinterpretCastMemoryRange<QEI0_Type* const>(infra::MakeRange(peripheralQeiArray));

        extern "C" uint32_t SystemCoreClock;
    }

    QuadratureEncoder::QuadratureEncoder(uint8_t aQeiIndex, GpioPin& phaseA, GpioPin& phaseB, GpioPin& index, const Config& config)
        : qeiIndex(aQeiIndex)
        , phaseA(phaseA, PinConfigPeripheral::qeiPhaseA)
        , phaseB(phaseB, PinConfigPeripheral::qeiPhaseB)
        , index(index, PinConfigPeripheral::qeiIndex)
    {
        qeiArray = peripheralQei;
        irqArray = infra::MakeRange(peripheralIrqQeiArray);

        EnableClock();

        qeiArray[qeiIndex]->CTL &=~QEI_CTL_ENABLE; /* Disable QEi */
		qeiArray[qeiIndex]->CTL |= QEI_CTL_RESMODE | QEI_CTL_CAPMODE | QEI_CTL_VELEN; /* Capture mode is enabled and also reset mode, velocity enabled */
		qeiArray[qeiIndex]->MAXPOS = config.resolution; /* max position is 1024 (encoder resolution) */
		qeiArray[qeiIndex]->POS = 0x7FFFFFFF; /* Configure position */
		qeiArray[qeiIndex]->CTL |= QEI_CTL_ENABLE; /* Enable QEi */

    }

    QuadratureEncoder::~QuadratureEncoder()
    {
        qeiArray[qeiIndex]->INTEN &=~QEI_INTEN_DIR;
        qeiInterruptRegistration = infra::none;
        qeiArray[qeiIndex]->CTL &=~QEI_CTL_ENABLE; /* Disable QEi */
        DisableClock();
    }

    void QuadratureEncoder::Position(uint32_t position)
    {
        qeiArray[qeiIndex]->POS = position;
    }

    void QuadratureEncoder::Resolution(uint32_t resolution)
    {
        qeiArray[qeiIndex]->MAXPOS = resolution;
    }

    uint32_t QuadratureEncoder::Position()
    {
        return qeiArray[qeiIndex]->POS;
    }

    uint32_t QuadratureEncoder::Resolution()
    {
        return qeiArray[qeiIndex]->MAXPOS;
    }

    QuadratureEncoder::MotionDirection QuadratureEncoder::Direction()
    {
        return qeiArray[qeiIndex]->STAT & QEI_STAT_DIRECTION ? QuadratureEncoder::MotionDirection::reverse : QuadratureEncoder::MotionDirection::forward;
    }

    void QuadratureEncoder::Direction(const infra::Function<void(MotionDirection)>& onDirectionChange)
    {
        this->onDirectionChange = onDirectionChange;

        qeiInterruptRegistration.Emplace(irqArray[qeiIndex], [this]()
            {
                HandleInterrupt();
            });

        qeiArray[qeiIndex]->INTEN |= QEI_INTEN_DIR;
    }

    uint32_t QuadratureEncoder::Speed()
    {
        return qeiArray[qeiIndex]->SPEED;
    }

    void QuadratureEncoder::HandleInterrupt()
    {
        if (qeiArray[qeiIndex]->RIS & QEI_RIS_DIR)
        {
            qeiArray[qeiIndex]->ISC |= QEI_ISC_DIR;

            if (this->onDirectionChange)
                this->onDirectionChange(Direction());
        }
    }

    void QuadratureEncoder::EnableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCQEI, true, qeiIndex);

        while (!infra::IsBitSet(SYSCTL->PRQEI, qeiIndex))
        {
        }
    }

    void QuadratureEncoder::DisableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCQEI, false, qeiIndex);
    }
}
