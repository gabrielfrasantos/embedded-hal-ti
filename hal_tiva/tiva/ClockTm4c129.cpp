#include DEVICE_HEADER
#include "hal_tiva/tiva/ClockTm4c129.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <array>

namespace
{
    // NOLINTBEGIN
    constexpr const uint32_t SYSCTL_RSCLKCFG_MEMTIMU = 0x80000000; // Memory Timing Register Update
    constexpr const uint32_t SYSCTL_RSCLKCFG_NEWFREQ = 0x40000000; // New PLLFREQ Accept
    constexpr const uint32_t SYSCTL_RSCLKCFG_ACG = 0x20000000; // Auto Clock Gating
    constexpr const uint32_t SYSCTL_RSCLKCFG_USEPLL = 0x10000000; // Use PLL
    constexpr const uint32_t SYSCTL_RSCLKCFG_PLLSRC_M = 0x0F000000; // PLL Source
    constexpr const uint32_t SYSCTL_RSCLKCFG_PLLSRC_PIOSC = 0x00000000; // PIOSC is PLL input clock source
    constexpr const uint32_t SYSCTL_RSCLKCFG_PLLSRC_MOSC = 0x03000000; // MOSC is the PLL input clock source
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSCSRC_M = 0x00F00000; // Oscillator Source
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSCSRC_PIOSC = 0x00000000; // PIOSC is oscillator source
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSCSRC_LFIOSC = 0x00200000; // LFIOSC is oscillator source
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSCSRC_MOSC = 0x00300000; // MOSC is oscillator source
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSCSRC_RTC = 0x00400000; // Hibernation Module RTC Oscillator (RTCOSC)
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSYSDIV_M = 0x000FFC00; // Oscillator System Clock Divisor
    constexpr const uint32_t SYSCTL_RSCLKCFG_PSYSDIV_M = 0x000003FF; // PLL System Clock Divisor
    constexpr const uint32_t SYSCTL_RSCLKCFG_OSYSDIV_S = 10;
    constexpr const uint32_t SYSCTL_RSCLKCFG_PSYSDIV_S = 0;

    constexpr const uint32_t SYSCTL_XTAL_1MHZ = 0x00000000;  // External crystal is 1MHz
    constexpr const uint32_t SYSCTL_XTAL_1_84MHZ = 0x00000040;  // External crystal is 1.8432MHz
    constexpr const uint32_t SYSCTL_XTAL_2MHZ = 0x00000080;  // External crystal is 2MHz
    constexpr const uint32_t SYSCTL_XTAL_2_45MHZ = 0x000000C0;  // External crystal is 2.4576MHz
    constexpr const uint32_t SYSCTL_XTAL_3_57MHZ = 0x00000100;  // External crystal is 3.579545MHz
    constexpr const uint32_t SYSCTL_XTAL_3_68MHZ = 0x00000140;  // External crystal is 3.6864MHz
    constexpr const uint32_t SYSCTL_XTAL_4MHZ = 0x00000180;  // External crystal is 4MHz
    constexpr const uint32_t SYSCTL_XTAL_4_09MHZ = 0x000001C0;  // External crystal is 4.096MHz
    constexpr const uint32_t SYSCTL_XTAL_4_91MHZ = 0x00000200;  // External crystal is 4.9152MHz
    constexpr const uint32_t SYSCTL_XTAL_5MHZ = 0x00000240;  // External crystal is 5MHz
    constexpr const uint32_t SYSCTL_XTAL_5_12MHZ = 0x00000280;  // External crystal is 5.12MHz
    constexpr const uint32_t SYSCTL_XTAL_6MHZ = 0x000002C0;  // External crystal is 6MHz
    constexpr const uint32_t SYSCTL_XTAL_6_14MHZ = 0x00000300;  // External crystal is 6.144MHz
    constexpr const uint32_t SYSCTL_XTAL_7_37MHZ = 0x00000340;  // External crystal is 7.3728MHz
    constexpr const uint32_t SYSCTL_XTAL_8MHZ = 0x00000380;  // External crystal is 8MHz
    constexpr const uint32_t SYSCTL_XTAL_8_19MHZ = 0x000003C0;  // External crystal is 8.192MHz
    constexpr const uint32_t SYSCTL_XTAL_10MHZ = 0x00000400;  // External crystal is 10 MHz
    constexpr const uint32_t SYSCTL_XTAL_12MHZ = 0x00000440; // External crystal is 12 MHz
    constexpr const uint32_t SYSCTL_XTAL_12_2MHZ = 0x00000480;  // External crystal is 12.288 MHz
    constexpr const uint32_t SYSCTL_XTAL_13_5MHZ = 0x000004C0;  // External crystal is 13.56 MHz
    constexpr const uint32_t SYSCTL_XTAL_14_3MHZ = 0x00000500; // External crystal is 14.31818 MHz
    constexpr const uint32_t SYSCTL_XTAL_16MHZ = 0x00000540;  // External crystal is 16 MHz
    constexpr const uint32_t SYSCTL_XTAL_16_3MHZ = 0x00000580;  // External crystal is 16.384 MHz
    constexpr const uint32_t SYSCTL_XTAL_18MHZ = 0x000005C0;  // External crystal is 18.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_20MHZ = 0x00000600;  // External crystal is 20.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_24MHZ = 0x00000640;  // External crystal is 24.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_25MHZ = 0x00000680;  // External crystal is 25.0 MHz
    constexpr const uint32_t SYSCTL_OSC_MAIN = 0x00000000;  // Osc source is main osc
    constexpr const uint32_t SYSCTL_OSC_INT = 0x00000010;  // Osc source is int. osc
    constexpr const uint32_t SYSCTL_OSC_INT4 = 0x00000020;  // Osc source is int. osc /4
    constexpr const uint32_t SYSCTL_OSC_INT30 = 0x00000030;  // Osc source is int. 30 KHz
    constexpr const uint32_t SYSCTL_OSC_EXT32 = 0x80000038;  // Osc source is ext. 32 KHz
    constexpr const uint32_t SYSCTL_INT_OSC_DIS = 0x00000002;  // Disable internal oscillator
    constexpr const uint32_t SYSCTL_MAIN_OSC_DIS = 0x00000001;  // Disable main oscillator

    constexpr const uint32_t SYSCTL_MOSCCTL_OSCRNG = 0x00000010;  // Oscillator Range
    constexpr const uint32_t SYSCTL_MOSCCTL_PWRDN = 0x00000008;  // Power Down
    constexpr const uint32_t SYSCTL_MOSCCTL_NOXTAL = 0x00000004;  // No Crystal Connected
    constexpr const uint32_t SYSCTL_MOSCCTL_MOSCIM = 0x00000002;  // MOSC Failure Action
    constexpr const uint32_t SYSCTL_MOSCCTL_CVAL = 0x00000001;  // Clock Validation for MOSC

    constexpr const uint32_t SYSCTL_PLLFREQ0_PLLPWR = 0x00800000;  // PLL Power
    constexpr const uint32_t SYSCTL_PLLFREQ0_MFRAC_M = 0x000FFC00;  // PLL M Fractional Value
    constexpr const uint32_t SYSCTL_PLLFREQ0_MINT_M = 0x000003FF;  // PLL M Integer Value
    constexpr const uint32_t SYSCTL_PLLFREQ0_MFRAC_S = 10;
    constexpr const uint32_t SYSCTL_PLLFREQ0_MINT_S = 0;

    constexpr const uint32_t SYSCTL_PLLFREQ1_Q_M = 0x00001F00;  // PLL Q Value
    constexpr const uint32_t SYSCTL_PLLFREQ1_N_M = 0x0000001F;  // PLL N Value
    constexpr const uint32_t SYSCTL_PLLFREQ1_Q_S = 8;
    constexpr const uint32_t SYSCTL_PLLFREQ1_N_S = 0;

    constexpr const uint32_t SYSCTL_PLLSTAT_LOCK = 0x00000001;  // PLL Lock

    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_M = 0x03C00000;  // EEPROM Clock High Time
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_0_5 = 0x00000000;  // 1/2 system clock period
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_1 = 0x00400000;  // 1 system clock period
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_1_5 = 0x00800000;  // 1.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_2 = 0x00C00000;  // 2 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_2_5 = 0x01000000;  // 2.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_3 = 0x01400000;  // 3 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_3_5 = 0x01800000;  // 3.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_4 = 0x01C00000;  // 4 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCHT_4_5 = 0x02000000;  // 4.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_EBCE = 0x00200000;  // EEPROM Bank Clock Edge
    constexpr const uint32_t SYSCTL_MEMTIM0_MB1 = 0x00100010;  // Must be one
    constexpr const uint32_t SYSCTL_MEMTIM0_EWS_M = 0x000F0000;  // EEPROM Wait States
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_M = 0x000003C0;  // Flash Bank Clock High Time
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_0_5 = 0x00000000;  // 1/2 system clock period
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_1 = 0x00000040;  // 1 system clock period
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_1_5 = 0x00000080;  // 1.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_2 = 0x000000C0;  // 2 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_2_5 = 0x00000100;  // 2.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_3 = 0x00000140;  // 3 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_3_5 = 0x00000180;  // 3.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_4 = 0x000001C0;  // 4 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCHT_4_5 = 0x00000200;  // 4.5 system clock periods
    constexpr const uint32_t SYSCTL_MEMTIM0_FBCE = 0x00000020;  // Flash Bank Clock Edge
    constexpr const uint32_t SYSCTL_MEMTIM0_FWS_M = 0x0000000F;  // Flash Wait State
    constexpr const uint32_t SYSCTL_MEMTIM0_EWS_S = 16;
    constexpr const uint32_t SYSCTL_MEMTIM0_FWS_S = 0;

    constexpr const uint32_t SYSCTL_RIS_BOR0RIS = 0x00000800;  // VDD under BOR0 Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_VDDARIS = 0x00000400;  // VDDA Power OK Event Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_MOSCPUPRIS = 0x00000100;  // MOSC Power Up Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_USBPLLLRIS = 0x00000080;  // USB PLL Lock Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_PLLLRIS = 0x00000040;  // PLL Lock Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_MOFRIS = 0x00000008;  // Main Oscillator Failure Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_BOR1RIS = 0x00000002;  // VDD under BOR1 Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_BORRIS = 0x00000002;  // Brown-Out Reset Raw Interrupt Status

    constexpr const uint32_t SYSCTL_MOSC_VALIDATE = 0x00000001;  // Enable MOSC validation
    constexpr const uint32_t SYSCTL_MOSC_INTERRUPT = 0x00000002;  // Generate interrupt on MOSC fail
    constexpr const uint32_t SYSCTL_MOSC_NO_XTAL = 0x00000004;  // No crystal is attached to MOSC
    constexpr const uint32_t SYSCTL_MOSC_PWR_DIS = 0x00000008;  // Power down the MOSC.
    constexpr const uint32_t SYSCTL_MOSC_LOWFREQ = 0x00000000;  // MOSC is less than 10MHz
    constexpr const uint32_t SYSCTL_MOSC_HIGHFREQ = 0x00000010;  // MOSC is greater than 10MHz
    constexpr const uint32_t SYSCTL_MOSC_SESRC = 0x00000020;  // Singled ended oscillator source.

    const std::array<uint32_t, 27> crystalLookupTable = { {
        1000000,
        1843200,
        2000000,
        2457600,
        3579545,
        3686400,
        4000000,
        4096000,
        4915200,
        5000000,
        5120000,
        6000000,
        6144000,
        7372800,
        8000000,
        8192000,
        10000000,
        12000000,
        12288000,
        13560000,
        14318180,
        16000000,
        16384000,
        18000000,
        20000000,
        24000000,
        25000000
    } };

    constexpr const uint32_t MAX_VCO_ENTRIES = 2;
    constexpr const uint32_t MAX_XTAL_ENTRIES = 18;

    constexpr uint32_t PLL_M_TO_REG(uint32_t mi, uint32_t mf)
    {
        return (mi | (mf << SYSCTL_PLLFREQ0_MFRAC_S));
    }

    constexpr uint32_t PLL_N_TO_REG(uint32_t n)
    {
        return ((n - 1) << SYSCTL_PLLFREQ1_N_S);
    }
    constexpr uint32_t PLL_Q_TO_REG(uint32_t q)
    {
        return ((q - 1) << SYSCTL_PLLFREQ1_Q_S);
    }

    static const uint32_t pllFrequencyLookupTable[MAX_VCO_ENTRIES][MAX_XTAL_ENTRIES][3] =
    {
        {
            //
            // VCO 320 MHz
            //
            { PLL_M_TO_REG(64, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 5 MHz
            { PLL_M_TO_REG(62, 512), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 5.12 MHz
            { PLL_M_TO_REG(160, 0),  PLL_N_TO_REG(3), PLL_Q_TO_REG(2) },     // 6 MHz
            { PLL_M_TO_REG(52, 85),  PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 6.144 MHz
            { PLL_M_TO_REG(43, 412), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 7.3728 MHz
            { PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 8 MHz
            { PLL_M_TO_REG(39, 64),  PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 8.192 MHz
            { PLL_M_TO_REG(32, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 10 MHz
            { PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(3), PLL_Q_TO_REG(2) },     // 12 MHz
            { PLL_M_TO_REG(26, 43),  PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 12.288 MHz
            { PLL_M_TO_REG(23, 613), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 13.56 MHz
            { PLL_M_TO_REG(22, 358), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 14.318180 MHz
            { PLL_M_TO_REG(20, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 16 MHz
            { PLL_M_TO_REG(19, 544), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 16.384 MHz
            { PLL_M_TO_REG(160, 0),  PLL_N_TO_REG(9), PLL_Q_TO_REG(2) },     // 18 MHz
            { PLL_M_TO_REG(16, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 20 MHz
            { PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(3), PLL_Q_TO_REG(2) },     // 24 MHz
            { PLL_M_TO_REG(64, 0),   PLL_N_TO_REG(5), PLL_Q_TO_REG(2) },     // 25 MHz
        },
        {
            //
            // VCO 480 MHz
            //
            { PLL_M_TO_REG(96, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 5 MHz
            { PLL_M_TO_REG(93, 768), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 5.12 MHz
            { PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 6 MHz
            { PLL_M_TO_REG(78, 128), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 6.144 MHz
            { PLL_M_TO_REG(65, 107), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 7.3728 MHz
            { PLL_M_TO_REG(60, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 8 MHz
            { PLL_M_TO_REG(58, 608), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 8.192 MHz
            { PLL_M_TO_REG(48, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 10 MHz
            { PLL_M_TO_REG(40, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 12 MHz
            { PLL_M_TO_REG(39, 64),  PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 12.288 MHz
            { PLL_M_TO_REG(35, 408), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 13.56 MHz
            { PLL_M_TO_REG(33, 536), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 14.318180 MHz
            { PLL_M_TO_REG(30, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 16 MHz
            { PLL_M_TO_REG(29, 304), PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 16.384 MHz
            { PLL_M_TO_REG(80, 0),   PLL_N_TO_REG(3), PLL_Q_TO_REG(2) },     // 18 MHz
            { PLL_M_TO_REG(24, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 20 MHz
            { PLL_M_TO_REG(20, 0),   PLL_N_TO_REG(1), PLL_Q_TO_REG(2) },     // 24 MHz
            { PLL_M_TO_REG(96, 0),   PLL_N_TO_REG(5), PLL_Q_TO_REG(2) },     // 25 MHz
        },
    };

    static const struct
    {
        uint32_t frequency;
        uint32_t memoryTiming;
    }
    systemClockMapLookupTable[] =
    {
        { 16000000, (SYSCTL_MEMTIM0_FBCHT_0_5 | SYSCTL_MEMTIM0_FBCE | (0 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_0_5 | SYSCTL_MEMTIM0_EBCE | (0 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
        { 40000000, (SYSCTL_MEMTIM0_FBCHT_1_5 | (1 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_1_5 | (1 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
        { 60000000, (SYSCTL_MEMTIM0_FBCHT_2 | (2 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_2 | (2 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
        { 80000000, (SYSCTL_MEMTIM0_FBCHT_2_5 | (3 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_2_5 | (3 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
        { 100000000, (SYSCTL_MEMTIM0_FBCHT_3 | (4 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_3 | (4 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
        { 120000000, (SYSCTL_MEMTIM0_FBCHT_3_5 | (5 << SYSCTL_MEMTIM0_FWS_S) | SYSCTL_MEMTIM0_EBCHT_3_5 | (5 << SYSCTL_MEMTIM0_EWS_S) | SYSCTL_MEMTIM0_MB1) },
    };

    static uint32_t GetMemoryTimings(uint32_t systemClock)
    {
        for (uint_fast8_t index = 0; index < (sizeof(systemClockMapLookupTable) / sizeof(systemClockMapLookupTable[0])); index++)
            if (systemClock <= systemClockMapLookupTable[index].frequency)
                return (systemClockMapLookupTable[index].memoryTiming);

        return(0);
    }

    const uint32_t CalculateSystemFrequency(uint32_t crystal)
    {
        uint32_t result;

        uint_fast16_t fractional = ((SYSCTL->PLLFREQ0 & SYSCTL_PLLFREQ0_MFRAC_M) >> SYSCTL_PLLFREQ0_MFRAC_S);
        uint_fast16_t integer = SYSCTL->PLLFREQ0 & SYSCTL_PLLFREQ0_MINT_M;
        uint_fast8_t q = (((SYSCTL->PLLFREQ1 & SYSCTL_PLLFREQ1_Q_M) >> SYSCTL_PLLFREQ1_Q_S) + 1);
        uint_fast8_t n = (((SYSCTL->PLLFREQ1 & SYSCTL_PLLFREQ1_N_M) >> SYSCTL_PLLFREQ1_N_S) + 1);

        crystal /= static_cast<uint32_t>(n);

        uint_fast16_t f1 = fractional / 32;
        uint_fast16_t f2 = fractional - (f1 * 32);

        result = crystal * static_cast<uint32_t>(integer);
        result += (crystal * static_cast<uint32_t>(f1)) / 32;
        result += (crystal * static_cast<uint32_t>(f2)) / 1024;

        return result / static_cast<uint32_t>(q);
    }

    const uint32_t vcoFrequenciesLookupTable[MAX_VCO_ENTRIES] =
    {
        160000000, // VCO 320
        240000000, // VCO 480
    };
    // NOLINTEND

    uint32_t CrystalToIndexArray(const hal::tiva::crystalFrequency& crystalValue)
    {
        return (static_cast<uint32_t>(crystalValue) & 0x7c0) >> 6;
    }

    void SetOscillatorInternal(uint32_t& oscillator, uint32_t& oscillatorSelection, uint32_t& crystalIndex)
    {
        oscillator = 16000000;
        oscillatorSelection = SYSCTL_RSCLKCFG_OSCSRC_PIOSC | SYSCTL_RSCLKCFG_OSCSRC_PIOSC;
        crystalIndex = CrystalToIndexArray(hal::tiva::crystalFrequency::_16_MHz);
    }

    void SetOscillatorLowFrequency(uint32_t& oscillator, uint32_t& oscillatorSelection, uint32_t& crystalIndex)
    {
        oscillator = 30000;
        oscillatorSelection = SYSCTL_RSCLKCFG_OSCSRC_LFIOSC;
        crystalIndex = 0;
    }

    void SetOscillatorRtc(uint32_t& oscillator, uint32_t& oscillatorSelection, uint32_t& crystalIndex)
    {
        oscillator = 32768;
        oscillatorSelection = SYSCTL_RSCLKCFG_OSCSRC_RTC;
        crystalIndex = 0;
    }

    void SetOscillatorMain(uint32_t& oscillator, uint32_t& oscillatorSelection, uint32_t& crystalIndex)
    {
        oscillator = crystalLookupTable.at(crystalIndex);
        oscillatorSelection = SYSCTL_RSCLKCFG_OSCSRC_MOSC | SYSCTL_RSCLKCFG_PLLSRC_MOSC;

        uint32_t mainOscillatorControl = SYSCTL->MOSCCTL & ~(SYSCTL_MOSCCTL_OSCRNG | SYSCTL_MOSCCTL_PWRDN | SYSCTL_MOSCCTL_NOXTAL);

        if (crystalIndex >= (CrystalToIndexArray(hal::tiva::crystalFrequency::_10_MHz) - (CrystalToIndexArray(hal::tiva::crystalFrequency::_5_MHz))))
            mainOscillatorControl |= SYSCTL_MOSCCTL_OSCRNG;

        SYSCTL->MOSCCTL = mainOscillatorControl;

        uint32_t delay = 524288;

        while ((SYSCTL->RIS & SYSCTL_RIS_MOSCPUPRIS) == 0)
        {
            delay--;
            really_assert(delay != 0);
        }
    }

    void EnablePll(hal::tiva::systemClockVco& systemClockVco, uint32_t& frequency, uint32_t& oscillatorSelection, uint32_t& crystalIndex)
    {
        auto oscillator = crystalLookupTable.at(crystalIndex);
        auto vcoIndex = (static_cast<uint32_t>(systemClockVco) >> 24) & 7;

        really_assert(vcoIndex < MAX_VCO_ENTRIES);

        SYSCTL->MEMTIM0 = GetMemoryTimings(25000000);
        uint32_t oldRsClkCfg = SYSCTL->RSCLKCFG & ~(SYSCTL_RSCLKCFG_PSYSDIV_M | SYSCTL_RSCLKCFG_OSCSRC_M | SYSCTL_RSCLKCFG_PLLSRC_M | SYSCTL_RSCLKCFG_USEPLL);
        SYSCTL->RSCLKCFG = oldRsClkCfg | SYSCTL_RSCLKCFG_MEMTIMU;

        crystalIndex -= CrystalToIndexArray(hal::tiva::crystalFrequency::_5_MHz);
        uint32_t divisor = (vcoFrequenciesLookupTable[vcoIndex] + frequency - 1) / frequency;

        SYSCTL->RSCLKCFG |= oscillatorSelection;
        SYSCTL->PLLFREQ1 = pllFrequencyLookupTable[vcoIndex][crystalIndex][1];
        SYSCTL->PLLFREQ1 |= PLL_Q_TO_REG(divisor);
        SYSCTL->PLLFREQ0 = (pllFrequencyLookupTable[vcoIndex][crystalIndex][0] | (SYSCTL->PLLFREQ0 & SYSCTL_PLLFREQ0_PLLPWR));

        SYSCTL->MEMTIM0 = GetMemoryTimings(CalculateSystemFrequency(oscillator) / 2);

        if (SYSCTL->PLLFREQ0 & SYSCTL_PLLFREQ0_PLLPWR)
            SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_NEWFREQ;
        else
            SYSCTL->PLLFREQ0 |= SYSCTL_PLLFREQ0_PLLPWR;

        for (uint32_t timeout = 32768; timeout > 0; timeout--)
            if (SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK)
                break;

        really_assert(SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK);

        SYSCTL->RSCLKCFG |= (1 << SYSCTL_RSCLKCFG_PSYSDIV_S) | oscillatorSelection | SYSCTL_RSCLKCFG_USEPLL | SYSCTL_RSCLKCFG_MEMTIMU;
    }

    void DisablePll(uint32_t& frequency, uint32_t& oscillator, uint32_t& oscillatorSelection)
    {
        uint32_t systemClockDivisor = 0;

        SYSCTL->MEMTIM0 = GetMemoryTimings(16000000);
        SYSCTL->PLLFREQ0 &= ~SYSCTL_PLLFREQ0_PLLPWR;

        uint32_t runAndSleepConfig = SYSCTL->RSCLKCFG;
        runAndSleepConfig &= ~(SYSCTL_RSCLKCFG_OSYSDIV_M | SYSCTL_RSCLKCFG_OSCSRC_M | SYSCTL_RSCLKCFG_USEPLL);
        runAndSleepConfig |= SYSCTL_RSCLKCFG_MEMTIMU;
        SYSCTL->RSCLKCFG = runAndSleepConfig;

        if(frequency != 0)
        {
            systemClockDivisor = oscillator / frequency;

            if(systemClockDivisor != 0)
                systemClockDivisor -= 1;

            frequency = oscillator / (systemClockDivisor + 1);
        }

        SYSCTL->MEMTIM0 = GetMemoryTimings(frequency);

        runAndSleepConfig = SYSCTL->RSCLKCFG;
        runAndSleepConfig |= (systemClockDivisor << SYSCTL_RSCLKCFG_OSYSDIV_S) | oscillatorSelection;
        runAndSleepConfig |= SYSCTL_RSCLKCFG_MEMTIMU;

        SYSCTL->RSCLKCFG = runAndSleepConfig;
    }
}

namespace hal::tiva
{
    void ConfigureClock(uint32_t frequency, crystalFrequency hseValue, oscillatorSource oscSource, systemClockVco systemClockVco, bool usesPll)
    {
        uint32_t oscillator = 0;
        uint32_t oscillatorSelection = 0;
        uint32_t crystalIndex = CrystalToIndexArray(hseValue);

        SYSCTL->MOSCCTL = SYSCTL_MOSC_HIGHFREQ;

        switch (oscSource)
        {
            case oscillatorSource::internal:
                SetOscillatorInternal(oscillator, oscillatorSelection, crystalIndex);
                break;

            case oscillatorSource::lowFrequency:
                SetOscillatorLowFrequency(oscillator, oscillatorSelection, crystalIndex);
                break;

            case oscillatorSource::rtc:
                SetOscillatorRtc(oscillator, oscillatorSelection, crystalIndex);
                break;

            default: // main
                SetOscillatorMain(oscillator, oscillatorSelection, crystalIndex);
                break;
        }

        if (usesPll)
        {
            really_assert(oscSource == oscillatorSource::main || oscSource == oscillatorSource::internal);
            EnablePll(systemClockVco, frequency, oscillatorSelection, crystalIndex);
        }
        else
            DisablePll(frequency, oscillator, oscillatorSelection);

        SYSCTL->RSCLKCFG &= ~(SYSCTL_RSCLKCFG_OSCSRC_M);

        SystemCoreClock = frequency;
    }
}
