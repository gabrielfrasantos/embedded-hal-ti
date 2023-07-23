#include DEVICE_HEADER
#include "hal_tiva/tiva/Clock.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <array>

namespace
{
    // NOLINTBEGIN
    constexpr const uint32_t SYSCTL_RCC_ACG_B = 0x08000000; // Auto Clock Gating
    constexpr const uint32_t SYSCTL_RCC_SYSDIV_M = 0x07800000; // System Clock Divisor
    constexpr const uint32_t SYSCTL_RCC_SYSDIV_S = 23; // System Clock Divisor
    constexpr const uint32_t SYSCTL_RCC_USESYSDIV_B = 0x00400000; // Enable System Clock Divider
    constexpr const uint32_t SYSCTL_RCC_USEPWMDIV_B = 0x00100000; // Enable PWM Clock Divisor
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_M = 0x000E0000; // PWM Unit Clock Divisor
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_S = 17; // PWM Unit Clock Divisor
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_2_V = 0x00000000; // PWM clock /2
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_4_V = 0x00020000; // PWM clock /4
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_8_V = 0x00040000; // PWM clock /8
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_16_V = 0x00060000; // PWM clock /16
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_32_V = 0x00080000; // PWM clock /32
    constexpr const uint32_t SYSCTL_RCC_PWMDIV_64_V = 0x000A0000; // PWM clock /64
    constexpr const uint32_t SYSCTL_RCC_PWRDN_B = 0x00002000; // PLL Power Down
    constexpr const uint32_t SYSCTL_RCC_BYPASS_B = 0x00000800; // PLL Bypass
    constexpr const uint32_t SYSCTL_RCC_XTAL_M = 0x000007C0; // Crystal Value
    constexpr const uint32_t SYSCTL_RCC_XTAL_S = 6; // Crystal Value
    constexpr const uint32_t SYSCTL_RCC_XTAL_4MHZ_V = 0x00000180; // 4 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_4_09MHZ_V = 0x000001C0; // 4.096 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_4_91MHZ_V = 0x00000200; // 4.9152 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_5MHZ_V = 0x00000240; // 5 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_5_12MHZ_V = 0x00000280; // 5.12 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_6MHZ_V = 0x000002C0; // 6 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_6_14MHZ_V = 0x00000300; // 6.144 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_7_37MHZ_V = 0x00000340; // 7.3728 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_8MHZ_V = 0x00000380; // 8 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_8_19MHZ_V = 0x000003C0; // 8.192 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_10MHZ_V = 0x00000400; // 10 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_12MHZ_V = 0x00000440; // 12 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_12_2MHZ_V = 0x00000480; // 12.288 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_13_5MHZ_V = 0x000004C0; // 13.56 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_14_3MHZ_V = 0x00000500; // 14.31818 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_16MHZ_V = 0x00000540; // 16 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_16_3MHZ_V = 0x00000580; // 16.384 MHz
    constexpr const uint32_t SYSCTL_RCC_XTAL_18MHZ_V = 0x000005C0; // 18.0 MHz (USB)
    constexpr const uint32_t SYSCTL_RCC_XTAL_20MHZ_V = 0x00000600; // 20.0 MHz (USB)
    constexpr const uint32_t SYSCTL_RCC_XTAL_24MHZ_V = 0x00000640; // 24.0 MHz (USB)
    constexpr const uint32_t SYSCTL_RCC_XTAL_25MHZ_V = 0x00000680; // 25.0 MHz (USB)
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_M = 0x00000030; // Oscillator Source
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_S = 4; // Oscillator Source
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_MOSC_V = 0x00000000; // MOSC
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_IOSC_V = 0x00000010; // IOSC
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_IOSC_4_V = 0x00000020; // IOSC/4
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_LFIOSC_V = 0x00000030; // LFIOSC
    constexpr const uint32_t SYSCTL_RCC_MOSCDIS_B = 0x00000001; // Main Oscillator Disable
    constexpr const uint32_t SYSCTL_MAIN_OSC_DIS_B = 0x00000001; // Disable main oscillator

    /* Bit/Fields in Register RCC2 of Module SYSCTL                               */
    constexpr const uint32_t SYSCTL_RCC2_USERCC2_B = 0x80000000; // Use RCC2
    constexpr const uint32_t SYSCTL_RCC2_DIV400_B = 0x40000000; // Divide PLL as 400 MHz vs. 200
    constexpr const uint32_t SYSCTL_RCC2_SYSDIV2_M = 0x1F800000; // System Clock Divisor 2
    constexpr const uint32_t SYSCTL_RCC2_SYSDIV2_S = 23; // System Clock Divisor 2
    constexpr const uint32_t SYSCTL_RCC2_SYSDIV2LSB_B = 0x00400000; // Additional LSB for SYSDIV2
    constexpr const uint32_t SYSCTL_RCC2_USBPWRDN_B = 0x00004000; // Power-Down USB PLL
    constexpr const uint32_t SYSCTL_RCC2_PWRDN2_B = 0x00002000; // Power-Down PLL 2
    constexpr const uint32_t SYSCTL_RCC2_BYPASS2_B = 0x00000800; // PLL Bypass 2
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_M = 0x00000070; // Oscillator Source 2
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_S = 4; // Oscillator Source 2
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_MOSC_V = 0x00000000; // MOSC
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_IOSC_V = 0x00000010; // PIOSC
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_IOSC_4_V = 0x00000020; // PIOSC/4
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_LFIOSC_V = 0x00000030; // LFIOSC
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_HIBOSC_V = 0x00000070; // 32.768 kHz

    /* Bit/Fields in Register RIS of Module SYSCTL                                */
    constexpr const uint32_t SYSCTL_RIS_BOR0RIS_B = 0x00000800; // VDD under BOR0 Raw Interrupt
    constexpr const uint32_t SYSCTL_RIS_VDDARIS_B = 0x00000400; // VDDA Power OK Event Raw
    constexpr const uint32_t SYSCTL_RIS_MOSCPUPRIS_B = 0x00000100; // MOSC Power Up Raw Interrupt
    constexpr const uint32_t SYSCTL_RIS_USBPLLLRIS_B = 0x00000080; // USB PLL Lock Raw Interrupt
    constexpr const uint32_t SYSCTL_RIS_PLLLRIS_B = 0x00000040; // PLL Lock Raw Interrupt Status
    constexpr const uint32_t SYSCTL_RIS_MOFRIS_B = 0x00000008; // Main Oscillator Failure Raw
    constexpr const uint32_t SYSCTL_RIS_BOR1RIS_B = 0x00000002; // VDD under BOR1 Raw Interrupt

    /* Bit/Fields in Register MISC of Module SYSCTL                               */
    constexpr const uint32_t SYSCTL_MISC_BOR0MIS_B = 0x00000800; // VDD under BOR0 Masked Interrupt
    constexpr const uint32_t SYSCTL_MISC_VDDAMIS_B = 0x00000400; // VDDA Power OK Masked Interrupt
    constexpr const uint32_t SYSCTL_MISC_MOSCPUPMIS_B = 0x00000100; // MOSC Power Up Masked Interrupt
    constexpr const uint32_t SYSCTL_MISC_USBPLLLMIS_B = 0x00000080; // USB PLL Lock Masked Interrupt
    constexpr const uint32_t SYSCTL_MISC_PLLLMIS_B = 0x00000040; // PLL Lock Masked Interrupt Status
    constexpr const uint32_t SYSCTL_MISC_MOFMIS_B = 0x00000008; // Main Oscillator Failure Masked
    constexpr const uint32_t SYSCTL_MISC_BOR1MIS_B = 0x00000002; // VDD under BOR1 Masked Interrupt

    /* Bit/Fields in Register PLLFREQ0 of Module SYSCTL                           */
    constexpr const uint32_t SYSCTL_PLLFREQ0_MFRAC_M = 0x000FFC00; // PLL M Fractional Value
    constexpr const uint32_t SYSCTL_PLLFREQ0_MFRAC_S = 10; // PLL M Fractional Value
    constexpr const uint32_t SYSCTL_PLLFREQ0_MINT_M = 0x000003FF; // PLL M Integer Value
    constexpr const uint32_t SYSCTL_PLLFREQ0_MINT_S = 0; // PLL M Integer Value

    /* Bit/Fields in Register PLLFREQ1 of Module SYSCTL                           */
    constexpr const uint32_t SYSCTL_PLLFREQ1_Q_M = 0x00001F00; // PLL Q Value
    constexpr const uint32_t SYSCTL_PLLFREQ1_Q_S = 8; // PLL Q Value
    constexpr const uint32_t SYSCTL_PLLFREQ1_N_M = 0x0000001F; // PLL N Value
    constexpr const uint32_t SYSCTL_PLLFREQ1_N_S = 0; // PLL N Value

    /* Bit/Fields in Register CTL of Module HIB                                   */
    constexpr const uint32_t HIB_CTL_WRC_B = 0x80000000; // Write Complete/Capable
    constexpr const uint32_t HIB_CTL_OSCDRV_B = 0x00020000; // Oscillator Drive Capability
    constexpr const uint32_t HIB_CTL_OSCBYP_B = 0x00010000; // Oscillator Bypass
    constexpr const uint32_t HIB_CTL_VBATSEL_M = 0x00006000; // Select for Low-Battery
    constexpr const uint32_t HIB_CTL_VBATSEL_S = 13; // Select for Low-Battery
    constexpr const uint32_t HIB_CTL_VBATSEL_1_9V_V = 0x00000000; // 1.9 Volts
    constexpr const uint32_t HIB_CTL_VBATSEL_2_1V_V = 0x00002000; // 2.1 Volts (default)
    constexpr const uint32_t HIB_CTL_VBATSEL_2_3V_V = 0x00004000; // 2.3 Volts
    constexpr const uint32_t HIB_CTL_VBATSEL_2_5V_V = 0x00006000; // 2.5 Volts
    constexpr const uint32_t HIB_CTL_BATCHK_B = 0x00000400; // Check Battery Status
    constexpr const uint32_t HIB_CTL_BATWKEN_B = 0x00000200; // Wake on Low Battery
    constexpr const uint32_t HIB_CTL_VDD3ON_B = 0x00000100; // VDD Powered
    constexpr const uint32_t HIB_CTL_VABORT_B = 0x00000080; // Power Cut Abort Enable
    constexpr const uint32_t HIB_CTL_CLK32EN_B = 0x00000040; // Clocking Enable
    constexpr const uint32_t HIB_CTL_PINWEN_B = 0x00000010; // External Wake Pin Enable
    constexpr const uint32_t HIB_CTL_RTCWEN_B = 0x00000008; // RTC Wake-up Enable
    constexpr const uint32_t HIB_CTL_HIBREQ_B = 0x00000002; // Hibernation Request
    constexpr const uint32_t HIB_CTL_RTCEN_B = 0x00000001; // RTC Timer Enable

    /* Bit/Fields in Register IM of Module HIB                                    */
    constexpr const uint32_t HIB_IM_WC_B = 0x00000010; // External Write Complete/Capable
    constexpr const uint32_t HIB_IM_EXTW_B = 0x00000008; // External Wake-Up Interrupt Mask
    constexpr const uint32_t HIB_IM_LOWBAT_B = 0x00000004; // Low Battery Voltage Interrupt
    constexpr const uint32_t HIB_IM_RTCALT0_B = 0x00000001; // RTC Alert = 0 Interrupt Mask

    /* Bit/Fields in Register PLLSTAT of Module SYSCTL                            */
    constexpr const uint32_t SYSCTL_PLLSTAT_LOCK_B = 0x00000001; // PLL Lock

    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_M = 0x0000F000; // System Clock Divider
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_80 = 0x00001000; // Specifies an 80-MHz CPU clock with a PLL divider of 2.5
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_66 = 0x00002000; // Specifies a 66-MHz CPU clock with a PLL divider of 3
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_50 = 0x00003000; // Specifies a 50-MHz CPU clock with a PLL divider of 4
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_40 = 0x00004000; // Specifies a 40-MHz CPU clock with a PLL divider of 5
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_25 = 0x00007000; // Specifies a 25-MHz clock with a PLL divider of 8
    constexpr const uint32_t SYSCTL_DC1_MINSYSDIV_20 = 0x00009000; // Specifies a 20-MHz clock with a PLL divider of 10

    constexpr const uint32_t SYSCTL_RCC_OSCSRC_MAIN = 0x00000000; // MOSC
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_INT = 0x00000010; // PIOSC
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_INT4 = 0x00000020; // PIOSC/4
    constexpr const uint32_t SYSCTL_RCC_OSCSRC_30 = 0x00000030; // LFIOSC
    constexpr const uint32_t SYSCTL_RCC2_OSCSRC2_32 = 0x00000070; // 32.768 kHz

    //*****************************************************************************
    //
    // The following are values that can be passed to the SysCtlClockSet() API as
    // the ui32Config parameter.
    //
    //*****************************************************************************
    constexpr const uint32_t SYSCTL_SYSDIV_1 = 0x07800000; // Processor clock is osc/pll /1
    constexpr const uint32_t SYSCTL_SYSDIV_2 = 0x00C00000; // Processor clock is osc/pll /2
    constexpr const uint32_t SYSCTL_SYSDIV_3 = 0x01400000; // Processor clock is osc/pll /3
    constexpr const uint32_t SYSCTL_SYSDIV_4 = 0x01C00000; // Processor clock is osc/pll /4
    constexpr const uint32_t SYSCTL_SYSDIV_5 = 0x02400000; // Processor clock is osc/pll /5
    constexpr const uint32_t SYSCTL_SYSDIV_6 = 0x02C00000; // Processor clock is osc/pll /6
    constexpr const uint32_t SYSCTL_SYSDIV_7 = 0x03400000; // Processor clock is osc/pll /7
    constexpr const uint32_t SYSCTL_SYSDIV_8 = 0x03C00000; // Processor clock is osc/pll /8
    constexpr const uint32_t SYSCTL_SYSDIV_9 = 0x04400000; // Processor clock is osc/pll /9
    constexpr const uint32_t SYSCTL_SYSDIV_10 = 0x04C00000; // Processor clock is osc/pll /10
    constexpr const uint32_t SYSCTL_SYSDIV_11 = 0x05400000; // Processor clock is osc/pll /11
    constexpr const uint32_t SYSCTL_SYSDIV_12 = 0x05C00000; // Processor clock is osc/pll /12
    constexpr const uint32_t SYSCTL_SYSDIV_13 = 0x06400000; // Processor clock is osc/pll /13
    constexpr const uint32_t SYSCTL_SYSDIV_14 = 0x06C00000; // Processor clock is osc/pll /14
    constexpr const uint32_t SYSCTL_SYSDIV_15 = 0x07400000; // Processor clock is osc/pll /15
    constexpr const uint32_t SYSCTL_SYSDIV_16 = 0x07C00000; // Processor clock is osc/pll /16
    constexpr const uint32_t SYSCTL_SYSDIV_17 = 0x88400000; // Processor clock is osc/pll /17
    constexpr const uint32_t SYSCTL_SYSDIV_18 = 0x88C00000; // Processor clock is osc/pll /18
    constexpr const uint32_t SYSCTL_SYSDIV_19 = 0x89400000; // Processor clock is osc/pll /19
    constexpr const uint32_t SYSCTL_SYSDIV_20 = 0x89C00000; // Processor clock is osc/pll /20
    constexpr const uint32_t SYSCTL_SYSDIV_21 = 0x8A400000; // Processor clock is osc/pll /21
    constexpr const uint32_t SYSCTL_SYSDIV_22 = 0x8AC00000; // Processor clock is osc/pll /22
    constexpr const uint32_t SYSCTL_SYSDIV_23 = 0x8B400000; // Processor clock is osc/pll /23
    constexpr const uint32_t SYSCTL_SYSDIV_24 = 0x8BC00000; // Processor clock is osc/pll /24
    constexpr const uint32_t SYSCTL_SYSDIV_25 = 0x8C400000; // Processor clock is osc/pll /25
    constexpr const uint32_t SYSCTL_SYSDIV_26 = 0x8CC00000; // Processor clock is osc/pll /26
    constexpr const uint32_t SYSCTL_SYSDIV_27 = 0x8D400000; // Processor clock is osc/pll /27
    constexpr const uint32_t SYSCTL_SYSDIV_28 = 0x8DC00000; // Processor clock is osc/pll /28
    constexpr const uint32_t SYSCTL_SYSDIV_29 = 0x8E400000; // Processor clock is osc/pll /29
    constexpr const uint32_t SYSCTL_SYSDIV_30 = 0x8EC00000; // Processor clock is osc/pll /30
    constexpr const uint32_t SYSCTL_SYSDIV_31 = 0x8F400000; // Processor clock is osc/pll /31
    constexpr const uint32_t SYSCTL_SYSDIV_32 = 0x8FC00000; // Processor clock is osc/pll /32
    constexpr const uint32_t SYSCTL_SYSDIV_33 = 0x90400000; // Processor clock is osc/pll /33
    constexpr const uint32_t SYSCTL_SYSDIV_34 = 0x90C00000; // Processor clock is osc/pll /34
    constexpr const uint32_t SYSCTL_SYSDIV_35 = 0x91400000; // Processor clock is osc/pll /35
    constexpr const uint32_t SYSCTL_SYSDIV_36 = 0x91C00000; // Processor clock is osc/pll /36
    constexpr const uint32_t SYSCTL_SYSDIV_37 = 0x92400000; // Processor clock is osc/pll /37
    constexpr const uint32_t SYSCTL_SYSDIV_38 = 0x92C00000; // Processor clock is osc/pll /38
    constexpr const uint32_t SYSCTL_SYSDIV_39 = 0x93400000; // Processor clock is osc/pll /39
    constexpr const uint32_t SYSCTL_SYSDIV_40 = 0x93C00000; // Processor clock is osc/pll /40
    constexpr const uint32_t SYSCTL_SYSDIV_41 = 0x94400000; // Processor clock is osc/pll /41
    constexpr const uint32_t SYSCTL_SYSDIV_42 = 0x94C00000; // Processor clock is osc/pll /42
    constexpr const uint32_t SYSCTL_SYSDIV_43 = 0x95400000; // Processor clock is osc/pll /43
    constexpr const uint32_t SYSCTL_SYSDIV_44 = 0x95C00000; // Processor clock is osc/pll /44
    constexpr const uint32_t SYSCTL_SYSDIV_45 = 0x96400000; // Processor clock is osc/pll /45
    constexpr const uint32_t SYSCTL_SYSDIV_46 = 0x96C00000; // Processor clock is osc/pll /46
    constexpr const uint32_t SYSCTL_SYSDIV_47 = 0x97400000; // Processor clock is osc/pll /47
    constexpr const uint32_t SYSCTL_SYSDIV_48 = 0x97C00000; // Processor clock is osc/pll /48
    constexpr const uint32_t SYSCTL_SYSDIV_49 = 0x98400000; // Processor clock is osc/pll /49
    constexpr const uint32_t SYSCTL_SYSDIV_50 = 0x98C00000; // Processor clock is osc/pll /50
    constexpr const uint32_t SYSCTL_SYSDIV_51 = 0x99400000; // Processor clock is osc/pll /51
    constexpr const uint32_t SYSCTL_SYSDIV_52 = 0x99C00000; // Processor clock is osc/pll /52
    constexpr const uint32_t SYSCTL_SYSDIV_53 = 0x9A400000; // Processor clock is osc/pll /53
    constexpr const uint32_t SYSCTL_SYSDIV_54 = 0x9AC00000; // Processor clock is osc/pll /54
    constexpr const uint32_t SYSCTL_SYSDIV_55 = 0x9B400000; // Processor clock is osc/pll /55
    constexpr const uint32_t SYSCTL_SYSDIV_56 = 0x9BC00000; // Processor clock is osc/pll /56
    constexpr const uint32_t SYSCTL_SYSDIV_57 = 0x9C400000; // Processor clock is osc/pll /57
    constexpr const uint32_t SYSCTL_SYSDIV_58 = 0x9CC00000; // Processor clock is osc/pll /58
    constexpr const uint32_t SYSCTL_SYSDIV_59 = 0x9D400000; // Processor clock is osc/pll /59
    constexpr const uint32_t SYSCTL_SYSDIV_60 = 0x9DC00000; // Processor clock is osc/pll /60
    constexpr const uint32_t SYSCTL_SYSDIV_61 = 0x9E400000; // Processor clock is osc/pll /61
    constexpr const uint32_t SYSCTL_SYSDIV_62 = 0x9EC00000; // Processor clock is osc/pll /62
    constexpr const uint32_t SYSCTL_SYSDIV_63 = 0x9F400000; // Processor clock is osc/pll /63
    constexpr const uint32_t SYSCTL_SYSDIV_64 = 0x9FC00000; // Processor clock is osc/pll /64
    constexpr const uint32_t SYSCTL_SYSDIV_2_5 = 0xC1000000; // Processor clock is pll / 2.5
    constexpr const uint32_t SYSCTL_SYSDIV_3_5 = 0xC1800000; // Processor clock is pll / 3.5
    constexpr const uint32_t SYSCTL_SYSDIV_4_5 = 0xC2000000; // Processor clock is pll / 4.5
    constexpr const uint32_t SYSCTL_SYSDIV_5_5 = 0xC2800000; // Processor clock is pll / 5.5
    constexpr const uint32_t SYSCTL_SYSDIV_6_5 = 0xC3000000; // Processor clock is pll / 6.5
    constexpr const uint32_t SYSCTL_SYSDIV_7_5 = 0xC3800000; // Processor clock is pll / 7.5
    constexpr const uint32_t SYSCTL_SYSDIV_8_5 = 0xC4000000; // Processor clock is pll / 8.5
    constexpr const uint32_t SYSCTL_SYSDIV_9_5 = 0xC4800000; // Processor clock is pll / 9.5
    constexpr const uint32_t SYSCTL_SYSDIV_10_5 = 0xC5000000; // Processor clock is pll / 10.5
    constexpr const uint32_t SYSCTL_SYSDIV_11_5 = 0xC5800000; // Processor clock is pll / 11.5
    constexpr const uint32_t SYSCTL_SYSDIV_12_5 = 0xC6000000; // Processor clock is pll / 12.5
    constexpr const uint32_t SYSCTL_SYSDIV_13_5 = 0xC6800000; // Processor clock is pll / 13.5
    constexpr const uint32_t SYSCTL_SYSDIV_14_5 = 0xC7000000; // Processor clock is pll / 14.5
    constexpr const uint32_t SYSCTL_SYSDIV_15_5 = 0xC7800000; // Processor clock is pll / 15.5
    constexpr const uint32_t SYSCTL_SYSDIV_16_5 = 0xC8000000; // Processor clock is pll / 16.5
    constexpr const uint32_t SYSCTL_SYSDIV_17_5 = 0xC8800000; // Processor clock is pll / 17.5
    constexpr const uint32_t SYSCTL_SYSDIV_18_5 = 0xC9000000; // Processor clock is pll / 18.5
    constexpr const uint32_t SYSCTL_SYSDIV_19_5 = 0xC9800000; // Processor clock is pll / 19.5
    constexpr const uint32_t SYSCTL_SYSDIV_20_5 = 0xCA000000; // Processor clock is pll / 20.5
    constexpr const uint32_t SYSCTL_SYSDIV_21_5 = 0xCA800000; // Processor clock is pll / 21.5
    constexpr const uint32_t SYSCTL_SYSDIV_22_5 = 0xCB000000; // Processor clock is pll / 22.5
    constexpr const uint32_t SYSCTL_SYSDIV_23_5 = 0xCB800000; // Processor clock is pll / 23.5
    constexpr const uint32_t SYSCTL_SYSDIV_24_5 = 0xCC000000; // Processor clock is pll / 24.5
    constexpr const uint32_t SYSCTL_SYSDIV_25_5 = 0xCC800000; // Processor clock is pll / 25.5
    constexpr const uint32_t SYSCTL_SYSDIV_26_5 = 0xCD000000; // Processor clock is pll / 26.5
    constexpr const uint32_t SYSCTL_SYSDIV_27_5 = 0xCD800000; // Processor clock is pll / 27.5
    constexpr const uint32_t SYSCTL_SYSDIV_28_5 = 0xCE000000; // Processor clock is pll / 28.5
    constexpr const uint32_t SYSCTL_SYSDIV_29_5 = 0xCE800000; // Processor clock is pll / 29.5
    constexpr const uint32_t SYSCTL_SYSDIV_30_5 = 0xCF000000; // Processor clock is pll / 30.5
    constexpr const uint32_t SYSCTL_SYSDIV_31_5 = 0xCF800000; // Processor clock is pll / 31.5
    constexpr const uint32_t SYSCTL_SYSDIV_32_5 = 0xD0000000; // Processor clock is pll / 32.5
    constexpr const uint32_t SYSCTL_SYSDIV_33_5 = 0xD0800000; // Processor clock is pll / 33.5
    constexpr const uint32_t SYSCTL_SYSDIV_34_5 = 0xD1000000; // Processor clock is pll / 34.5
    constexpr const uint32_t SYSCTL_SYSDIV_35_5 = 0xD1800000; // Processor clock is pll / 35.5
    constexpr const uint32_t SYSCTL_SYSDIV_36_5 = 0xD2000000; // Processor clock is pll / 36.5
    constexpr const uint32_t SYSCTL_SYSDIV_37_5 = 0xD2800000; // Processor clock is pll / 37.5
    constexpr const uint32_t SYSCTL_SYSDIV_38_5 = 0xD3000000; // Processor clock is pll / 38.5
    constexpr const uint32_t SYSCTL_SYSDIV_39_5 = 0xD3800000; // Processor clock is pll / 39.5
    constexpr const uint32_t SYSCTL_SYSDIV_40_5 = 0xD4000000; // Processor clock is pll / 40.5
    constexpr const uint32_t SYSCTL_SYSDIV_41_5 = 0xD4800000; // Processor clock is pll / 41.5
    constexpr const uint32_t SYSCTL_SYSDIV_42_5 = 0xD5000000; // Processor clock is pll / 42.5
    constexpr const uint32_t SYSCTL_SYSDIV_43_5 = 0xD5800000; // Processor clock is pll / 43.5
    constexpr const uint32_t SYSCTL_SYSDIV_44_5 = 0xD6000000; // Processor clock is pll / 44.5
    constexpr const uint32_t SYSCTL_SYSDIV_45_5 = 0xD6800000; // Processor clock is pll / 45.5
    constexpr const uint32_t SYSCTL_SYSDIV_46_5 = 0xD7000000; // Processor clock is pll / 46.5
    constexpr const uint32_t SYSCTL_SYSDIV_47_5 = 0xD7800000; // Processor clock is pll / 47.5
    constexpr const uint32_t SYSCTL_SYSDIV_48_5 = 0xD8000000; // Processor clock is pll / 48.5
    constexpr const uint32_t SYSCTL_SYSDIV_49_5 = 0xD8800000; // Processor clock is pll / 49.5
    constexpr const uint32_t SYSCTL_SYSDIV_50_5 = 0xD9000000; // Processor clock is pll / 50.5
    constexpr const uint32_t SYSCTL_SYSDIV_51_5 = 0xD9800000; // Processor clock is pll / 51.5
    constexpr const uint32_t SYSCTL_SYSDIV_52_5 = 0xDA000000; // Processor clock is pll / 52.5
    constexpr const uint32_t SYSCTL_SYSDIV_53_5 = 0xDA800000; // Processor clock is pll / 53.5
    constexpr const uint32_t SYSCTL_SYSDIV_54_5 = 0xDB000000; // Processor clock is pll / 54.5
    constexpr const uint32_t SYSCTL_SYSDIV_55_5 = 0xDB800000; // Processor clock is pll / 55.5
    constexpr const uint32_t SYSCTL_SYSDIV_56_5 = 0xDC000000; // Processor clock is pll / 56.5
    constexpr const uint32_t SYSCTL_SYSDIV_57_5 = 0xDC800000; // Processor clock is pll / 57.5
    constexpr const uint32_t SYSCTL_SYSDIV_58_5 = 0xDD000000; // Processor clock is pll / 58.5
    constexpr const uint32_t SYSCTL_SYSDIV_59_5 = 0xDD800000; // Processor clock is pll / 59.5
    constexpr const uint32_t SYSCTL_SYSDIV_60_5 = 0xDE000000; // Processor clock is pll / 60.5
    constexpr const uint32_t SYSCTL_SYSDIV_61_5 = 0xDE800000; // Processor clock is pll / 61.5
    constexpr const uint32_t SYSCTL_SYSDIV_62_5 = 0xDF000000; // Processor clock is pll / 62.5
    constexpr const uint32_t SYSCTL_SYSDIV_63_5 = 0xDF800000; // Processor clock is pll / 63.5
    constexpr const uint32_t SYSCTL_CFG_VCO_480 = 0xF1000000; // VCO is 480 MHz
    constexpr const uint32_t SYSCTL_CFG_VCO_320 = 0xF0000000; // VCO is 320 MHz
    constexpr const uint32_t SYSCTL_USE_PLL = 0x00000000; // System clock is the PLL clock
    constexpr const uint32_t SYSCTL_USE_OSC = 0x00003800; // System clock is the osc clock
    constexpr const uint32_t SYSCTL_XTAL_1MHZ = 0x00000000; // External crystal is 1MHz
    constexpr const uint32_t SYSCTL_XTAL_1_84MHZ = 0x00000040; // External crystal is 1.8432MHz
    constexpr const uint32_t SYSCTL_XTAL_2MHZ = 0x00000080; // External crystal is 2MHz
    constexpr const uint32_t SYSCTL_XTAL_2_45MHZ = 0x000000C0; // External crystal is 2.4576MHz
    constexpr const uint32_t SYSCTL_XTAL_3_57MHZ = 0x00000100; // External crystal is 3.579545MHz
    constexpr const uint32_t SYSCTL_XTAL_3_68MHZ = 0x00000140; // External crystal is 3.6864MHz
    constexpr const uint32_t SYSCTL_XTAL_4MHZ = 0x00000180; // External crystal is 4MHz
    constexpr const uint32_t SYSCTL_XTAL_4_09MHZ = 0x000001C0; // External crystal is 4.096MHz
    constexpr const uint32_t SYSCTL_XTAL_4_91MHZ = 0x00000200; // External crystal is 4.9152MHz
    constexpr const uint32_t SYSCTL_XTAL_5MHZ = 0x00000240; // External crystal is 5MHz
    constexpr const uint32_t SYSCTL_XTAL_5_12MHZ = 0x00000280; // External crystal is 5.12MHz
    constexpr const uint32_t SYSCTL_XTAL_6MHZ = 0x000002C0; // External crystal is 6MHz
    constexpr const uint32_t SYSCTL_XTAL_6_14MHZ = 0x00000300; // External crystal is 6.144MHz
    constexpr const uint32_t SYSCTL_XTAL_7_37MHZ = 0x00000340; // External crystal is 7.3728MHz
    constexpr const uint32_t SYSCTL_XTAL_8MHZ = 0x00000380; // External crystal is 8MHz
    constexpr const uint32_t SYSCTL_XTAL_8_19MHZ = 0x000003C0; // External crystal is 8.192MHz
    constexpr const uint32_t SYSCTL_XTAL_10MHZ = 0x00000400; // External crystal is 10 MHz
    constexpr const uint32_t SYSCTL_XTAL_12MHZ = 0x00000440; // External crystal is 12 MHz
    constexpr const uint32_t SYSCTL_XTAL_12_2MHZ = 0x00000480; // External crystal is 12.288 MHz
    constexpr const uint32_t SYSCTL_XTAL_13_5MHZ = 0x000004C0; // External crystal is 13.56 MHz
    constexpr const uint32_t SYSCTL_XTAL_14_3MHZ = 0x00000500; // External crystal is 14.31818 MHz
    constexpr const uint32_t SYSCTL_XTAL_16MHZ = 0x00000540; // External crystal is 16 MHz
    constexpr const uint32_t SYSCTL_XTAL_16_3MHZ = 0x00000580; // External crystal is 16.384 MHz
    constexpr const uint32_t SYSCTL_XTAL_18MHZ = 0x000005C0; // External crystal is 18.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_20MHZ = 0x00000600; // External crystal is 20.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_24MHZ = 0x00000640; // External crystal is 24.0 MHz
    constexpr const uint32_t SYSCTL_XTAL_25MHZ = 0x00000680; // External crystal is 25.0 MHz
    constexpr const uint32_t SYSCTL_OSC_MAIN = 0x00000000; // Osc source is main osc
    constexpr const uint32_t SYSCTL_OSC_INT = 0x00000010; // Osc source is int. osc
    constexpr const uint32_t SYSCTL_OSC_INT4 = 0x00000020; // Osc source is int. osc /4
    constexpr const uint32_t SYSCTL_OSC_INT30 = 0x00000030; // Osc source is int. 30 KHz
    constexpr const uint32_t SYSCTL_OSC_EXT32 = 0x80000038; // Osc source is ext. 32 KHz
    constexpr const uint32_t SYSCTL_INT_OSC_DIS = 0x00000002; // Disable internal oscillator
    constexpr const uint32_t SYSCTL_MAIN_OSC_DIS = 0x00000001; // Disable main oscillator

    constexpr const uint32_t CLOCK_FREQ_PIOSC = (16000000UL); /* Internal Oscillator Frequency: 16 MHz             */
    constexpr const uint32_t CLOCK_FREQ_PIOSC_4 = (4000000UL); /* PIOSC divided by 4 : 4 MHz                        */
    constexpr const uint32_t CLOCK_FREQ_LFIOSC = (30000UL); /* Low Frequency Internal Oscillator: 30 KHz         */
    constexpr const uint32_t CLOCK_FREQ_HIBOSC = (32768UL); /* External Hibernate Module Oscillator: 32768 Hz    */
    constexpr const uint32_t CLOCK_FREQ_PLLOSC = (400000000UL); /* PLL Oscillator : 400 MHz                          */
    constexpr const uint32_t CLOCK_FREQ_PLLOSC_2 = (200000000UL); /* PLL Oscillator divided by 2 : 200 MHz             */
    constexpr const uint32_t CLOCK_FREQ_MAX = (80000000UL); /* Maximum processo clock : 80 MHz                   */

    const std::array<uint32_t, 32> crystalLookupTable = { {
        0, /*  0 */
        0, /*  1 */
        0, /*  2 */
        0, /*  3 */
        0, /*  4 */
        0, /*  5 */
        4000000, /*  6 */
        4096000, /*  7 */
        4915200, /*  8 */
        5000000, /*  9 */
        5120000, /* 10 */
        6000000, /* 11 */
        6144000, /* 12 */
        7372800, /* 13 */
        8000000, /* 14 */
        8192000, /* 15 */
        10000000, /* 16 */
        12000000, /* 17 */
        12288000, /* 18 */
        13560000, /* 19 */
        13560000, /* 20 */
        14318180, /* 21 */
        16000000, /* 22 */
        16384000, /* 23 */
        18000000, /* 24 */
        20000000, /* 25 */
        25000000, /* 26 */
        0, /* 27 */
        0, /* 28 */
        0, /* 29 */
        0, /* 30 */
        0, /* 31 */
    } };

    void BypassPllAndSystemClock(uint32_t& rcc, uint32_t& rcc2)
    {
        rcc |= SYSCTL_RCC_BYPASS_B;
        rcc &= ~(SYSCTL_RCC_USESYSDIV_B);
        rcc2 |= SYSCTL_RCC2_BYPASS2_B;

        SYSCTL->RCC = rcc;
        SYSCTL->RCC2 = rcc2;
    }

    void HandleOscillatorSource(uint32_t& rcc, uint32_t& rcc2, hal::tiva::oscillatorSource oscSource)
    {
        auto oscSourceReg = static_cast<uint32_t>(oscSource);

        if ((rcc & SYSCTL_RCC_MOSCDIS_B) != 0 && (oscSourceReg & SYSCTL_MAIN_OSC_DIS_B) == 0)
        {
            rcc &= (~SYSCTL_RCC_MOSCDIS_B | (oscSourceReg & SYSCTL_MAIN_OSC_DIS_B));
            SYSCTL->MISC = SYSCTL_MISC_MOSCPUPMIS_B;
            SYSCTL->RCC = rcc;

            uint32_t delay = 524288;

            while ((SYSCTL->RIS & SYSCTL_RIS_MOSCPUPRIS_B) == 0)
            {
                delay--;
                really_assert(delay != 0);
            }
        }
    }

    void SetCrystalAndOscillatorSource(uint32_t& rcc, uint32_t& rcc2, hal::tiva::crystalFrequency crystalValue, hal::tiva::oscillatorSource oscSource, bool useRcc2)
    {
        rcc &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M);
        rcc |= (static_cast<uint32_t>(crystalValue) << SYSCTL_RCC_XTAL_S) & (SYSCTL_RCC_XTAL_M);
        rcc |= (static_cast<uint32_t>(oscSource) << SYSCTL_RCC_OSCSRC_S) & (SYSCTL_RCC_OSCSRC_M);

        rcc2 &= ~(SYSCTL_RCC2_USERCC2_B | SYSCTL_RCC2_OSCSRC2_M);
        rcc2 |= useRcc2 << 31 & (SYSCTL_RCC2_USERCC2_B);
        rcc2 |= (static_cast<uint32_t>(oscSource) << SYSCTL_RCC2_OSCSRC2_S) & (SYSCTL_RCC2_OSCSRC2_M);

        SYSCTL->RCC = rcc;
        SYSCTL->RCC2 = rcc2;
    }

    void SetPll(uint32_t& rcc, uint32_t& rcc2, bool pllPowerDown)
    {
        rcc &= ~SYSCTL_RCC_PWRDN_B;
        rcc |= (pllPowerDown << 13) & SYSCTL_RCC_PWRDN_B;

        rcc2 &= ~SYSCTL_RCC2_PWRDN2_B;
        rcc2 |= (pllPowerDown << 13) & SYSCTL_RCC2_PWRDN2_B;

        /* Clear the PLL lock interrupt. */
        SYSCTL->MISC = SYSCTL_MISC_PLLLMIS_B;

        /* Write the new RCC value. */
        if ((rcc2 & SYSCTL_RCC2_USERCC2_B) != 0)
        {
            SYSCTL->RCC2 = rcc2;
            SYSCTL->RCC = rcc;
        }
        else
        {
            SYSCTL->RCC = rcc;
            SYSCTL->RCC2 = rcc2;
        }
    }

    void SetSystemClockDivider(uint32_t& rcc, uint32_t& rcc2, hal::tiva::systemClockDivider systemClockDivisor, bool enableDiv400)
    {
        really_assert(systemClockDivisor.integer >= 2);
        really_assert(systemClockDivisor.fractional == 0 || systemClockDivisor.fractional == 5);

        if (systemClockDivisor.fractional == 0)
            systemClockDivisor.integer--;

        rcc &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV_B | SYSCTL_RCC_MOSCDIS_B);
        rcc |= (systemClockDivisor.integer << SYSCTL_RCC_SYSDIV_S) & (SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV_B | SYSCTL_RCC_MOSCDIS_B);

        rcc2 &= ~(SYSCTL_RCC2_SYSDIV2_M);
        rcc2 |= (systemClockDivisor.integer << SYSCTL_RCC2_SYSDIV2_S) & SYSCTL_RCC2_SYSDIV2_M;

        if (enableDiv400)
        {
            rcc  |=  (SYSCTL_RCC_USESYSDIV_B);
            rcc2 &= ~(SYSCTL_RCC_USESYSDIV_B);
            rcc2 |=  SYSCTL_RCC2_DIV400_B;

            if (systemClockDivisor.fractional == 5)
                rcc2 &=~ SYSCTL_RCC2_SYSDIV2LSB_B;
            else
                rcc2 |= SYSCTL_RCC2_SYSDIV2LSB_B;
        }
        else
        {
            rcc2 &= ~(SYSCTL_RCC2_DIV400_B);
        }
    }

    void ConfigureAndEnablePll(uint32_t& rcc, uint32_t& rcc2, bool usePll)
    {
        if (usePll)
        {
            for (uint32_t i = 32768; i > 0; i--)
                if ((SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK_B) != 0)
                    break;

            rcc &= ~(SYSCTL_RCC_BYPASS_B);
            rcc2 &= ~(SYSCTL_RCC2_BYPASS2_B);
        }

        SYSCTL->RCC = rcc;
        SYSCTL->RCC2 = rcc2;
    }

    bool IsPllInUse(uint32_t& rcc, uint32_t& rcc2)
    {        
        return (((rcc2 & SYSCTL_RCC2_USERCC2_B) != 0 && (rcc2 & SYSCTL_RCC2_BYPASS2_B) == 0) || ((rcc2 & SYSCTL_RCC2_USERCC2_B) == 0 && (rcc & SYSCTL_RCC_BYPASS_B) == 0));
    }

    uint32_t OscillatorFrequency(uint32_t& rcc, uint32_t& rcc2)
    {
        switch ((rcc2 & SYSCTL_RCC2_USERCC2_B) != 0 ? (rcc2 & SYSCTL_RCC2_OSCSRC2_M) : (rcc & SYSCTL_RCC_OSCSRC_M))
        {
            case SYSCTL_RCC_OSCSRC_MAIN: return crystalLookupTable.at((((rcc & SYSCTL_RCC_XTAL_M) >> SYSCTL_RCC_XTAL_S) & 0x1F) + 1); break;
            case SYSCTL_RCC_OSCSRC_INT: return 16000000; break;
            case SYSCTL_RCC_OSCSRC_INT4: return 4000000; break;
            case SYSCTL_RCC_OSCSRC_30: return 30000; break;
            case SYSCTL_RCC2_OSCSRC2_32: return 32768; break;
            default: return 0; break;
        }
    }

    uint32_t PllFrequency(uint32_t& rcc, uint32_t& rcc2, uint32_t oscillatorFrequency)
    {
        /* Read the two PLL frequency registers.  The formula for a TM4C123 device is "(xtal * m) / ((q + 1) * (n + 1))". */
        uint32_t pll = SYSCTL->PLLFREQ0;
        uint32_t pll1 = SYSCTL->PLLFREQ1;

        /* Divide the input clock by the dividers. */
        auto pllFrequency = oscillatorFrequency / ((((pll1 & SYSCTL_PLLFREQ1_Q_M) >> SYSCTL_PLLFREQ1_Q_S) + 1) * (((pll1 & SYSCTL_PLLFREQ1_N_M) >> SYSCTL_PLLFREQ1_N_S) + 1) * 2);

        /* Multiply the clock by the multiplier, which is split into an integer part and a fractional part. */
        return ((pllFrequency * ((pll & SYSCTL_PLLFREQ0_MINT_M) >> SYSCTL_PLLFREQ0_MINT_S)) + ((pllFrequency * ((pll & SYSCTL_PLLFREQ0_MFRAC_M) >> SYSCTL_PLLFREQ0_MFRAC_S)) >> 10));
    }

    uint32_t CoreFrequency(uint32_t& rcc, uint32_t& rcc2, uint32_t inputFrequency)
    {
        if ((rcc2 & SYSCTL_RCC2_USERCC2_B) != 0)
        {
            if ((rcc2 & SYSCTL_RCC2_DIV400_B) != 0 && (((rcc2 & SYSCTL_RCC2_USERCC2_B) != 0 && (rcc2 & SYSCTL_RCC2_BYPASS2_B) == 0) || 
               ((rcc2 & SYSCTL_RCC2_USERCC2_B) == 0 && (rcc & SYSCTL_RCC_BYPASS_B) == 0)))

            {
                return ((inputFrequency * 2) / (((rcc2 & (SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB_B)) >> (SYSCTL_RCC2_SYSDIV2_S - 1)) + 1));
            }
            else
            {
                return inputFrequency / (((rcc2 & SYSCTL_RCC2_SYSDIV2_M) >> SYSCTL_RCC2_SYSDIV2_S) + 1);
            }
        }
        else
        {
            return inputFrequency / (((rcc & SYSCTL_RCC_SYSDIV_M) >> SYSCTL_RCC_SYSDIV_S) + 1);
        }
    }

    void VerifyFrequency()
    {
        uint32_t rcc = SYSCTL->RCC;
        uint32_t rcc2 = SYSCTL->RCC2;
        auto inputFrequency = OscillatorFrequency(rcc, rcc2);

        if (IsPllInUse(rcc, rcc2))
        {
            inputFrequency = PllFrequency(rcc, rcc2, inputFrequency);
        }

        SystemCoreClock = CoreFrequency(rcc, rcc2, inputFrequency);

        really_assert(SystemCoreClock == 80000000);
    }

    void Delay(uint32_t value)
    {
        while (value-- != 0) { }
    }
}

namespace hal::tiva
{
    void ConfigureClock(crystalFrequency hseValue, oscillatorSource oscSource, systemClockDivider systemClockDivisor, bool usesPll)
    {
        uint32_t rcc = SYSCTL->RCC;
        uint32_t rcc2 = SYSCTL->RCC2;

        BypassPllAndSystemClock(rcc, rcc2);
        HandleOscillatorSource(rcc, rcc2, oscSource);
        SetCrystalAndOscillatorSource(rcc, rcc2, hseValue, oscSource, true);
        SetPll(rcc, rcc2, false);
        SetSystemClockDivider(rcc, rcc2, systemClockDivisor, true);
        ConfigureAndEnablePll(rcc, rcc2, usesPll);

        Delay(18);

        VerifyFrequency();
    }
}
