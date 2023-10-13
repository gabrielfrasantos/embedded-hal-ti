#include "hal_tiva/tiva/Ethernet.hpp"
#include "TM4C1294NCPDT.h"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"
#include "infra/util/ReallyAssert.hpp"
#include "system_TM4C129.h"
#include <array>
#include <stdint.h>

namespace
{
#if defined(ewarm) || defined(DOXYGEN)
    extern "C" void Delay(uint32_t count)
    {
        __asm("    subs    r0, #1\n"
            "    bne.n   Delay\n"
            "    bx      lr");
    }
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx) || defined(__GNUC__) || defined(__GNUG__)
    extern "C" void __attribute__((naked)) Delay(uint32_t count)
    {
        __asm("    subs    r0, #1\n"
            "    bne     Delay\n"
            "    bx      lr");
    }
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
    extern "C" __asm void Delay(uint32_t count)
    {
        subs    r0, #1;
        bne     Delay;
        bx      lr;
    }
#endif

    constexpr uint32_t EMAC_PHY_TYPE_INTERNAL = 0x00000000;
    constexpr uint32_t EMAC_PHY_TYPE_EXTERNAL_MII = 0x80000000;
    constexpr uint32_t EMAC_PHY_TYPE_EXTERNAL_RMII = 0xC0000000;
    constexpr uint32_t EMAC_PHY_INT_NIB_TXERR_DET_DIS = 0x01000000;
    constexpr uint32_t EMAC_PHY_INT_RX_ER_DURING_IDLE = 0x00800000;
    constexpr uint32_t EMAC_PHY_INT_ISOLATE_MII_LLOSS = 0x00400000;
    constexpr uint32_t EMAC_PHY_INT_LINK_LOSS_RECOVERY = 0x00200000;
    constexpr uint32_t EMAC_PHY_INT_TDRRUN = 0x00100000;
    constexpr uint32_t EMAC_PHY_INT_LD_ON_RX_ERR_COUNT = 0x00040000;
    constexpr uint32_t EMAC_PHY_INT_LD_ON_MTL3_ERR_COUNT = 0x00020000;
    constexpr uint32_t EMAC_PHY_INT_LD_ON_LOW_SNR = 0x00010000;
    constexpr uint32_t EMAC_PHY_INT_LD_ON_SIGNAL_ENERGY = 0x00008000;
    constexpr uint32_t EMAC_PHY_INT_POLARITY_SWAP = 0x00004000;
    constexpr uint32_t EMAC_PHY_INT_MDI_SWAP = 0x00002000;
    constexpr uint32_t EMAC_PHY_INT_ROBUST_MDIX = 0x00001000;
    constexpr uint32_t EMAC_PHY_INT_FAST_MDIX = 0x00000800;
    constexpr uint32_t EMAC_PHY_INT_MDIX_EN = 0x00000400;
    constexpr uint32_t EMAC_PHY_INT_FAST_RXDV_DETECT = 0x00000200;
    constexpr uint32_t EMAC_PHY_INT_FAST_L_UP_DETECT = 0x00000100;
    constexpr uint32_t EMAC_PHY_INT_EXT_FULL_DUPLEX = 0x00000080;
    constexpr uint32_t EMAC_PHY_INT_FAST_AN_80_50_35 = 0x00000040;
    constexpr uint32_t EMAC_PHY_INT_FAST_AN_120_75_50 = 0x00000050;
    constexpr uint32_t EMAC_PHY_INT_FAST_AN_140_150_100 = 0x00000060;
    constexpr uint32_t EMAC_PHY_FORCE_10B_T_HALF_DUPLEX = 0x00000000;
    constexpr uint32_t EMAC_PHY_FORCE_10B_T_FULL_DUPLEX = 0x00000002;
    constexpr uint32_t EMAC_PHY_FORCE_100B_T_HALF_DUPLEX = 0x00000004;
    constexpr uint32_t EMAC_PHY_FORCE_100B_T_FULL_DUPLEX = 0x00000006;
    constexpr uint32_t EMAC_PHY_AN_10B_T_HALF_DUPLEX = 0x00000008;
    constexpr uint32_t EMAC_PHY_AN_10B_T_FULL_DUPLEX = 0x0000000A;
    constexpr uint32_t EMAC_PHY_AN_100B_T_HALF_DUPLEX = 0x0000000C;
    constexpr uint32_t EMAC_PHY_AN_100B_T_FULL_DUPLEX = 0x0000000E;
    constexpr uint32_t EMAC_PHY_INT_HOLD = 0x00000001;

    constexpr uint32_t EMAC_PHY_TYPE_MASK = 0xC0000000;

    constexpr uint32_t EMAC_CC_PTPCEN = 0x00040000;  // PTP Clock Reference Enable
    constexpr uint32_t EMAC_CC_POL = 0x00020000;  // LED Polarity Control
    constexpr uint32_t EMAC_CC_CLKEN = 0x00010000;  // EN0RREF_CLK Signal Enable

    constexpr uint32_t EMAC_DMABUSMOD_RIB = 0x80000000;  // Rebuild Burst
    constexpr uint32_t EMAC_DMABUSMOD_TXPR = 0x08000000;  // Transmit Priority
    constexpr uint32_t EMAC_DMABUSMOD_MB = 0x04000000;  // Mixed Burst
    constexpr uint32_t EMAC_DMABUSMOD_AAL = 0x02000000;  // Address Aligned Beats
    constexpr uint32_t EMAC_DMABUSMOD_8XPBL = 0x01000000;  // 8 x Programmable Burst Length (PBL) Mode
    constexpr uint32_t EMAC_DMABUSMOD_USP = 0x00800000;  // Use Separate Programmable Burst Length (PBL)
    constexpr uint32_t EMAC_DMABUSMOD_RPBL_M = 0x007E0000;  // RX DMA Programmable Burst Length (PBL)
    constexpr uint32_t EMAC_DMABUSMOD_FB = 0x00010000;  // Fixed Burst
    constexpr uint32_t EMAC_DMABUSMOD_PR_M = 0x0000C000;  // Priority Ratio
    constexpr uint32_t EMAC_DMABUSMOD_PBL_M = 0x00003F00;  // Programmable Burst Length
    constexpr uint32_t EMAC_DMABUSMOD_ATDS = 0x00000080;  // Alternate Descriptor Size
    constexpr uint32_t EMAC_DMABUSMOD_DSL_M = 0x0000007C;  // Descriptor Skip Length
    constexpr uint32_t EMAC_DMABUSMOD_DA = 0x00000002;  // DMA Arbitration Scheme
    constexpr uint32_t EMAC_DMABUSMOD_SWR = 0x00000001;  // DMA Software Reset
    constexpr uint32_t EMAC_DMABUSMOD_RPBL_S = 17;
    constexpr uint32_t EMAC_DMABUSMOD_PR_S = 14;
    constexpr uint32_t EMAC_DMABUSMOD_PBL_S = 8;
    constexpr uint32_t EMAC_DMABUSMOD_DSL_S = 2;

    constexpr uint32_t EMAC_MIIADDR_PLA_M = 0x0000F800;  // Physical Layer Address
    constexpr uint32_t EMAC_MIIADDR_MII_M = 0x000007C0;  // MII Register
    constexpr uint32_t EMAC_MIIADDR_CR_M = 0x0000003C;  // Clock Reference Frequency Selection
    constexpr uint32_t EMAC_MIIADDR_CR_60_100 = 0x00000000;  // The frequency of the System Clock is 60 to 100 MHz providing a MDIO clock of SYSCLK/42
    constexpr uint32_t EMAC_MIIADDR_CR_100_150 = 0x00000004;  // The frequency of the System Clock is 100 to 150 MHz providing a MDIO clock of SYSCLK/62
    constexpr uint32_t EMAC_MIIADDR_CR_20_35 = 0x00000008;  // The frequency of the System Clock is 20-35 MHz providing a MDIO clock of System Clock/16
    constexpr uint32_t EMAC_MIIADDR_CR_35_60 = 0x0000000C;  // The frequency of the System Clock is 35 to 60 MHz providing a MDIO clock of System Clock/26
    constexpr uint32_t EMAC_MIIADDR_MIIW = 0x00000002;  // MII Write
    constexpr uint32_t EMAC_MIIADDR_MIIB = 0x00000001;  // MII Busy
    constexpr uint32_t EMAC_MIIADDR_PLA_S = 11;
    constexpr uint32_t EMAC_MIIADDR_MII_S = 6;

    constexpr uint32_t EMAC_CFG_TWOKPEN = 0x08000000;  // IEEE 802
    constexpr uint32_t EMAC_CFG_CST = 0x02000000;  // CRC Stripping for Type Frames
    constexpr uint32_t EMAC_CFG_WDDIS = 0x00800000;  // Watchdog Disable
    constexpr uint32_t EMAC_CFG_JD = 0x00400000;  // Jabber Disable
    constexpr uint32_t EMAC_CFG_JFEN = 0x00100000;  // Jumbo Frame Enable
    constexpr uint32_t EMAC_CFG_IFG_M = 0x000E0000;  // Inter-Frame Gap (IFG)
    constexpr uint32_t EMAC_CFG_IFG_96 = 0x00000000;  // 96 bit times
    constexpr uint32_t EMAC_CFG_IFG_88 = 0x00020000;  // 88 bit times
    constexpr uint32_t EMAC_CFG_IFG_80 = 0x00040000;  // 80 bit times
    constexpr uint32_t EMAC_CFG_IFG_72 = 0x00060000;  // 72 bit times
    constexpr uint32_t EMAC_CFG_IFG_64 = 0x00080000;  // 64 bit times
    constexpr uint32_t EMAC_CFG_IFG_56 = 0x000A0000;  // 56 bit times
    constexpr uint32_t EMAC_CFG_IFG_48 = 0x000C0000;  // 48 bit times
    constexpr uint32_t EMAC_CFG_IFG_40 = 0x000E0000;  // 40 bit times
    constexpr uint32_t EMAC_CFG_DISCRS = 0x00010000;  // Disable Carrier Sense During Transmission
    constexpr uint32_t EMAC_CFG_PS = 0x00008000;  // Port Select
    constexpr uint32_t EMAC_CFG_FES = 0x00004000;  // Speed
    constexpr uint32_t EMAC_CFG_DRO = 0x00002000;  // Disable Receive Own
    constexpr uint32_t EMAC_CFG_LOOPBM = 0x00001000;  // Loopback Mode
    constexpr uint32_t EMAC_CFG_DUPM = 0x00000800;  // Duplex Mode
    constexpr uint32_t EMAC_CFG_IPC = 0x00000400;  // Checksum Offload
    constexpr uint32_t EMAC_CFG_DR = 0x00000200;  // Disable Retry
    constexpr uint32_t EMAC_CFG_ACS = 0x00000080;  // Automatic Pad or CRC Stripping
    constexpr uint32_t EMAC_CFG_BL_M = 0x00000060;  // Back-Off Limit
    constexpr uint32_t EMAC_CFG_BL_1024 = 0x00000000;  // k = min (n,10)
    constexpr uint32_t EMAC_CFG_BL_256 = 0x00000020;  // k = min (n,8)
    constexpr uint32_t EMAC_CFG_BL_8 = 0x00000040;  // k = min (n,4)
    constexpr uint32_t EMAC_CFG_BL_2 = 0x00000060;  // k = min (n,1)
    constexpr uint32_t EMAC_CFG_DC = 0x00000010;  // Deferral Check
    constexpr uint32_t EMAC_CFG_TE = 0x00000008;  // Transmitter Enable
    constexpr uint32_t EMAC_CFG_RE = 0x00000004;  // Receiver Enable
    constexpr uint32_t EMAC_CFG_PRELEN_M = 0x00000003;  // Preamble Length for Transmit Frames
    constexpr uint32_t EMAC_CFG_PRELEN_7 = 0x00000000;  // 7 bytes of preamble
    constexpr uint32_t EMAC_CFG_PRELEN_5 = 0x00000001;  // 5 bytes of preamble
    constexpr uint32_t EMAC_CFG_PRELEN_3 = 0x00000002;  // 3 bytes of preamble

    constexpr uint32_t EMAC_WDOGTO_PWE = 0x00010000;  // Programmable Watchdog Enable
    constexpr uint32_t EMAC_WDOGTO_WTO_M = 0x00003FFF;  // Watchdog Timeout
    constexpr uint32_t EMAC_WDOGTO_WTO_S = 0;

    constexpr uint32_t EMAC_MIIDATA_DATA_M = 0x0000FFFF;  // MII Data
    constexpr uint32_t EMAC_MIIDATA_DATA_S = 0;

    constexpr uint32_t EPHY_BMCR = 0x00000000;  // Ethernet PHY Basic Mode Control
    constexpr uint32_t EPHY_BMSR = 0x00000001;  // Ethernet PHY Basic Mode Status
    constexpr uint32_t EPHY_ID1 = 0x00000002;  // Ethernet PHY Identifier Register 1
    constexpr uint32_t EPHY_ID2 = 0x00000003;  // Ethernet PHY Identifier Register 2
    constexpr uint32_t EPHY_ANA = 0x00000004;  // Ethernet PHY Auto-Negotiation Advertisement
    constexpr uint32_t EPHY_ANLPA = 0x00000005;  // Ethernet PHY Auto-Negotiation Link Partner Ability
    constexpr uint32_t EPHY_ANER = 0x00000006;  // Ethernet PHY Auto-Negotiation Expansion
    constexpr uint32_t EPHY_ANNPTR = 0x00000007;  // Ethernet PHY Auto-Negotiation Next Page TX
    constexpr uint32_t EPHY_ANLNPTR = 0x00000008;  // Ethernet PHY Auto-Negotiation Link Partner Ability Next Page
    constexpr uint32_t EPHY_CFG1 = 0x00000009;  // Ethernet PHY Configuration 1
    constexpr uint32_t EPHY_CFG2 = 0x0000000A;  // Ethernet PHY Configuration 2
    constexpr uint32_t EPHY_CFG3 = 0x0000000B;  // Ethernet PHY Configuration 3
    constexpr uint32_t EPHY_REGCTL = 0x0000000D;  // Ethernet PHY Register Control
    constexpr uint32_t EPHY_ADDAR = 0x0000000E;  // Ethernet PHY Address or Data
    constexpr uint32_t EPHY_STS = 0x00000010;  // Ethernet PHY Status
    constexpr uint32_t EPHY_SCR = 0x00000011;  // Ethernet PHY Specific Control
    constexpr uint32_t EPHY_MISR1 = 0x00000012;  // Ethernet PHY MII Interrupt Status 1
    constexpr uint32_t EPHY_MISR2 = 0x00000013;  // Ethernet PHY MII Interrupt Status 2
    constexpr uint32_t EPHY_FCSCR = 0x00000014;  // Ethernet PHY False Carrier Sense Counter
    constexpr uint32_t EPHY_RXERCNT = 0x00000015;  // Ethernet PHY Receive Error Count
    constexpr uint32_t EPHY_BISTCR = 0x00000016;  // Ethernet PHY BIST Control
    constexpr uint32_t EPHY_LEDCR = 0x00000018;  // Ethernet PHY LED Control
    constexpr uint32_t EPHY_CTL = 0x00000019;  // Ethernet PHY Control
    constexpr uint32_t EPHY_10BTSC = 0x0000001A;  // Ethernet PHY 10Base-T Status/Control - MR26
    constexpr uint32_t EPHY_BICSR1 = 0x0000001B;  // Ethernet PHY BIST Control and Status 1
    constexpr uint32_t EPHY_BICSR2 = 0x0000001C;  // Ethernet PHY BIST Control and Status 2
    constexpr uint32_t EPHY_CDCR = 0x0000001E;  // Ethernet PHY Cable Diagnostic Control
    constexpr uint32_t EPHY_RCR = 0x0000001F;  // Ethernet PHY Reset Control
    constexpr uint32_t EPHY_LEDCFG = 0x00000025;  // Ethernet PHY LED Configuration

    constexpr uint32_t EPHY_MISR1_LINKSTAT = 0x00002000;  // Change of Link Status Interrupt
    constexpr uint32_t EPHY_MISR1_SPEED = 0x00001000;  // Change of Speed Status Interrupt
    constexpr uint32_t EPHY_MISR1_DUPLEXM = 0x00000800;  // Change of Duplex Status Interrupt
    constexpr uint32_t EPHY_MISR1_ANC = 0x00000400;  // Auto-Negotiation Complete Interrupt
    constexpr uint32_t EPHY_MISR1_FCHF = 0x00000200;  // False Carrier Counter Half-Full Interrupt
    constexpr uint32_t EPHY_MISR1_RXHF = 0x00000100;  // Receive Error Counter Half-Full Interrupt
    constexpr uint32_t EPHY_MISR1_LINKSTATEN = 0x00000020;  // Link Status Interrupt Enable
    constexpr uint32_t EPHY_MISR1_SPEEDEN = 0x00000010;  // Speed Change Interrupt Enable
    constexpr uint32_t EPHY_MISR1_DUPLEXMEN = 0x00000008;  // Duplex Status Interrupt Enable
    constexpr uint32_t EPHY_MISR1_ANCEN = 0x00000004;  // Auto-Negotiation Complete Interrupt Enable
    constexpr uint32_t EPHY_MISR1_FCHFEN = 0x00000002;  // False Carrier Counter Register half-full Interrupt Enable
    constexpr uint32_t EPHY_MISR1_RXHFEN = 0x00000001;  // Receive Error Counter Register Half-Full Event Interrupt

    constexpr uint32_t EMAC_SUBSECINC_SSINC_M = 0x000000FF;  // Sub-second Increment Value
    constexpr uint32_t EMAC_SUBSECINC_SSINC_S = 0;

    constexpr uint32_t EMAC_TIMSTCTRL_PTPFLTR = 0x00040000;  // Enable MAC address for PTP Frame Filtering
    constexpr uint32_t EMAC_TIMSTCTRL_SELPTP_M = 0x00030000;  // Select PTP packets for Taking Snapshots
    constexpr uint32_t EMAC_TIMSTCTRL_TSMAST = 0x00008000;  // Enable Snapshot for Messages Relevant to Master
    constexpr uint32_t EMAC_TIMSTCTRL_TSEVNT = 0x00004000;  // Enable Timestamp Snapshot for Event Messages
    constexpr uint32_t EMAC_TIMSTCTRL_PTPIPV4 = 0x00002000;  // Enable Processing of PTP Frames Sent over IPv4-UDP
    constexpr uint32_t EMAC_TIMSTCTRL_PTPIPV6 = 0x00001000;  // Enable Processing of PTP Frames Sent Over IPv6-UDP
    constexpr uint32_t EMAC_TIMSTCTRL_PTPETH = 0x00000800;  // Enable Processing of PTP Over Ethernet Frames
    constexpr uint32_t EMAC_TIMSTCTRL_PTPVER2 = 0x00000400;  // Enable PTP Packet Processing For Version 2 Format
    constexpr uint32_t EMAC_TIMSTCTRL_DGTLBIN = 0x00000200;  // Timestamp Digital or Binary Rollover Control
    constexpr uint32_t EMAC_TIMSTCTRL_ALLF = 0x00000100;  // Enable Timestamp For All Frames
    constexpr uint32_t EMAC_TIMSTCTRL_ADDREGUP = 0x00000020;  // Addend Register Update
    constexpr uint32_t EMAC_TIMSTCTRL_INTTRIG = 0x00000010;  // Timestamp Interrupt Trigger Enable
    constexpr uint32_t EMAC_TIMSTCTRL_TSUPDT = 0x00000008;  // Timestamp Update
    constexpr uint32_t EMAC_TIMSTCTRL_TSINIT = 0x00000004;  // Timestamp Initialize
    constexpr uint32_t EMAC_TIMSTCTRL_TSFCUPDT = 0x00000002;  // Timestamp Fine or Coarse Update
    constexpr uint32_t EMAC_TIMSTCTRL_TSEN = 0x00000001;  // Timestamp Enable
    constexpr uint32_t EMAC_TIMSTCTRL_SELPTP_S = 16;

    constexpr uint32_t EMAC_DMARIS_LPI = 0x40000000;  // LPI Trigger Interrupt Status
    constexpr uint32_t EMAC_DMARIS_TT = 0x20000000;  // Timestamp Trigger Interrupt Status
    constexpr uint32_t EMAC_DMARIS_PMT = 0x10000000;  // MAC PMT Interrupt Status
    constexpr uint32_t EMAC_DMARIS_MMC = 0x08000000;  // MAC MMC Interrupt
    constexpr uint32_t EMAC_DMARIS_AE_M = 0x03800000;  // Access Error
    constexpr uint32_t EMAC_DMARIS_AE_RXDMAWD = 0x00000000;  // Error during RX DMA Write Data Transfer
    constexpr uint32_t EMAC_DMARIS_AE_TXDMARD = 0x01800000;  // Error during TX DMA Read Data Transfer
    constexpr uint32_t EMAC_DMARIS_AE_RXDMADW = 0x02000000;  // Error during RX DMA Descriptor Write Access
    constexpr uint32_t EMAC_DMARIS_AE_TXDMADW = 0x02800000;  // Error during TX DMA Descriptor Write Access
    constexpr uint32_t EMAC_DMARIS_AE_RXDMADR = 0x03000000;  // Error during RX DMA Descriptor Read Access
    constexpr uint32_t EMAC_DMARIS_AE_TXDMADR = 0x03800000;  // Error during TX DMA Descriptor Read Access
    constexpr uint32_t EMAC_DMARIS_TS_M = 0x00700000;  // Transmit Process State
    constexpr uint32_t EMAC_DMARIS_TS_STOP = 0x00000000;  // Stopped; Reset or Stop transmit command processed
    constexpr uint32_t EMAC_DMARIS_TS_RUNTXTD = 0x00100000;  // Running; Fetching transmit transfer descriptor
    constexpr uint32_t EMAC_DMARIS_TS_STATUS = 0x00200000;  // Running; Waiting for status
    constexpr uint32_t EMAC_DMARIS_TS_RUNTX = 0x00300000;  // Running; Reading data from host memory buffer and queuing it to transmit buffer (TX FIFO)
    constexpr uint32_t EMAC_DMARIS_TS_TSTAMP = 0x00400000;  // Writing Timestamp
    constexpr uint32_t EMAC_DMARIS_TS_SUSPEND = 0x00600000;  // Suspended; Transmit descriptor unavailable or transmit buffer underflow
    constexpr uint32_t EMAC_DMARIS_TS_RUNCTD = 0x00700000;  // Running; Closing transmit descriptor
    constexpr uint32_t EMAC_DMARIS_RS_M = 0x000E0000;  // Received Process State
    constexpr uint32_t EMAC_DMARIS_RS_STOP = 0x00000000;  // Stopped: Reset or stop receive command issued
    constexpr uint32_t EMAC_DMARIS_RS_RUNRXTD = 0x00020000;  // Running: Fetching receive transfer descriptor
    constexpr uint32_t EMAC_DMARIS_RS_RUNRXD = 0x00060000;  // Running: Waiting for receive packet
    constexpr uint32_t EMAC_DMARIS_RS_SUSPEND = 0x00080000;  // Suspended: Receive descriptor unavailable
    constexpr uint32_t EMAC_DMARIS_RS_RUNCRD = 0x000A0000;  // Running: Closing receive descriptor
    constexpr uint32_t EMAC_DMARIS_RS_TSWS = 0x000C0000;  // Writing Timestamp
    constexpr uint32_t EMAC_DMARIS_RS_RUNTXD = 0x000E0000;  // Running: Transferring the receive packet data from receive buffer to host memory
    constexpr uint32_t EMAC_DMARIS_NIS = 0x00010000;  // Normal Interrupt Summary
    constexpr uint32_t EMAC_DMARIS_AIS = 0x00008000;  // Abnormal Interrupt Summary
    constexpr uint32_t EMAC_DMARIS_ERI = 0x00004000;  // Early Receive Interrupt
    constexpr uint32_t EMAC_DMARIS_FBI = 0x00002000;  // Fatal Bus Error Interrupt
    constexpr uint32_t EMAC_DMARIS_ETI = 0x00000400;  // Early Transmit Interrupt
    constexpr uint32_t EMAC_DMARIS_RWT = 0x00000200;  // Receive Watchdog Timeout
    constexpr uint32_t EMAC_DMARIS_RPS = 0x00000100;  // Receive Process Stopped
    constexpr uint32_t EMAC_DMARIS_RU = 0x00000080;  // Receive Buffer Unavailable
    constexpr uint32_t EMAC_DMARIS_RI = 0x00000040;  // Receive Interrupt
    constexpr uint32_t EMAC_DMARIS_UNF = 0x00000020;  // Transmit Underflow
    constexpr uint32_t EMAC_DMARIS_OVF = 0x00000010;  // Receive Overflow
    constexpr uint32_t EMAC_DMARIS_TJT = 0x00000008;  // Transmit Jabber Timeout
    constexpr uint32_t EMAC_DMARIS_TU = 0x00000004;  // Transmit Buffer Unavailable
    constexpr uint32_t EMAC_DMARIS_TPS = 0x00000002;  // Transmit Process Stopped
    constexpr uint32_t EMAC_DMARIS_TI = 0x00000001;  // Transmit Interrupt

    constexpr uint32_t EPHY_BMCR_MIIRESET = 0x00008000;  // MII Register reset
    constexpr uint32_t EPHY_BMCR_MIILOOPBK = 0x00004000;  // MII Loopback
    constexpr uint32_t EPHY_BMCR_SPEED = 0x00002000;  // Speed Select
    constexpr uint32_t EPHY_BMCR_ANEN = 0x00001000;  // Auto-Negotiate Enable
    constexpr uint32_t EPHY_BMCR_PWRDWN = 0x00000800;  // Power Down
    constexpr uint32_t EPHY_BMCR_ISOLATE = 0x00000400;  // Port Isolate
    constexpr uint32_t EPHY_BMCR_RESTARTAN = 0x00000200;  // Restart Auto-Negotiation
    constexpr uint32_t EPHY_BMCR_DUPLEXM = 0x00000100;  // Duplex Mode
    constexpr uint32_t EPHY_BMCR_COLLTST = 0x00000080;  // Collision Test

    constexpr uint32_t EMAC_EPHYMISC_INT = 0x00000001;  // Ethernet PHY Status and Clear register

    constexpr uint32_t EMAC_DMAOPMODE_DT = 0x04000000;  // Disable Dropping of TCP/IP Checksum Error Frames
    constexpr uint32_t EMAC_DMAOPMODE_RSF = 0x02000000;  // Receive Store and Forward
    constexpr uint32_t EMAC_DMAOPMODE_DFF = 0x01000000;  // Disable Flushing of Received Frames
    constexpr uint32_t EMAC_DMAOPMODE_TSF = 0x00200000;  // Transmit Store and Forward
    constexpr uint32_t EMAC_DMAOPMODE_FTF = 0x00100000;  // Flush Transmit FIFO
    constexpr uint32_t EMAC_DMAOPMODE_TTC_M = 0x0001C000;  // Transmit Threshold Control
    constexpr uint32_t EMAC_DMAOPMODE_TTC_64 = 0x00000000;  // 64 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_128 = 0x00004000;  // 128 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_192 = 0x00008000;  // 192 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_256 = 0x0000C000;  // 256 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_40 = 0x00010000;  // 40 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_32 = 0x00014000;  // 32 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_24 = 0x00018000;  // 24 bytes
    constexpr uint32_t EMAC_DMAOPMODE_TTC_16 = 0x0001C000;  // 16 bytes
    constexpr uint32_t EMAC_DMAOPMODE_ST = 0x00002000;  // Start or Stop Transmission Command
    constexpr uint32_t EMAC_DMAOPMODE_FEF = 0x00000080;  // Forward Error Frames
    constexpr uint32_t EMAC_DMAOPMODE_FUF = 0x00000040;  // Forward Undersized Good Frames
    constexpr uint32_t EMAC_DMAOPMODE_DGF = 0x00000020;  // Drop Giant Frame Enable
    constexpr uint32_t EMAC_DMAOPMODE_RTC_M = 0x00000018;  // Receive Threshold Control
    constexpr uint32_t EMAC_DMAOPMODE_RTC_64 = 0x00000000;  // 64 bytes
    constexpr uint32_t EMAC_DMAOPMODE_RTC_32 = 0x00000008;  // 32 bytes
    constexpr uint32_t EMAC_DMAOPMODE_RTC_96 = 0x00000010;  // 96 bytes
    constexpr uint32_t EMAC_DMAOPMODE_RTC_128 = 0x00000018;  // 128 bytes
    constexpr uint32_t EMAC_DMAOPMODE_OSF = 0x00000004;  // Operate on Second Frame
    constexpr uint32_t EMAC_DMAOPMODE_SR = 0x00000002;  // Start or Stop Receive

    constexpr uint32_t EMAC_EPHYIM_INT = 0x00000001;  // Ethernet PHY Interrupt Mask

    constexpr uint32_t EMAC_PMTCTLSTAT_WUPFRRST = 0x80000000;  // Wake-Up Frame Filter Register Pointer Reset
    constexpr uint32_t EMAC_PMTCTLSTAT_RWKPTR_M = 0x07000000;  // Remote Wake-Up FIFO Pointer
    constexpr uint32_t EMAC_PMTCTLSTAT_GLBLUCAST = 0x00000200;  // Global Unicast
    constexpr uint32_t EMAC_PMTCTLSTAT_WUPRX = 0x00000040;  // Wake-Up Frame Received
    constexpr uint32_t EMAC_PMTCTLSTAT_MGKPRX = 0x00000020;  // Magic Packet Received
    constexpr uint32_t EMAC_PMTCTLSTAT_WUPFREN = 0x00000004;  // Wake-Up Frame Enable
    constexpr uint32_t EMAC_PMTCTLSTAT_MGKPKTEN = 0x00000002;  // Magic Packet Enable
    constexpr uint32_t EMAC_PMTCTLSTAT_PWRDWN = 0x00000001;  // Power Down
    constexpr uint32_t EMAC_PMTCTLSTAT_RWKPTR_S = 24;

    constexpr uint32_t EPHY_STS_MDIXM = 0x00004000;  // MDI-X Mode
    constexpr uint32_t EPHY_STS_RXLERR = 0x00002000;  // Receive Error Latch
    constexpr uint32_t EPHY_STS_POLSTAT = 0x00001000;  // Polarity Status
    constexpr uint32_t EPHY_STS_FCSL = 0x00000800;  // False Carrier Sense Latch
    constexpr uint32_t EPHY_STS_SD = 0x00000400;  // Signal Detect
    constexpr uint32_t EPHY_STS_DL = 0x00000200;  // Descrambler Lock
    constexpr uint32_t EPHY_STS_PAGERX = 0x00000100;  // Link Code Page Received
    constexpr uint32_t EPHY_STS_MIIREQ = 0x00000080;  // MII Interrupt Pending
    constexpr uint32_t EPHY_STS_RF = 0x00000040;  // Remote Fault
    constexpr uint32_t EPHY_STS_JD = 0x00000020;  // Jabber Detect
    constexpr uint32_t EPHY_STS_ANS = 0x00000010;  // Auto-Negotiation Status
    constexpr uint32_t EPHY_STS_MIILB = 0x00000008;  // MII Loopback Status
    constexpr uint32_t EPHY_STS_DUPLEX = 0x00000004;  // Duplex Status
    constexpr uint32_t EPHY_STS_SPEED = 0x00000002;  // Speed Status
    constexpr uint32_t EPHY_STS_LINK = 0x00000001;  // Link Status

    constexpr uint32_t EMAC_LPITIMERCTL_LST_M = 0x03FF0000;  // Low Power Idle LS Timer
    constexpr uint32_t EMAC_LPITIMERCTL_LST_S = 16;
    constexpr uint32_t EMAC_LPITIMERCTL_TWT_M = 0x0000FFFF;  // Low Power Idle TW Timer
    constexpr uint32_t EMAC_LPITIMERCTL_TWT_S = 0;

    constexpr uint32_t EMAC_LPICTLSTAT_LPITXA = 0x00080000;  // LPI TX Automate
    constexpr uint32_t EMAC_LPICTLSTAT_PLSEN = 0x00040000;  // PHY Link Status Enable
    constexpr uint32_t EMAC_LPICTLSTAT_PLS = 0x00020000;  // PHY Link Status
    constexpr uint32_t EMAC_LPICTLSTAT_LPIEN = 0x00010000;  // LPI Enable
    constexpr uint32_t EMAC_LPICTLSTAT_RLPIST = 0x00000200;  // Receive LPI State
    constexpr uint32_t EMAC_LPICTLSTAT_TLPIST = 0x00000100;  // Transmit LPI State
    constexpr uint32_t EMAC_LPICTLSTAT_RLPIEX = 0x00000008;  // Receive LPI Exit
    constexpr uint32_t EMAC_LPICTLSTAT_RLPIEN = 0x00000004;  // Receive LPI Entry
    constexpr uint32_t EMAC_LPICTLSTAT_TLPIEX = 0x00000002;  // Transmit LPI Exit
    constexpr uint32_t EMAC_LPICTLSTAT_TLPIEN = 0x00000001;  // Transmit LPI Entry

    constexpr uint32_t EMAC_BCONFIG_DMA_PRIO_WEIGHT_M = 0x30000000;
    constexpr uint32_t EMAC_BCONFIG_DMA_PRIO_WEIGHT_1 = 0x00000000;
    constexpr uint32_t EMAC_BCONFIG_DMA_PRIO_WEIGHT_2 = 0x10000000;
    constexpr uint32_t EMAC_BCONFIG_DMA_PRIO_WEIGHT_3 = 0x20000000;
    constexpr uint32_t EMAC_BCONFIG_DMA_PRIO_WEIGHT_4 = 0x30000000;
    constexpr uint32_t EMAC_BCONFIG_TX_PRIORITY = 0x08000000;
    constexpr uint32_t EMAC_BCONFIG_ADDR_ALIGNED = 0x02000000;
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_M = 0x0000C000;
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_1_1 = (0 << 14);
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_2_1 = (1 << 14);
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_3_1 = (2 << 14);
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_4_1 = (3 << 14);
    constexpr uint32_t EMAC_BCONFIG_PRIORITY_FIXED = 0x00000002;
    constexpr uint32_t EMAC_BCONFIG_FIXED_BURST = 0x00010000;
    constexpr uint32_t EMAC_BCONFIG_MIXED_BURST = 0x04000000;

    constexpr uint32_t EMAC_CONFIG_USE_MACADDR1 = 0x40000000;
    constexpr uint32_t EMAC_CONFIG_USE_MACADDR0 = 0x00000000;
    constexpr uint32_t EMAC_CONFIG_SA_FROM_DESCRIPTOR = 0x00000000;
    constexpr uint32_t EMAC_CONFIG_SA_INSERT = 0x20000000;
    constexpr uint32_t EMAC_CONFIG_SA_REPLACE = 0x30000000;
    constexpr uint32_t EMAC_CONFIG_2K_PACKETS = 0x08000000;
    constexpr uint32_t EMAC_CONFIG_STRIP_CRC = 0x02000000;
    constexpr uint32_t EMAC_CONFIG_JABBER_DISABLE = 0x00400000;
    constexpr uint32_t EMAC_CONFIG_JUMBO_ENABLE = 0x00100000;
    constexpr uint32_t EMAC_CONFIG_IF_GAP_MASK = 0x000E0000;
    constexpr uint32_t EMAC_CONFIG_IF_GAP_96BITS = (0x0 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_88BITS = (0x1 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_80BITS = (0x2 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_72BITS = (0x3 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_64BITS = (0x4 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_56BITS = (0x5 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_48BITS = (0x6 << 17);
    constexpr uint32_t EMAC_CONFIG_IF_GAP_40BITS = (0x7 << 17);
    constexpr uint32_t EMAC_CONFIG_CS_DISABLE = 0x00010000;
    constexpr uint32_t EMAC_CONFIG_100MBPS = 0x00004000;
    constexpr uint32_t EMAC_CONFIG_10MBPS = 0x00000000;
    constexpr uint32_t EMAC_CONFIG_RX_OWN_DISABLE = 0x00002000;
    constexpr uint32_t EMAC_CONFIG_LOOPBACK = 0x00001000;
    constexpr uint32_t EMAC_CONFIG_FULL_DUPLEX = 0x00000800;
    constexpr uint32_t EMAC_CONFIG_HALF_DUPLEX = 0x00000000;
    constexpr uint32_t EMAC_CONFIG_CHECKSUM_OFFLOAD = 0x00000400;
    constexpr uint32_t EMAC_CONFIG_RETRY_DISABLE = 0x00000200;
    constexpr uint32_t EMAC_CONFIG_AUTO_CRC_STRIPPING = 0x00000080;
    constexpr uint32_t EMAC_CONFIG_BO_MASK = 0x00000060;
    constexpr uint32_t EMAC_CONFIG_BO_LIMIT_1024 = (0x0 << 5);
    constexpr uint32_t EMAC_CONFIG_BO_LIMIT_256 = (0x1 << 5);
    constexpr uint32_t EMAC_CONFIG_BO_LIMIT_16 = (0x2 << 5);
    constexpr uint32_t EMAC_CONFIG_BO_LIMIT_2 = (0x3 << 5);
    constexpr uint32_t EMAC_CONFIG_DEFERRAL_CHK_ENABLE = 0x00000010;
    constexpr uint32_t EMAC_CONFIG_PREAMBLE_MASK = 0x00000003;
    constexpr uint32_t EMAC_CONFIG_7BYTE_PREAMBLE = 0x00000000;
    constexpr uint32_t EMAC_CONFIG_5BYTE_PREAMBLE = 0x00000001;
    constexpr uint32_t EMAC_CONFIG_3BYTE_PREAMBLE = 0x00000002;

    constexpr uint32_t EMAC_CONFIG_TX_ENABLED = 0x00000008;
    constexpr uint32_t EMAC_CONFIG_RX_ENABLED = 0x00000004;

    constexpr uint32_t EMAC_MODE_KEEP_BAD_CRC = 0x04000000;
    constexpr uint32_t EMAC_MODE_RX_STORE_FORWARD = 0x02000000;
    constexpr uint32_t EMAC_MODE_RX_FLUSH_DISABLE = 0x01000000;
    constexpr uint32_t EMAC_MODE_TX_STORE_FORWARD = 0x00200000;
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_16_BYTES = (7 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_24_BYTES = (6 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_32_BYTES = (5 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_40_BYTES = (4 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_64_BYTES = (0 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_128_BYTES = (1 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_192_BYTES = (2 << 14);
    constexpr uint32_t EMAC_MODE_TX_THRESHOLD_256_BYTES = (3 << 14);
    constexpr uint32_t EMAC_MODE_RX_ERROR_FRAMES = 0x00000080;
    constexpr uint32_t EMAC_MODE_RX_UNDERSIZED_FRAMES = 0x00000040;
    constexpr uint32_t EMAC_MODE_RX_THRESHOLD_64_BYTES = (0 << 3);
    constexpr uint32_t EMAC_MODE_RX_THRESHOLD_32_BYTES = (1 << 3);
    constexpr uint32_t EMAC_MODE_RX_THRESHOLD_96_BYTES = (2 << 3);
    constexpr uint32_t EMAC_MODE_RX_THRESHOLD_128_BYTES = 0x18;
    constexpr uint32_t EMAC_MODE_OPERATE_2ND_FRAME = 0x00000002;

    constexpr uint32_t DES0_TX_CTRL_OWN = 0x80000000;
    constexpr uint32_t DES0_TX_CTRL_INTERRUPT = 0x40000000;
    constexpr uint32_t DES0_TX_CTRL_LAST_SEG = 0x20000000;
    constexpr uint32_t DES0_TX_CTRL_FIRST_SEG = 0x10000000;

    constexpr uint32_t DES0_TX_CTRL_REPLACE_CRC = 0x01000000;
    constexpr uint32_t DES0_TX_CTRL_CHKSUM_M = 0x00C00000;
    constexpr uint32_t DES0_TX_CTRL_NO_CHKSUM = 0x00000000;
    constexpr uint32_t DES0_TX_CTRL_IP_HDR_CHKSUM = 0x00400000;
    constexpr uint32_t DES0_TX_CTRL_IP_HDR_PAY_CHKSUM = 0x00800000;
    constexpr uint32_t DES0_TX_CTRL_IP_ALL_CKHSUMS = 0x00C00000;
    constexpr uint32_t DES0_TX_CTRL_END_OF_RING = 0x00200000;
    constexpr uint32_t DES0_TX_CTRL_CHAINED = 0x00100000;
    constexpr uint32_t DES0_TX_CTRL_VLAN_M = 0x000C0000;
    constexpr uint32_t DES0_TX_CTRL_VLAN_NONE = 0x00000000;
    constexpr uint32_t DES0_TX_CTRL_VLAN_REMOVE = 0x00040000;
    constexpr uint32_t DES0_TX_CTRL_VLAN_INSERT = 0x00080000;
    constexpr uint32_t DES0_TX_CTRL_VLAN_REPLACE = 0x000C0000;
    constexpr uint32_t DES0_TX_STAT_TS_CAPTURED = 0x00020000;
    constexpr uint32_t DES0_TX_STAT_IPH_ERR = 0x00010000;
    constexpr uint32_t DES0_TX_STAT_ERR = 0x00008000;
    constexpr uint32_t DES0_TX_STAT_JABBER_TO = 0x00004000;
    constexpr uint32_t DES0_TX_STAT_FLUSHED = 0x00002000;
    constexpr uint32_t DES0_TX_STAT_PAYLOAD_ERR = 0x00001000;
    constexpr uint32_t DES0_TX_STAT_CARRIER_LOST = 0x00000800;
    constexpr uint32_t DES0_TX_STAT_NO_CARRIER = 0x00000400;
    constexpr uint32_t DES0_TX_STAT_TX_L_COLLISION = 0x00000200;
    constexpr uint32_t DES0_TX_STAT_E_COLLISION = 0x00000100;
    constexpr uint32_t DES0_TX_STAT_VLAN_FRAME = 0x00000080;
    constexpr uint32_t DES0_TX_STAT_COL_COUNT_M = 0x00000078;
    constexpr uint32_t DES0_TX_STAT_COL_COUNT_S = 3;
    constexpr uint32_t DES0_TX_STAT_E_DEFERRAL = 0x00000004;
    constexpr uint32_t DES0_TX_STAT_UNDERFLOW = 0x00000002;
    constexpr uint32_t DES0_TX_STAT_DEFERRED = 0x00000001;

    constexpr uint32_t DES0_RX_CTRL_OWN = 0x80000000;
    constexpr uint32_t DES0_RX_STAT_DEST_ADDR_FAIL = 0x40000000;
    constexpr uint32_t DES0_RX_STAT_FRAME_LENGTH_M = 0x3FFF0000;
    constexpr uint32_t DES0_RX_STAT_FRAME_LENGTH_S = 16;
    constexpr uint32_t DES0_RX_STAT_ERR = 0x00008000;
    constexpr uint32_t DES0_RX_STAT_DESCRIPTOR_ERR = 0x00004000;
    constexpr uint32_t DES0_RX_STAT_SRC_ADDR_FAIL = 0x00002000;
    constexpr uint32_t DES0_RX_STAT_LENGTH_ERR = 0x00001000;
    constexpr uint32_t DES0_RX_STAT_OVERFLOW = 0x00000800;
    constexpr uint32_t DES0_RX_STAT_VLAN_TAG = 0x00000400;
    constexpr uint32_t DES0_RX_STAT_FIRST_DESC = 0x00000200;
    constexpr uint32_t DES0_RX_STAT_LAST_DESC = 0x00000100;
    constexpr uint32_t DES0_RX_STAT_TS_AVAILABLE = 0x00000080;
    constexpr uint32_t DES0_RX_STAT_RX_L_COLLISION = 0x00000040;
    constexpr uint32_t DES0_RX_STAT_FRAME_TYPE = 0x00000020;
    constexpr uint32_t DES0_RX_STAT_WDOG_TIMEOUT = 0x00000010;
    constexpr uint32_t DES0_RX_STAT_RX_ERR = 0x00000008;
    constexpr uint32_t DES0_RX_STAT_DRIBBLE_ERR = 0x00000004;
    constexpr uint32_t DES0_RX_STAT_CRC_ERR = 0x00000002;
    constexpr uint32_t DES0_RX_STAT_MAC_ADDR = 0x00000001;
    constexpr uint32_t DES0_RX_STAT_EXT_AVAILABLE = 0x00000001;

    constexpr uint32_t DES1_RX_CTRL_DISABLE_INT = 0x80000000;
    constexpr uint32_t DES1_RX_CTRL_BUFF2_SIZE_M = 0x1FFF0000;
    constexpr uint32_t DES1_RX_CTRL_BUFF2_SIZE_S = 16;
    constexpr uint32_t DES1_RX_CTRL_END_OF_RING = 0x00008000;
    constexpr uint32_t DES1_RX_CTRL_CHAINED = 0x00004000;
    constexpr uint32_t DES1_RX_CTRL_BUFF1_SIZE_M = 0x00001FFF;
    constexpr uint32_t DES1_RX_CTRL_BUFF1_SIZE_S = 0;

    constexpr uint32_t EPHY_SCR_INPOL_EXT = 0x00000008;
    constexpr uint32_t EPHY_SCR_TINT_EXT = 0x00000004;
    constexpr uint32_t EPHY_SCR_INTEN_EXT = 0x00000002;
    constexpr uint32_t EPHY_SCR_INTOE_EXT = 0x00000001;

    constexpr uint32_t EMAC_FRMFILTER_RX_ALL = 0x80000000;
    constexpr uint32_t EMAC_FRMFILTER_VLAN = 0x00010000;
    constexpr uint32_t EMAC_FRMFILTER_HASH_AND_PERFECT = 0x00000400;
    constexpr uint32_t EMAC_FRMFILTER_SADDR = 0x00000200;
    constexpr uint32_t EMAC_FRMFILTER_INV_SADDR = 0x00000100;
    constexpr uint32_t EMAC_FRMFILTER_PASS_MASK = (0x03 << 6);
    constexpr uint32_t EMAC_FRMFILTER_PASS_NO_CTRL = (0x00 << 6);
    constexpr uint32_t EMAC_FRMFILTER_PASS_NO_PAUSE = (0x01 << 6);
    constexpr uint32_t EMAC_FRMFILTER_PASS_ALL_CTRL = (0x02 << 6);
    constexpr uint32_t EMAC_FRMFILTER_PASS_ADDR_CTRL = (0x03 << 6);
    constexpr uint32_t EMAC_FRMFILTER_BROADCAST = 0x00000020;
    constexpr uint32_t EMAC_FRMFILTER_PASS_MULTICAST = 0x00000010;
    constexpr uint32_t EMAC_FRMFILTER_INV_DADDR = 0x00000008;
    constexpr uint32_t EMAC_FRMFILTER_HASH_MULTICAST = 0x00000004;
    constexpr uint32_t EMAC_FRMFILTER_HASH_UNICAST = 0x00000002;
    constexpr uint32_t EMAC_FRMFILTER_PROMISCUOUS = 0x00000001;

    constexpr uint32_t EMAC_TS_MAC_FILTER_ENABLE = 0x00040000;
    constexpr uint32_t EMAC_TS_MAC_FILTER_DISABLE = 0x00000000;
    constexpr uint32_t EMAC_TS_SYNC_FOLLOW_DREQ_DRESP = 0x00000000;
    constexpr uint32_t EMAC_TS_SYNC_ONLY = 0x00004000;
    constexpr uint32_t EMAC_TS_DELAYREQ_ONLY = 0x0000C000;
    constexpr uint32_t EMAC_TS_ALL = 0x00010000;
    constexpr uint32_t EMAC_TS_SYNC_PDREQ_PDRESP = 0x00014000;
    constexpr uint32_t EMAC_TS_DREQ_PDREQ_PDRESP = 0x0001C000;
    constexpr uint32_t EMAC_TS_SYNC_DELAYREQ = 0x00020000;
    constexpr uint32_t EMAC_TS_PDREQ_PDRESP = 0x00030000;
    constexpr uint32_t EMAC_TS_PROCESS_IPV4_UDP = 0x00002000;
    constexpr uint32_t EMAC_TS_PROCESS_IPV6_UDP = 0x00001000;
    constexpr uint32_t EMAC_TS_PROCESS_ETHERNET = 0x00000800;
    constexpr uint32_t EMAC_TS_PTP_VERSION_2 = 0x00000400;
    constexpr uint32_t EMAC_TS_PTP_VERSION_1 = 0x00000000;
    constexpr uint32_t EMAC_TS_DIGITAL_ROLLOVER = 0x00000200;
    constexpr uint32_t EMAC_TS_BINARY_ROLLOVER = 0x00000000;
    constexpr uint32_t EMAC_TS_ALL_RX_FRAMES = 0x00000100;
    constexpr uint32_t EMAC_TS_UPDATE_FINE = 0x00000002;
    constexpr uint32_t EMAC_TS_UPDATE_COARSE = 0x00000000;

    constexpr uint32_t EMAC_INT_PHY = 0x80000000;
    constexpr uint32_t EMAC_INT_EARLY_RECEIVE = 0x00004000;
    constexpr uint32_t EMAC_INT_BUS_ERROR = 0x00002000;
    constexpr uint32_t EMAC_INT_EARLY_TRANSMIT = 0x00000400;
    constexpr uint32_t EMAC_INT_RX_WATCHDOG = 0x00000200;
    constexpr uint32_t EMAC_INT_RX_STOPPED = 0x00000100;
    constexpr uint32_t EMAC_INT_RX_NO_BUFFER = 0x00000080;
    constexpr uint32_t EMAC_INT_RECEIVE = 0x00000040;
    constexpr uint32_t EMAC_INT_TX_UNDERFLOW = 0x00000020;
    constexpr uint32_t EMAC_INT_RX_OVERFLOW = 0x00000010;
    constexpr uint32_t EMAC_INT_TX_JABBER = 0x00000008;
    constexpr uint32_t EMAC_INT_TX_NO_BUFFER = 0x00000004;
    constexpr uint32_t EMAC_INT_TX_STOPPED = 0x00000002;
    constexpr uint32_t EMAC_INT_TRANSMIT = 0x00000001;

    constexpr uint32_t EMAC_INT_NORMAL_INT = 0x00010000;
    constexpr uint32_t EMAC_INT_ABNORMAL_INT = 0x00008000;

    constexpr uint32_t EMAC_INT_LPI = 0x40000000;
    constexpr uint32_t EMAC_INT_TIMESTAMP = 0x20000000;
    constexpr uint32_t EMAC_TS_INT_TARGET_REACHED = 0x00000002;
    constexpr uint32_t EMAC_TS_INT_TS_SEC_OVERFLOW = 0x00000001;
    constexpr uint32_t EMAC_INT_POWER_MGMNT = 0x10000000;

    constexpr uint16_t phyAnlpaHalfDuplex10MHz = 5;
    constexpr uint16_t phyAnlpaFullDuplex10MHz = 6;
    constexpr uint16_t phyAnlpaHalfDuplex100MHz = 7;
    constexpr uint16_t phyAnlpaFullDuplex100MHz = 8;

    const std::array<uint32_t, 4> toTivaLinkSpeed =
    {{
        EMAC_PHY_AN_10B_T_FULL_DUPLEX,  /* fullDuplex10MHz */
        EMAC_PHY_AN_10B_T_HALF_DUPLEX,  /* halfDuplex10MHz */
        EMAC_PHY_AN_100B_T_FULL_DUPLEX, /* fullDuplex100MHz */
        EMAC_PHY_AN_100B_T_HALF_DUPLEX  /* halfDuplex100MHz */
    }};

    struct MiiClock
    {
        uint32_t systemClockMax;
        uint32_t divisor;
    };

    const std::array<MiiClock, 3> miiClockTable =
    {{
        { 64000000, EMAC_MIIADDR_CR_35_60 },
        { 104000000, EMAC_MIIADDR_CR_60_100 },
        { 150000000, EMAC_MIIADDR_CR_100_150 }
    }};

    constexpr uint32_t VALID_CONFIG_FLAGS = (EMAC_CONFIG_USE_MACADDR1 |                   \
                                 EMAC_CONFIG_SA_INSERT |                      \
                                 EMAC_CONFIG_SA_REPLACE |                     \
                                 EMAC_CONFIG_2K_PACKETS |                     \
                                 EMAC_CONFIG_STRIP_CRC |                      \
                                 EMAC_CONFIG_JABBER_DISABLE |                 \
                                 EMAC_CONFIG_JUMBO_ENABLE |                   \
                                 EMAC_CONFIG_IF_GAP_MASK |                    \
                                 EMAC_CONFIG_CS_DISABLE |                     \
                                 EMAC_CONFIG_100MBPS |                        \
                                 EMAC_CONFIG_RX_OWN_DISABLE |                 \
                                 EMAC_CONFIG_LOOPBACK |                       \
                                 EMAC_CONFIG_FULL_DUPLEX |                    \
                                 EMAC_CONFIG_CHECKSUM_OFFLOAD |               \
                                 EMAC_CONFIG_RETRY_DISABLE |                  \
                                 EMAC_CONFIG_AUTO_CRC_STRIPPING |             \
                                 EMAC_CONFIG_BO_MASK |                        \
                                 EMAC_CONFIG_DEFERRAL_CHK_ENABLE |            \
                                 EMAC_CONFIG_PREAMBLE_MASK);

    constexpr uint32_t VALID_FRMFILTER_FLAGS = (EMAC_FRMFILTER_RX_ALL |                      \
                                 EMAC_FRMFILTER_VLAN |                        \
                                 EMAC_FRMFILTER_HASH_AND_PERFECT |            \
                                 EMAC_FRMFILTER_SADDR |                       \
                                 EMAC_FRMFILTER_INV_SADDR |                   \
                                 EMAC_FRMFILTER_PASS_NO_PAUSE |               \
                                 EMAC_FRMFILTER_PASS_ALL_CTRL |               \
                                 EMAC_FRMFILTER_PASS_ADDR_CTRL |              \
                                 EMAC_FRMFILTER_BROADCAST |                   \
                                 EMAC_FRMFILTER_PASS_MULTICAST |              \
                                 EMAC_FRMFILTER_INV_DADDR |                   \
                                 EMAC_FRMFILTER_HASH_MULTICAST |              \
                                 EMAC_FRMFILTER_HASH_UNICAST |                \
                                 EMAC_FRMFILTER_PROMISCUOUS);

    constexpr uint32_t EMAC_NON_MASKED_INTS = (EMAC_DMARIS_LPI |                            \
                                 EMAC_DMARIS_TT |                             \
                                 EMAC_DMARIS_PMT |                            \
                                 EMAC_DMARIS_MMC);

    constexpr uint32_t EMAC_NORMAL_INTS = (EMAC_INT_TRANSMIT |                          \
                                 EMAC_INT_RECEIVE |                           \
                                 EMAC_INT_EARLY_RECEIVE |                     \
                                 EMAC_INT_TX_NO_BUFFER);

    constexpr uint32_t EMAC_ABNORMAL_INTS = (EMAC_INT_TX_STOPPED |                        \
                                 EMAC_INT_TX_JABBER |                         \
                                 EMAC_INT_RX_OVERFLOW |                       \
                                 EMAC_INT_TX_UNDERFLOW |                      \
                                 EMAC_INT_RX_NO_BUFFER |                      \
                                 EMAC_INT_RX_STOPPED |                        \
                                 EMAC_INT_RX_WATCHDOG |                       \
                                 EMAC_INT_EARLY_TRANSMIT |                    \
                                 EMAC_INT_BUS_ERROR);

    constexpr uint32_t EMAC_CONFIG = (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD | EMAC_CONFIG_7BYTE_PREAMBLE | EMAC_CONFIG_IF_GAP_96BITS | EMAC_CONFIG_USE_MACADDR0 | EMAC_CONFIG_SA_FROM_DESCRIPTOR | EMAC_CONFIG_BO_LIMIT_1024);
    constexpr uint32_t EMAC_MODE = (EMAC_MODE_RX_STORE_FORWARD | EMAC_MODE_TX_STORE_FORWARD | EMAC_MODE_TX_THRESHOLD_64_BYTES | EMAC_MODE_RX_THRESHOLD_64_BYTES);

    constexpr uint16_t DEV_ADDR(const uint16_t& address)
    {
        return ((address & 0xF000) >> 12);
    }

    constexpr uint16_t REG_ADDR(const uint16_t& address)
    {
        return ((address & 0x0FFF));
    }
}

namespace hal::tiva
{
    Ethernet::Ethernet(Leds leds, PhySelection phySelection, hal::LinkSpeed linkSpeed, hal::MacAddress macAddress)
        : led0{ leds.led0, PinConfigPeripheral::ethernetLed0 }
        , led1{ leds.led1, PinConfigPeripheral::ethernetLed1 }
        , led2{ leds.led2, PinConfigPeripheral::ethernetLed2 }
        , macAddress(macAddress)
        , phyId(phySelection == PhySelection::internal ? 0 : 1)
        , interrupt(EMAC0_IRQn, [this]()
            {
                Interrupt();
            })
    {
        EnableEMACClock();
        ResetEMACClock();

        really_assert(IsEPHYPresent());

        EnableEPHYClock();
        ResetEPHYClock();

        while (!IsEMACReady());

        SelectPhy(phySelection, linkSpeed);
        InitializeEthernetMac(EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 4, 4, 0);
        ConfigureEthernetMac(EMAC_CONFIG, EMAC_MODE, 0);
        SetMacAddress(macAddress);

        ReadPhy(phyId, EPHY_MISR1);
        ReadPhy(phyId, EPHY_MISR2);

        uint16_t enableLinkStatus = ReadPhy(phyId, EPHY_SCR) | (EPHY_SCR_INTEN_EXT | EPHY_SCR_INTOE_EXT);
        WritePhy(phyId, EPHY_SCR, enableLinkStatus);
        WritePhy(phyId, EPHY_MISR1, (EPHY_MISR1_LINKSTATEN | EPHY_MISR1_SPEEDEN | EPHY_MISR1_DUPLEXMEN | EPHY_MISR1_ANCEN));

        ReadPhy(phyId, EPHY_MISR1);
    }

    Ethernet::~Ethernet()
    {
        EMAC0->DMABUSMOD |= EMAC_DMABUSMOD_SWR;
        while (EMAC0->DMABUSMOD & EMAC_DMABUSMOD_SWR);
    }

    void Ethernet::Initialize()
    {
        receiveDescriptors.Emplace(*this);
        sendDescriptors.Emplace(*this);

        SetMacFilter(EMAC_FRMFILTER_HASH_AND_PERFECT | EMAC_FRMFILTER_PASS_MULTICAST);

        ConfigureTimestamp(EMAC_TS_ALL_RX_FRAMES | EMAC_TS_DIGITAL_ROLLOVER | EMAC_TS_PROCESS_IPV4_UDP | EMAC_TS_ALL | EMAC_TS_PTP_VERSION_1 | EMAC_TS_UPDATE_FINE, 1000000000 / (25000000 / 2), 0x80000000);
        EnableTimestamp();

        ClearInterruptPending(GetInterruptStatus(false));

        EnableTxInterrupts();
        EnableRxInterrupts();

        EnableInterruptsSource(EMAC_INT_RECEIVE | EMAC_INT_TRANSMIT | EMAC_INT_TX_STOPPED | EMAC_INT_RX_NO_BUFFER | EMAC_INT_RX_STOPPED | EMAC_INT_PHY);

        WritePhy(phyId, EPHY_BMCR, (EPHY_BMCR_ANEN | EPHY_BMCR_RESTARTAN));
    }

    void Ethernet::SendBuffer(infra::ConstByteRange data, bool last)
    {
        sendDescriptors->SendBuffer(data, last);
    }

    void Ethernet::RetryAllocation()
    {
        receiveDescriptors->RequestReceiveBuffers();
    }

    void Ethernet::AddMacAddressFilter(hal::MacAddress address)
    {
        auto lr = reinterpret_cast<const uint32_t*>(address.data())[0];
        auto hr = (reinterpret_cast<const uint32_t*>(address.data())[1] & 0xffff) | (1 << 31);

        if ((EMAC0->ADDR1H & infra::Bit<uint32_t>(31)) == 0)
        {
            EMAC0->ADDR1L = lr;
            EMAC0->ADDR1H = hr;
        }
        else if ((EMAC0->ADDR2H & infra::Bit<uint32_t>(31)) == 0)
        {
            EMAC0->ADDR2L = lr;
            EMAC0->ADDR2H = hr;
        }
        else if ((EMAC0->ADDR3H & infra::Bit<uint32_t>(31)) == 0)
        {
            EMAC0->ADDR3L = lr;
            EMAC0->ADDR3H = hr;
        }
        else
            abort();
    }

    void Ethernet::RemoveMacAddressFilter(hal::MacAddress address)
    {
        auto lr = reinterpret_cast<const uint32_t*>(address.data())[0];
        auto hr = (reinterpret_cast<const uint32_t*>(address.data())[1] & 0xffff) | (1 << 31);

        if (EMAC0->ADDR1H == hr && EMAC0->ADDR1L == lr)
        {
            EMAC0->ADDR1L = 0;
            EMAC0->ADDR1H = 0;
        }
        else if (EMAC0->ADDR2H == hr && EMAC0->ADDR2L == lr)
        {
            EMAC0->ADDR2L = 0;
            EMAC0->ADDR2H = 0;
        }
        else if (EMAC0->ADDR3H == hr && EMAC0->ADDR3L == lr)
        {
            EMAC0->ADDR3L = 0;
            EMAC0->ADDR3H = 0;
        }
        else
            abort();
    }

    uint16_t Ethernet::PhyAddress() const
    {
        return 0;
    }

    void Ethernet::Interrupt()
    {
        auto status = GetInterruptStatus(true);

        if (status & EMAC_INT_LPI)
            auto content = EMAC0->PMTCTLSTAT;

        if (status & EMAC_INT_POWER_MGMNT)
        {
            EnableTxInterrupts();
            EnableRxInterrupts();

            auto content = EMAC0->PMTCTLSTAT & (EMAC_PMTCTLSTAT_WUPRX | EMAC_PMTCTLSTAT_MGKPRX | EMAC_PMTCTLSTAT_PWRDWN);

            status &= ~EMAC_INT_POWER_MGMNT;
        }

        if (status)
            ClearInterruptPending(status);

        if (status & EMAC_INT_TIMESTAMP)
            auto content = EMAC0->TIMSTAT;

        if (status)
            ProcessInterrupt(status);
    }

    void Ethernet::EnableEMACClock() const
    {
        infra::ReplaceBit(SYSCTL->RCGCEMAC, true, 0);
    }

    void Ethernet::ResetEMACClock() const
    {
        infra::ReplaceBit(SYSCTL->SREMAC, true, 0);

        for(uint32_t delay = 0; delay < 16; delay++);

        infra::ReplaceBit(SYSCTL->SREMAC, false, 0);
    }

    bool Ethernet::IsEMACReady() const
    {
        return infra::IsBitSet(SYSCTL->RCGCEMAC, 0);
    }

    void Ethernet::EnableEPHYClock() const
    {
        infra::ReplaceBit(SYSCTL->RCGCEPHY, true, 0);
    }

    void Ethernet::ResetEPHYClock() const
    {
        infra::ReplaceBit(SYSCTL->SREPHY, true, 0);

        for(uint32_t delay = 0; delay < 16; delay++);

        infra::ReplaceBit(SYSCTL->SREPHY, false, 0);
    }

    bool Ethernet::IsEPHYPresent() const
    {
        return infra::IsBitSet(SYSCTL->PPEPHY, 0);
    }

    bool Ethernet::IsEPHYReady() const
    {
        return infra::IsBitSet(SYSCTL->RCGCEPHY, 0);
    }

    void Ethernet::SelectPhy(PhySelection phySelection, hal::LinkSpeed linkSpeed) const
    {
        EMAC0->PC = EMAC_PHY_INT_MDIX_EN | static_cast<uint32_t>(phySelection) | toTivaLinkSpeed.at(static_cast<uint32_t>(linkSpeed));

        if (phySelection == PhySelection::internal)
        {
            ResetEPHYClock();
            while (!IsEPHYReady());
            Delay(10000);
        }

        if (phySelection == PhySelection::externalRmii)
            EMAC0->CC |= EMAC_CC_CLKEN;
        else
            EMAC0->CC &= ~EMAC_CC_CLKEN;

        ResetEthernetMac();
        Delay(10000);
    }

    void Ethernet::ResetEthernetMac() const
    {
        EMAC0->DMABUSMOD |= EMAC_DMABUSMOD_SWR;
        while (EMAC0->DMABUSMOD & EMAC_DMABUSMOD_SWR);
    }

    void Ethernet::InitializeEthernetMac(uint32_t busConfig, uint32_t rxBurst, uint32_t txBurst, uint32_t descriptorSkipSize) const
    {
        really_assert(descriptorSkipSize < 32);
        really_assert(txBurst < (32 * 8));
        really_assert(rxBurst < (32 * 8));

        while (EMAC0->DMABUSMOD & EMAC_DMABUSMOD_SWR);

        uint32_t dmaBusMode = (busConfig | (descriptorSkipSize << EMAC_DMABUSMOD_DSL_S) | EMAC_DMABUSMOD_ATDS);

        if ((txBurst > 32) || (rxBurst > 32))
        {
            dmaBusMode |= EMAC_DMABUSMOD_8XPBL;
            txBurst >>= 3;
            rxBurst >>= 3;

            really_assert(rxBurst);
            really_assert(txBurst);
        }

        if (rxBurst == txBurst)
            dmaBusMode |= (txBurst << EMAC_DMABUSMOD_PBL_S);
        else
            dmaBusMode |= (EMAC_DMABUSMOD_USP | (txBurst << EMAC_DMABUSMOD_PBL_S) | (rxBurst << EMAC_DMABUSMOD_RPBL_S));

        EMAC0->DMABUSMOD = dmaBusMode;

        auto divisor = miiClockTable.back().divisor;

        for (uint32_t i = 0; i < miiClockTable.size(); i++)
        {
            if (SystemCoreClock <= miiClockTable[i].systemClockMax)
            {
                divisor = miiClockTable[i].divisor;
                break;
            }
        }

        EMAC0->MIIADDR = ((EMAC0->MIIADDR & ~EMAC_MIIADDR_CR_M) | divisor);
        EMAC0->MMCRXIM = 0xFFFFFFFF;
        EMAC0->MMCTXIM = 0xFFFFFFFF;
    }

    void Ethernet::ConfigureEthernetMac(uint32_t configuration, uint32_t mode, uint32_t rxMaxFrameSize) const
    {
        really_assert((configuration & ~(VALID_CONFIG_FLAGS |  EMAC_CONFIG_TX_ENABLED | EMAC_CONFIG_RX_ENABLED)) == 0);
        really_assert(!rxMaxFrameSize || ((rxMaxFrameSize < 0x4000) && (rxMaxFrameSize > 1522)));

        EMAC0->CFG = ((EMAC0->CFG) & ~VALID_CONFIG_FLAGS) | configuration | EMAC_CFG_PS;

        if (rxMaxFrameSize)
            EMAC0->WDOGTO = rxMaxFrameSize | EMAC_WDOGTO_PWE;
        else
            EMAC0->WDOGTO &= ~EMAC_WDOGTO_PWE;

        EMAC0->DMAOPMODE = mode;
    }

    void Ethernet::SetMacAddress(hal::MacAddress macAddress) const
    {
        EMAC0->ADDR0H = (EMAC0->ADDR0H & 0xFFFF0000) | macAddress[4] | (macAddress[5] << 8); //NOSONAR
        EMAC0->ADDR0L = macAddress[0] | (macAddress[1] << 8) | (macAddress[2] << 16) | (macAddress[3] << 24); //NOSONAR
    }

    uint16_t Ethernet::ReadExtendedPhy(uint8_t phyAddress, uint16_t registerAddress)
    {
        really_assert(phyAddress < 32);

        WritePhy(phyAddress, EPHY_REGCTL, DEV_ADDR(registerAddress));
        WritePhy(phyAddress, EPHY_ADDAR,  REG_ADDR(registerAddress));
        WritePhy(phyAddress, EPHY_REGCTL, (0x4000 | DEV_ADDR(registerAddress)));

        return ReadPhy(phyAddress, EPHY_ADDAR);
    }

    uint16_t Ethernet::ReadPhy(uint8_t phyAddress, uint8_t registerAddress) const
    {
        really_assert(phyAddress < 32);

        while (EMAC0->MIIADDR & EMAC_MIIADDR_MIIB);

        EMAC0->MIIADDR = ((EMAC0->MIIADDR & EMAC_MIIADDR_CR_M) | (registerAddress << EMAC_MIIADDR_MII_S) | (phyAddress << EMAC_MIIADDR_PLA_S) | EMAC_MIIADDR_MIIB); //NOSONAR

        while (EMAC0->MIIADDR & EMAC_MIIADDR_MIIB);

        return (EMAC0->MIIDATA & EMAC_MIIDATA_DATA_M);
    }

    void Ethernet::WritePhy(uint8_t phyAddress, uint8_t registerAddress, uint16_t data) const
    {
        really_assert(phyAddress < 32);

        while (EMAC0->MIIADDR & EMAC_MIIADDR_MIIB);

        EMAC0->MIIDATA = data;
        EMAC0->MIIADDR = ((EMAC0->MIIADDR & EMAC_MIIADDR_CR_M) | (registerAddress << EMAC_MIIADDR_MII_S) | (phyAddress << EMAC_MIIADDR_PLA_S) | EMAC_MIIADDR_MIIW | EMAC_MIIADDR_MIIB); //NOSONAR

        while (EMAC0->MIIADDR & EMAC_MIIADDR_MIIB);
    }

    void Ethernet::SetMacFilter(uint32_t options) const
    {
        EMAC0->FRAMEFLTR = (EMAC0->FRAMEFLTR & ~VALID_FRMFILTER_FLAGS) | options;
    }

    void Ethernet::ConfigureTimestamp(uint32_t options, uint32_t subsecondClockIncrement, uint32_t adjust) const
    {
        EMAC0->CC |= EMAC_CC_PTPCEN;
        EMAC0->SUBSECINC = ((subsecondClockIncrement << EMAC_SUBSECINC_SSINC_S) & EMAC_SUBSECINC_SSINC_M);
        EMAC0->TIMSTCTRL = options;

        EMAC0->TIMADD = adjust;
        while (EMAC0->TIMSTCTRL & EMAC_TIMSTCTRL_ADDREGUP);
        EMAC0->TIMSTCTRL |= EMAC_TIMSTCTRL_ADDREGUP;
    }

    void Ethernet::EnableTimestamp() const
    {
        EMAC0->TIMSTCTRL |= EMAC_TIMSTCTRL_TSEN;

        if (!(EMAC0->TIMSTCTRL & EMAC_TIMSTCTRL_TSINIT))
            EMAC0->TIMSTCTRL |= EMAC_TIMSTCTRL_TSINIT;
    }

    void Ethernet::ClearInterruptPending(uint32_t flags) const
    {
        if (flags & EMAC_NORMAL_INTS)
            flags |= EMAC_INT_NORMAL_INT;

        if (flags & EMAC_ABNORMAL_INTS)
            flags |= EMAC_INT_ABNORMAL_INT;

        if (flags & ~EMAC_INT_PHY)
            EMAC0->DMARIS = (flags & ~EMAC_INT_PHY);

        if (flags & EMAC_INT_PHY)
            EMAC0->EPHYMISC |= EMAC_EPHYMISC_INT;
    }

    uint32_t Ethernet::GetInterruptStatus(bool mask) const
    {
        uint32_t phyStatus = 0;
        uint32_t rawStatus = EMAC0->DMARIS & ~(EMAC_DMARIS_AE_M | EMAC_DMARIS_TS_M | EMAC_DMARIS_RS_M);

        if (mask)
            rawStatus &= (EMAC_NON_MASKED_INTS | EMAC0->DMAIM);

        if (mask)
            phyStatus = EMAC0->EPHYMISC;
        else
            phyStatus = EMAC0->EPHYRIS;

        if (phyStatus & EMAC_EPHYMISC_INT)
            rawStatus |= EMAC_INT_PHY;

        return rawStatus;
    }

    void Ethernet::EnableTxInterrupts() const
    {
        EMAC0->DMAOPMODE |= EMAC_DMAOPMODE_ST;
        EMAC0->CFG |= EMAC_CFG_TE;
    }

    void Ethernet::EnableRxInterrupts() const
    {
        EMAC0->DMAOPMODE |= EMAC_DMAOPMODE_SR;
        EMAC0->CFG |= EMAC_CFG_RE;
    }

    void Ethernet::EnableInterruptsSource(uint32_t options) const
    {
        if (options & EMAC_NORMAL_INTS)
            options |= EMAC_INT_NORMAL_INT;

        if (options & EMAC_ABNORMAL_INTS)
            options |= EMAC_INT_ABNORMAL_INT;

        if (options & ~EMAC_INT_PHY)
            EMAC0->DMAIM |= options & ~EMAC_INT_PHY;

        if (options & EMAC_INT_PHY)
            EMAC0->EPHYIM |= EMAC_EPHYIM_INT;
    }

    void Ethernet::GetEthernetMacConfiguration(uint32_t& config, uint32_t& mode, uint32_t& maxRxFrameSize)
    {
        config = EMAC0->DMAOPMODE;
        mode = EMAC0->CFG & (VALID_CONFIG_FLAGS | EMAC_CONFIG_TX_ENABLED | EMAC_CONFIG_RX_ENABLED);
        auto value = EMAC0->WDOGTO;

        if (value & EMAC_WDOGTO_PWE)
            maxRxFrameSize = value & EMAC_WDOGTO_WTO_M;
        else
            if (EMAC0->CFG & EMAC_CFG_JFEN)
                maxRxFrameSize = 10240;
            else
                maxRxFrameSize = 2048;
    }

    void Ethernet::SetEthernetMacConfiguration(uint32_t config, uint32_t mode, uint32_t maxRxFrameSize)
    {
        EMAC0->CFG = (EMAC0->CFG & ~VALID_CONFIG_FLAGS) | config | EMAC_CFG_PS;

        if (maxRxFrameSize)
            EMAC0->WDOGTO = maxRxFrameSize | EMAC_WDOGTO_PWE;
        else
            EMAC0->WDOGTO &= ~EMAC_WDOGTO_PWE;

        EMAC0->DMAOPMODE = mode;
    }

    void Ethernet::ConfigureLPITimers(bool config, uint16_t lsTimerInMs, uint16_t twTimerInMs)
    {
        auto timerValue = ((lsTimerInMs << EMAC_LPITIMERCTL_LST_S) & EMAC_LPITIMERCTL_LST_M) | twTimerInMs & EMAC_LPITIMERCTL_TWT_M;

        EMAC0->LPITIMERCTL = timerValue;

        if (config)
            EMAC0->LPICTLSTAT |= EMAC_LPICTLSTAT_LPITXA;
        else
            EMAC0->LPICTLSTAT = 0;
    }

    void Ethernet::ProcessInterrupt(uint32_t status)
    {
        if (status & EMAC_INT_PHY)
            ProcessPhyInterrupt();

        if (status & EMAC_INT_TRANSMIT)
        {
            if (EEELinkActive)
                EMAC0->LPICTLSTAT |= EMAC_LPICTLSTAT_LPIEN;

            sendDescriptors->SentFrame();
        }

        if (status & (EMAC_INT_RECEIVE | EMAC_INT_RX_NO_BUFFER | EMAC_INT_RX_STOPPED))
            receiveDescriptors->ReceivedFrame();
    }

    void Ethernet::ProcessPhyInterrupt()
    {
        auto interruptStatus = ReadPhy(phyId, EPHY_MISR1);
        auto phyStatus = ReadPhy(phyId, EPHY_STS);
        auto EEEStatus = ReadExtendedPhy(phyId, 0x703D);

        if (interruptStatus & EPHY_MISR1_LINKSTAT)
        {
            if (phyStatus & EPHY_STS_LINK)
            {
                auto linkAbility = ReadPhy(phyId, EPHY_ANA);
                auto linkPartnerAbility = ReadPhy(phyId, EPHY_ANLPA);

                LinkSpeed speed;
                if (infra::IsBitSet(linkPartnerAbility, phyAnlpaFullDuplex100MHz) && infra::IsBitSet(linkAbility, phyAnlpaFullDuplex100MHz))
                    speed = LinkSpeed::fullDuplex100MHz;
                else if (infra::IsBitSet(linkPartnerAbility, phyAnlpaHalfDuplex100MHz) && infra::IsBitSet(linkAbility, phyAnlpaHalfDuplex100MHz))
                    speed = LinkSpeed::halfDuplex100MHz;
                else if (infra::IsBitSet(linkPartnerAbility, phyAnlpaFullDuplex10MHz) && infra::IsBitSet(linkAbility, phyAnlpaFullDuplex10MHz))
                    speed = LinkSpeed::fullDuplex10MHz;
                else
                    speed = LinkSpeed::halfDuplex10MHz;

                EthernetSmi::GetObserver().LinkUp(speed);

                if (EEEStatus & 0x2)
                {
                    ConfigureLPITimers(true, 1000, 36);
                    EMAC0->LPICTLSTAT |= EMAC_LPICTLSTAT_PLS;
                    EEELinkActive = true;
                }
            }
            else
            {
                EthernetSmi::GetObserver().LinkDown();

                EEELinkActive = false;
                EMAC0->LPICTLSTAT &= ~EMAC_LPICTLSTAT_PLS;
                ConfigureLPITimers(false, 1000, 0);
            }
        }

        if (interruptStatus & (EPHY_MISR1_SPEED | EPHY_MISR1_SPEED | EPHY_MISR1_ANC))
        {
            uint32_t config = 0;
            uint32_t mode = 0;
            uint32_t maxRxFrameSize = 0;

            GetEthernetMacConfiguration(config, mode, maxRxFrameSize);

            if (phyStatus & EPHY_STS_SPEED)
                config &= ~EMAC_CONFIG_100MBPS;
            else
                config |= EMAC_CONFIG_100MBPS;

            if (phyStatus & EPHY_STS_DUPLEX)
                config |= EMAC_CONFIG_FULL_DUPLEX;
            else
                config &= ~EMAC_CONFIG_FULL_DUPLEX;

            SetEthernetMacConfiguration(config, mode, maxRxFrameSize);
        }
    }

    Ethernet::ReceiveDescriptors::ReceiveDescriptors(Ethernet& ethernetMac)
        : ethernetMac(ethernetMac)
    {
        for (auto& descriptor : descriptors)
        {
            descriptor.Desc0 = DES0_RX_CTRL_OWN;
            descriptor.Desc1 = DES1_RX_CTRL_CHAINED;
            descriptor.Desc3.link = &descriptor + 1;
            descriptor.Desc4 = 0;
        }

        descriptors.back().Desc1 |= DES1_RX_CTRL_END_OF_RING;
        descriptors.back().Desc3.link = &descriptors.front();

        EMAC0->RXDLADDR = reinterpret_cast<uint32_t>(descriptors.data()); //NOSONAR

        infra::EventDispatcher::Instance().Schedule([this]()
            {
                RequestReceiveBuffers();
            });
    }

    void Ethernet::ReceiveDescriptors::ReceivedFrame()
    {
        while (receivedFramesAllocated != 0 && (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_CTRL_OWN) == 0)
        {
            bool receiveDone = (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_CTRL_OWN) == 0 && (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_STAT_LAST_DESC) != 0;
            uint16_t frameSize = (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_STAT_FRAME_LENGTH_M) >> DES0_RX_STAT_FRAME_LENGTH_S;
            bool errorFrame = (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_STAT_ERR) != 0 && (descriptors[receiveDescriptorReceiveIndex].Desc0 & DES0_RX_STAT_LAST_DESC) != 0;

            descriptors[receiveDescriptorReceiveIndex].Desc2 = nullptr;
            ++receivedFrameBuffers;
            --receivedFramesAllocated;
            ++receiveDescriptorReceiveIndex;

            if (receiveDescriptorReceiveIndex == descriptors.size())
                receiveDescriptorReceiveIndex = 0;

            RequestReceiveBuffer();

            if (receiveDone)
            {
                if (!errorFrame)
                    ethernetMac.EthernetMac::GetObserver().ReceivedFrame(receivedFrameBuffers, frameSize);
                else
                    ethernetMac.EthernetMac::GetObserver().ReceivedErrorFrame(receivedFrameBuffers, frameSize);

                receivedFrameBuffers = 0;
            }
        }
    }

    void Ethernet::ReceiveDescriptors::RequestReceiveBuffers()
    {
        while (receivedFramesAllocated != descriptors.size())
            if (!RequestReceiveBuffer())
                break;
    }

    bool Ethernet::ReceiveDescriptors::RequestReceiveBuffer()
    {
        infra::ByteRange buffer = ethernetMac.EthernetMac::GetObserver().RequestReceiveBuffer();
        if (buffer.empty())
            return false;

        descriptors[receiveDescriptorAllocatedIndex].Desc1 = (buffer.size() << DES1_RX_CTRL_BUFF1_SIZE_S) | DES1_RX_CTRL_CHAINED;
        descriptors[receiveDescriptorAllocatedIndex].Desc2 = buffer.begin();
        descriptors[receiveDescriptorAllocatedIndex].Desc0 |= DES0_RX_CTRL_OWN;

#if 0   // Check if it is necessary...
        __DSB();
        EMAC0->RXPOLLD = 0;
#endif

        ++receivedFramesAllocated;
        ++receiveDescriptorAllocatedIndex;

        if (receiveDescriptorAllocatedIndex == descriptors.size())
            receiveDescriptorAllocatedIndex = 0;

        return true;
    }

    Ethernet::SendDescriptors::SendDescriptors(Ethernet& ethernetMac)
        : ethernetMac(ethernetMac)
    {
        for (auto& descriptor : descriptors)
        {
            descriptor.Desc0 = DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_IP_ALL_CKHSUMS;
            descriptor.Desc3.link = &descriptor + 1;
        }

        descriptors.back().Desc0 |= DES0_TX_CTRL_END_OF_RING;
        descriptors.back().Desc3.link = &descriptors.front();

        EMAC0->TXDLADDR = reinterpret_cast<uint32_t>(descriptors.data()); //NOSONAR
    }

    void Ethernet::SendDescriptors::SendBuffer(infra::ConstByteRange data, bool last)
    {
        really_assert((descriptors[sendDescriptorIndex].Desc0 & DES0_TX_CTRL_OWN) == 0);
        descriptors[sendDescriptorIndex].Desc1 = data.size();
        descriptors[sendDescriptorIndex].Desc2 = const_cast<uint8_t *>(data.begin()); //NOSONAR

        if (sendFirst)
            descriptors[sendDescriptorIndex].Desc0 |= DES0_TX_CTRL_FIRST_SEG;
        else
            descriptors[sendDescriptorIndex].Desc0 &= ~DES0_TX_CTRL_FIRST_SEG;

        if (last)
            descriptors[sendDescriptorIndex].Desc0 |= DES0_TX_CTRL_LAST_SEG;
        else
            descriptors[sendDescriptorIndex].Desc0 &= ~DES0_TX_CTRL_LAST_SEG;

        if (sendFirst)
            sendDescriptorIndexFirst = sendDescriptorIndex;
        else
            descriptors[sendDescriptorIndex].Desc0 |= DES0_TX_CTRL_OWN;

        if (last)
            descriptors[sendDescriptorIndexFirst].Desc0 |= DES0_TX_CTRL_OWN;

        __DSB();
        sendFirst = last;
        EMAC0->TXPOLLD = 0;

        ++sendDescriptorIndex;
        if (sendDescriptorIndex == descriptors.size())
            sendDescriptorIndex = 0;
    }

    void Ethernet::SendDescriptors::SentFrame()
    {
        uint32_t previousDescriptor = sendDescriptorIndex != 0 ? sendDescriptorIndex - 1 : descriptors.size() - 1;

        bool sentDone = (descriptors[previousDescriptor].Desc0 & DES0_TX_CTRL_OWN) == 0;

        really_assert(sentDone);

        if (sentDone)
        {
            descriptors[previousDescriptor].Desc0 &= ~DES0_TX_CTRL_LAST_SEG;
            ethernetMac.EthernetMac::GetObserver().SentFrame();
        }
    }
}
