#include "hal_tiva/tiva/Pwm.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"

namespace hal::tiva
{
    namespace
    {
        constexpr const uint32_t PWM_CTL_GLOBALSYNC3 = 0x00000008;  // Update PWM Generator 3
        constexpr const uint32_t PWM_CTL_GLOBALSYNC2 = 0x00000004;  // Update PWM Generator 2
        constexpr const uint32_t PWM_CTL_GLOBALSYNC1 = 0x00000002;  // Update PWM Generator 1
        constexpr const uint32_t PWM_CTL_GLOBALSYNC0 = 0x00000001;  // Update PWM Generator 0

        constexpr const uint32_t PWM_SYNC_SYNC3 = 0x00000008;  // Reset Generator 3 Counter
        constexpr const uint32_t PWM_SYNC_SYNC2 = 0x00000004;  // Reset Generator 2 Counter
        constexpr const uint32_t PWM_SYNC_SYNC1 = 0x00000002;  // Reset Generator 1 Counter
        constexpr const uint32_t PWM_SYNC_SYNC0 = 0x00000001;  // Reset Generator 0 Counter

        constexpr const uint32_t PWM_ENABLE_PWM7EN = 0x00000080;  // MnPWM7 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM6EN = 0x00000040;  // MnPWM6 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM5EN = 0x00000020;  // MnPWM5 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM4EN = 0x00000010;  // MnPWM4 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM3EN = 0x00000008;  // MnPWM3 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM2EN = 0x00000004;  // MnPWM2 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM1EN = 0x00000002;  // MnPWM1 Output Enable
        constexpr const uint32_t PWM_ENABLE_PWM0EN = 0x00000001;  // MnPWM0 Output Enable

        constexpr const uint32_t PWM_INVERT_PWM7INV = 0x00000080;  // Invert MnPWM7 Signal
        constexpr const uint32_t PWM_INVERT_PWM6INV = 0x00000040;  // Invert MnPWM6 Signal
        constexpr const uint32_t PWM_INVERT_PWM5INV = 0x00000020;  // Invert MnPWM5 Signal
        constexpr const uint32_t PWM_INVERT_PWM4INV = 0x00000010;  // Invert MnPWM4 Signal
        constexpr const uint32_t PWM_INVERT_PWM3INV = 0x00000008;  // Invert MnPWM3 Signal
        constexpr const uint32_t PWM_INVERT_PWM2INV = 0x00000004;  // Invert MnPWM2 Signal
        constexpr const uint32_t PWM_INVERT_PWM1INV = 0x00000002;  // Invert MnPWM1 Signal
        constexpr const uint32_t PWM_INVERT_PWM0INV = 0x00000001;  // Invert MnPWM0 Signal

        constexpr const uint32_t PWM_FAULT_FAULT7 = 0x00000080;  // MnPWM7 Fault
        constexpr const uint32_t PWM_FAULT_FAULT6 = 0x00000040;  // MnPWM6 Fault
        constexpr const uint32_t PWM_FAULT_FAULT5 = 0x00000020;  // MnPWM5 Fault
        constexpr const uint32_t PWM_FAULT_FAULT4 = 0x00000010;  // MnPWM4 Fault
        constexpr const uint32_t PWM_FAULT_FAULT3 = 0x00000008;  // MnPWM3 Fault
        constexpr const uint32_t PWM_FAULT_FAULT2 = 0x00000004;  // MnPWM2 Fault
        constexpr const uint32_t PWM_FAULT_FAULT1 = 0x00000002;  // MnPWM1 Fault
        constexpr const uint32_t PWM_FAULT_FAULT0 = 0x00000001;  // MnPWM0 Fault

        constexpr const uint32_t PWM_INTEN_INTFAULT3 = 0x00080000;  // Interrupt Fault 3
        constexpr const uint32_t PWM_INTEN_INTFAULT2 = 0x00040000;  // Interrupt Fault 2
        constexpr const uint32_t PWM_INTEN_INTFAULT1 = 0x00020000;  // Interrupt Fault 1
        constexpr const uint32_t PWM_INTEN_INTFAULT0 = 0x00010000;  // Interrupt Fault 0
        constexpr const uint32_t PWM_INTEN_INTPWM3 = 0x00000008;  // PWM3 Interrupt Enable
        constexpr const uint32_t PWM_INTEN_INTPWM2 = 0x00000004;  // PWM2 Interrupt Enable
        constexpr const uint32_t PWM_INTEN_INTPWM1 = 0x00000002;  // PWM1 Interrupt Enable
        constexpr const uint32_t PWM_INTEN_INTPWM0 = 0x00000001;  // PWM0 Interrupt Enable

        constexpr const uint32_t PWM_RIS_INTFAULT3 = 0x00080000;  // Interrupt Fault PWM 3
        constexpr const uint32_t PWM_RIS_INTFAULT2 = 0x00040000;  // Interrupt Fault PWM 2
        constexpr const uint32_t PWM_RIS_INTFAULT1 = 0x00020000;  // Interrupt Fault PWM 1
        constexpr const uint32_t PWM_RIS_INTFAULT0 = 0x00010000;  // Interrupt Fault PWM 0
        constexpr const uint32_t PWM_RIS_INTPWM3 = 0x00000008;  // PWM3 Interrupt Asserted
        constexpr const uint32_t PWM_RIS_INTPWM2 = 0x00000004;  // PWM2 Interrupt Asserted
        constexpr const uint32_t PWM_RIS_INTPWM1 = 0x00000002;  // PWM1 Interrupt Asserted
        constexpr const uint32_t PWM_RIS_INTPWM0 = 0x00000001;  // PWM0 Interrupt Asserted

        constexpr const uint32_t PWM_ISC_INTFAULT3 = 0x00080000;  // FAULT3 Interrupt Asserted
        constexpr const uint32_t PWM_ISC_INTFAULT2 = 0x00040000;  // FAULT2 Interrupt Asserted
        constexpr const uint32_t PWM_ISC_INTFAULT1 = 0x00020000;  // FAULT1 Interrupt Asserted
        constexpr const uint32_t PWM_ISC_INTFAULT0 = 0x00010000;  // FAULT0 Interrupt Asserted
        constexpr const uint32_t PWM_ISC_INTPWM3 = 0x00000008;  // PWM3 Interrupt Status
        constexpr const uint32_t PWM_ISC_INTPWM2 = 0x00000004;  // PWM2 Interrupt Status
        constexpr const uint32_t PWM_ISC_INTPWM1 = 0x00000002;  // PWM1 Interrupt Status
        constexpr const uint32_t PWM_ISC_INTPWM0 = 0x00000001;  // PWM0 Interrupt Status

        constexpr const uint32_t PWM_STATUS_FAULT3 = 0x00000008;  // Generator 3 Fault Status
        constexpr const uint32_t PWM_STATUS_FAULT2 = 0x00000004;  // Generator 2 Fault Status
        constexpr const uint32_t PWM_STATUS_FAULT1 = 0x00000002;  // Generator 1 Fault Status
        constexpr const uint32_t PWM_STATUS_FAULT0 = 0x00000001;  // Generator 0 Fault Status

        constexpr const uint32_t PWM_FAULTVAL_PWM7 = 0x00000080;  // MnPWM7 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM6 = 0x00000040;  // MnPWM6 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM5 = 0x00000020;  // MnPWM5 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM4 = 0x00000010;  // MnPWM4 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM3 = 0x00000008;  // MnPWM3 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM2 = 0x00000004;  // MnPWM2 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM1 = 0x00000002;  // MnPWM1 Fault Value
        constexpr const uint32_t PWM_FAULTVAL_PWM0 = 0x00000001;  // MnPWM0 Fault Value

        constexpr const uint32_t PWM_ENUPD_ENUPD7_M = 0x0000C000;  // MnPWM7 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD7_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD7_LSYNC = 0x00008000;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD7_GSYNC = 0x0000C000;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD6_M = 0x00003000;  // MnPWM6 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD6_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD6_LSYNC = 0x00002000;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD6_GSYNC = 0x00003000;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD5_M = 0x00000C00;  // MnPWM5 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD5_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD5_LSYNC = 0x00000800;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD5_GSYNC = 0x00000C00;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD4_M = 0x00000300;  // MnPWM4 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD4_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD4_LSYNC = 0x00000200;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD4_GSYNC = 0x00000300;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD3_M = 0x000000C0;  // MnPWM3 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD3_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD3_LSYNC = 0x00000080;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD3_GSYNC = 0x000000C0;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD2_M = 0x00000030;  // MnPWM2 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD2_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD2_LSYNC = 0x00000020;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD2_GSYNC = 0x00000030;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD1_M = 0x0000000C;  // MnPWM1 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD1_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD1_LSYNC = 0x00000008;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD1_GSYNC = 0x0000000C;  // Globally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD0_M = 0x00000003;  // MnPWM0 Enable Update Mode
        constexpr const uint32_t PWM_ENUPD_ENUPD0_IMM = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_ENUPD_ENUPD0_LSYNC = 0x00000002;  // Locally Synchronized
        constexpr const uint32_t PWM_ENUPD_ENUPD0_GSYNC = 0x00000003;  // Globally Synchronized

        constexpr const uint32_t PWM_CHANNEL_CTL_LATCH = 0x00040000;  // Latch Fault Input
        constexpr const uint32_t PWM_CHANNEL_CTL_MINFLTPER = 0x00020000;  // Minimum Fault Period
        constexpr const uint32_t PWM_CHANNEL_CTL_FLTSRC = 0x00010000;  // Fault Condition Source
        constexpr const uint32_t PWM_CHANNEL_CTL_DBFALLUPD_M = 0x0000C000;  // PWMnDBFALL Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_DBFALLUPD_I = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_CHANNEL_CTL_DBFALLUPD_LS = 0x00008000;  // Locally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_DBFALLUPD_GS = 0x0000C000;  // Globally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_DBRISEUPD_M = 0x00003000;  // PWMnDBRISE Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_DBRISEUPD_I = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_CHANNEL_CTL_DBRISEUPD_LS = 0x00002000;  // Locally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_DBRISEUPD_GS = 0x00003000;  // Globally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_DBCTLUPD_M = 0x00000C00;  // PWMnDBCTL Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_DBCTLUPD_I = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_CHANNEL_CTL_DBCTLUPD_LS = 0x00000800;  // Locally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_DBCTLUPD_GS = 0x00000C00;  // Globally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_GENBUPD_M = 0x00000300;  // PWMnGENB Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_GENBUPD_I = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_CHANNEL_CTL_GENBUPD_LS = 0x00000200;  // Locally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_GENBUPD_GS = 0x00000300;  // Globally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_GENAUPD_M = 0x000000C0;  // PWMnGENA Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_GENAUPD_I = 0x00000000;  // Immediate
        constexpr const uint32_t PWM_CHANNEL_CTL_GENAUPD_LS = 0x00000080;  // Locally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_GENAUPD_GS = 0x000000C0;  // Globally Synchronized
        constexpr const uint32_t PWM_CHANNEL_CTL_CMPBUPD = 0x00000020;  // Comparator B Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_CMPAUPD = 0x00000010;  // Comparator A Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_LOADUPD = 0x00000008;  // Load Register Update Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_DEBUG = 0x00000004;  // Debug Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_MODE = 0x00000002;  // Counter Mode
        constexpr const uint32_t PWM_CHANNEL_CTL_ENABLE = 0x00000001;  // PWM Block Enable

        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCMPBD = 0x00002000;  // Trigger for Counter=PWMnCMPB Down
        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCMPBU = 0x00001000;  // Trigger for Counter=PWMnCMPB Up
        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCMPAD = 0x00000800;  // Trigger for Counter=PWMnCMPA Down
        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCMPAU = 0x00000400;  // Trigger for Counter=PWMnCMPA Up
        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCNTLOAD = 0x00000200;  // Trigger for Counter=PWMnLOAD
        constexpr const uint32_t PWM_CHANNEL_INTEN_TRCNTZERO = 0x00000100;  // Trigger for Counter=0
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCMPBD = 0x00000020;  // Interrupt for Counter=PWMnCMPB Down
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCMPBU = 0x00000010;  // Interrupt for Counter=PWMnCMPB Up
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCMPAD = 0x00000008;  // Interrupt for Counter=PWMnCMPA Down
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCMPAU = 0x00000004;  // Interrupt for Counter=PWMnCMPA Up
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCNTLOAD = 0x00000002;  // Interrupt for Counter=PWMnLOAD
        constexpr const uint32_t PWM_CHANNEL_INTEN_INTCNTZERO = 0x00000001;  // Interrupt for Counter=0

        constexpr const uint32_t PWM_CHANNEL_RIS_INTCMPBD = 0x00000020;  // Comparator B Down Interrupt Status
        constexpr const uint32_t PWM_CHANNEL_RIS_INTCMPBU = 0x00000010;  // Comparator B Up Interrupt Status
        constexpr const uint32_t PWM_CHANNEL_RIS_INTCMPAD = 0x00000008;  // Comparator A Down Interrupt Status
        constexpr const uint32_t PWM_CHANNEL_RIS_INTCMPAU = 0x00000004;  // Comparator A Up Interrupt Status
        constexpr const uint32_t PWM_CHANNEL_RIS_INTCNTLOAD = 0x00000002;  // Counter=Load Interrupt Status
        constexpr const uint32_t PWM_CHANNEL_RIS_INTCNTZERO = 0x00000001;  // Counter=0 Interrupt Status

        constexpr const uint32_t PWM_CHANNEL_ISC_INTCMPBD = 0x00000020;  // Comparator B Down Interrupt
        constexpr const uint32_t PWM_CHANNEL_ISC_INTCMPBU = 0x00000010;  // Comparator B Up Interrupt
        constexpr const uint32_t PWM_CHANNEL_ISC_INTCMPAD = 0x00000008;  // Comparator A Down Interrupt
        constexpr const uint32_t PWM_CHANNEL_ISC_INTCMPAU = 0x00000004;  // Comparator A Up Interrupt
        constexpr const uint32_t PWM_CHANNEL_ISC_INTCNTLOAD = 0x00000002;  // Counter=Load Interrupt
        constexpr const uint32_t PWM_CHANNEL_ISC_INTCNTZERO = 0x00000001;  // Counter=0 Interrupt

        constexpr const uint32_t PWM_CHANNEL_LOAD_M = 0x0000FFFF;  // Counter Load Value
        constexpr const uint32_t PWM_CHANNEL_LOAD_S = 0;

        constexpr const uint32_t PWM_CHANNEL_COUNT_M = 0x0000FFFF;  // Counter Value
        constexpr const uint32_t PWM_CHANNEL_COUNT_S = 0;

        constexpr const uint32_t PWM_CHANNEL_CMPA_M = 0x0000FFFF;  // Comparator A Value
        constexpr const uint32_t PWM_CHANNEL_CMPA_S = 0;

        constexpr const uint32_t PWM_CHANNEL_CMPB_M = 0x0000FFFF;  // Comparator B Value
        constexpr const uint32_t PWM_CHANNEL_CMPB_S = 0;

        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBD_M = 0x00000C00;  // Action for Comparator B Down
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBD_INV = 0x00000400;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBD_ZERO = 0x00000800;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBD_ONE = 0x00000C00;  // Drive pwmA High
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBU_M = 0x00000300;  // Action for Comparator B Up
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBU_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBU_INV = 0x00000100;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBU_ZERO = 0x00000200;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPBU_ONE = 0x00000300;  // Drive pwmA High
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAD_M = 0x000000C0;  // Action for Comparator A Down
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAD_INV = 0x00000040;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAD_ZERO = 0x00000080;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAD_ONE = 0x000000C0;  // Drive pwmA High
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAU_M = 0x00000030;  // Action for Comparator A Up
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAU_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAU_INV = 0x00000010;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAU_ZERO = 0x00000020;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTCMPAU_ONE = 0x00000030;  // Drive pwmA High
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTLOAD_M = 0x0000000C;  // Action for Counter=LOAD
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTLOAD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTLOAD_INV = 0x00000004;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTLOAD_ZERO = 0x00000008;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTLOAD_ONE = 0x0000000C;  // Drive pwmA High
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTZERO_M = 0x00000003;  // Action for Counter=0
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTZERO_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTZERO_INV = 0x00000001;  // Invert pwmA
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTZERO_ZERO = 0x00000002;  // Drive pwmA Low
        constexpr const uint32_t PWM_CHANNEL_GENA_ACTZERO_ONE = 0x00000003;  // Drive pwmA High

        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBD_M = 0x00000C00;  // Action for Comparator B Down
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBD_INV = 0x00000400;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBD_ZERO = 0x00000800;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBD_ONE = 0x00000C00;  // Drive pwmB High
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBU_M = 0x00000300;  // Action for Comparator B Up
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBU_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBU_INV = 0x00000100;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBU_ZERO = 0x00000200;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPBU_ONE = 0x00000300;  // Drive pwmB High
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAD_M = 0x000000C0;  // Action for Comparator A Down
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAD_INV = 0x00000040;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAD_ZERO = 0x00000080;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAD_ONE = 0x000000C0;  // Drive pwmB High
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAU_M = 0x00000030;  // Action for Comparator A Up
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAU_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAU_INV = 0x00000010;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAU_ZERO  =  0x00000020;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTCMPAU_ONE = 0x00000030;  // Drive pwmB High
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTLOAD_M = 0x0000000C;  // Action for Counter=LOAD
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTLOAD_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTLOAD_INV = 0x00000004;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTLOAD_ZERO = 0x00000008;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTLOAD_ONE = 0x0000000C;  // Drive pwmB High
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTZERO_M = 0x00000003;  // Action for Counter=0
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTZERO_NONE = 0x00000000;  // Do nothing
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTZERO_INV = 0x00000001;  // Invert pwmB
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTZERO_ZERO = 0x00000002;  // Drive pwmB Low
        constexpr const uint32_t PWM_CHANNEL_GENB_ACTZERO_ONE = 0x00000003;  // Drive pwmB High

        constexpr const uint32_t PWM_CHANNEL_DBCTL_ENABLE = 0x00000001;  // Dead-Band Generator Enable

        constexpr const uint32_t PWM_CHANNEL_DBRISE_DELAY_M = 0x00000FFF;  // Dead-Band Rise Delay
        constexpr const uint32_t PWM_CHANNEL_DBRISE_DELAY_S = 0;

        constexpr const uint32_t PWM_CHANNEL_DBFALL_DELAY_M = 0x00000FFF;  // Dead-Band Fall Delay
        constexpr const uint32_t PWM_CHANNEL_DBFALL_DELAY_S = 0;

        constexpr const uint32_t PWM_CHANNEL_FLTSRC0_FAULT3 = 0x00000008;  // Fault3 Input
        constexpr const uint32_t PWM_CHANNEL_FLTSRC0_FAULT2 = 0x00000004;  // Fault2 Input
        constexpr const uint32_t PWM_CHANNEL_FLTSRC0_FAULT1 = 0x00000002;  // Fault1 Input
        constexpr const uint32_t PWM_CHANNEL_FLTSRC0_FAULT0 = 0x00000001;  // Fault0 Input

        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP7 = 0x00000080;  // Digital Comparator 7
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP6 = 0x00000040;  // Digital Comparator 6
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP5 = 0x00000020;  // Digital Comparator 5
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP4 = 0x00000010;  // Digital Comparator 4
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP3 = 0x00000008;  // Digital Comparator 3
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP2 = 0x00000004;  // Digital Comparator 2
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP1 = 0x00000002;  // Digital Comparator 1
        constexpr const uint32_t PWM_CHANNEL_FLTSRC1_DCMP0 = 0x00000001;  // Digital Comparator 0

        constexpr const uint32_t PWM_CHANNEL_MINFLTPER_M = 0x0000FFFF;  // Minimum Fault Period
        constexpr const uint32_t PWM_CHANNEL_MINFLTPER_S = 0;

        constexpr const uint32_t SYSCTL_RCC_USEPWMDIV = 0x00100000;  // Enable PWM Clock Divisor
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_M = 0x000E0000;  // PWM Unit Clock Divisor
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_2 = 0x00000000;  // PWM clock /2
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_4 = 0x00020000;  // PWM clock /4
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_8 = 0x00040000;  // PWM clock /8
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_16 = 0x00060000;  // PWM clock /16
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_32 = 0x00080000;  // PWM clock /32
        constexpr const uint32_t SYSCTL_RCC_PWMDIV_64 = 0x000A0000;  // PWM clock /64

        constexpr uint32_t ToRccPwmDiv(uint32_t multiplyFactor)
        {
            really_assert(multiplyFactor >= 2 && multiplyFactor <= 64 && multiplyFactor % 2 == 0);

            switch (multiplyFactor)
            {
                case 2: return SYSCTL_RCC_PWMDIV_2; break;
                case 4: return SYSCTL_RCC_PWMDIV_4; break;
                case 8: return SYSCTL_RCC_PWMDIV_8; break;
                case 16: return SYSCTL_RCC_PWMDIV_16; break;
                case 32: return SYSCTL_RCC_PWMDIV_32; break;
                case 64: return SYSCTL_RCC_PWMDIV_64; break;
                default: std::abort();
            }
        }

        constexpr std::array<uint32_t, 2> peripheralPwmArray =
        {{
            PWM0_BASE,
            PWM1_BASE,
        }};

        const infra::MemoryRange<PWM0_Type* const> peripheralPwm = infra::ReinterpretCastMemoryRange<PWM0_Type* const>(infra::MakeRange(peripheralPwmArray));

        extern "C" uint32_t SystemCoreClock;
    }

    Pwm::Pwm(uint8_t aPwmIndex, PinChannel channel0, const Config& config)
        : Pwm(aPwmIndex, channel0, PinChannel(), PinChannel(), PinChannel(), config)
    { }

    Pwm::Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, const Config& config)
        : Pwm(aPwmIndex, channel0, channel1, PinChannel(), PinChannel(), config)
    { }

    Pwm::Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, PinChannel channel2, const Config& config)
        : Pwm(aPwmIndex, channel0, channel1, channel2, PinChannel(), config)
    { }

    Pwm::Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, PinChannel channel2, PinChannel channel3, const Config& config)
        : pwmIndex(aPwmIndex)
        , config(config)
    {
        channels.emplace_back(channel0, PinConfigPeripheral::pwmChannel0, PinConfigPeripheral::pwmChannel1, peripheralPwmArray[pwmIndex], 0);
        channels.emplace_back(channel1, PinConfigPeripheral::pwmChannel2, PinConfigPeripheral::pwmChannel3, peripheralPwmArray[pwmIndex], 1);
        channels.emplace_back(channel2, PinConfigPeripheral::pwmChannel4, PinConfigPeripheral::pwmChannel5, peripheralPwmArray[pwmIndex], 2);
        channels.emplace_back(channel3, PinConfigPeripheral::pwmChannel6, PinConfigPeripheral::pwmChannel6, peripheralPwmArray[pwmIndex], 3);

        EnableClock();
    }

    Pwm::~Pwm()
    {
        for (auto& channel : channels)
        {
            channel.address->CTL = 0;
        }

        peripheralPwm[pwmIndex]->ENABLE = 0;

        DisableClock();
    }

    void Pwm::SetBaseFrequency(hal::Hertz baseFrequency)
    {
        auto load = (peripheralFrequency / baseFrequency.Value()) - 1;
        really_assert((load & 0xffff) == 0);

        this->baseFrequency = baseFrequency;
    }

    void Pwm::Start(hal::Percent globalDutyCycle)
    {
        really_assert(globalDutyCycle.Value() < 100);

        Reset();
        MasterControl();

        for (auto& channel : channels)
        {
            channel.address->CTL &= ~PWM_CHANNEL_CTL_ENABLE;

            Configure(channel);

            auto comparator = channel.address->LOAD * globalDutyCycle.Value() / 100;

            if (channel.a)
                channel.address->CMPA = comparator;
            if (channel.b)
                channel.address->CMPB = comparator;

            channel.address->CTL |= PWM_CHANNEL_CTL_ENABLE;
        }
    }

    void Pwm::Stop()
    {
        for (auto& channel : channels)
        {
            channel.address->CTL &= ~PWM_CHANNEL_CTL_ENABLE;
        }
    }

    void Pwm::HandleInterrupt()
    {

    }

    void Pwm::Configure(const PeripheralChannels& channel)
    {
        if (channel.a || channel.b)
        {
            auto load = (peripheralFrequency / baseFrequency.Value()) - 1;
            really_assert((load & 0xffff) == 0);

            channel.address->CTL &= ~PWM_CHANNEL_CTL_ENABLE;

            channel.address->DBCTL |= PWM_CHANNEL_DBCTL_ENABLE;
            channel.address->DBFALL = 0;
            channel.address->DBRISE = 0;

            channel.address->LOAD = load & 0xffff;

            channel.address->CTL |= config.control.Value();
            channel.address->GENA = config.generatorA.Value();
            channel.address->GENB = config.generatorB.Value();
            channel.address->DBFALL = config.deadTime.fall;
            channel.address->DBRISE = config.deadTime.rise;

            peripheralPwm[pwmIndex]->ENABLE |= channel.enable;
        }
    }

    void Pwm::Reset()
    {
        SYSCTL->SRPWM |= 1 << pwmIndex;
        while (!infra::IsBitSet(SYSCTL->PRPWM, pwmIndex))
        {
        }
    }

    void Pwm::MasterControl()
    {
        pwmArray[pwmIndex]->CTL = PWM_CTL_GLOBALSYNC3 | PWM_CTL_GLOBALSYNC2 | PWM_CTL_GLOBALSYNC1 | PWM_CTL_GLOBALSYNC0;
        pwmArray[pwmIndex]->SYNC = PWM_SYNC_SYNC3 | PWM_SYNC_SYNC2 | PWM_SYNC_SYNC1 | PWM_SYNC_SYNC0;
        //pwmArray[pwmIndex]->ENUPD = ;
    }

    void Pwm::SetClockPrescaler(uint32_t frequency)
    {
        uint32_t divisor = 1;
        peripheralFrequency = SystemCoreClock;
        really_assert(frequency > (SystemCoreClock / 2));

        SYSCTL->RCC = (SYSCTL->RCC & ~(SYSCTL_RCC_PWMDIV_M | SYSCTL_RCC_USEPWMDIV));

        bool needToFixClock = frequency < (peripheralFrequency / 0xffff);

        while (needToFixClock)
        {
            divisor *= 2;
            peripheralFrequency /= divisor;

            really_assert(divisor <= 64);

            if (frequency < (peripheralFrequency / 0xffff))
                break;
        }

        if (needToFixClock)
            SYSCTL->RCC |= SYSCTL_RCC_USEPWMDIV | ToRccPwmDiv(divisor);
    }

    void Pwm::EnableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCPWM, true, pwmIndex);

        while (!infra::IsBitSet(SYSCTL->PRPWM, pwmIndex))
        {
        }
    }

    void Pwm::DisableClock()
    {
        infra::ReplaceBit(SYSCTL->RCGCPWM, false, pwmIndex);
    }
}
