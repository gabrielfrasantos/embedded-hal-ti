//*****************************************************************************
//
// startup_TM4C123GH6PM.c - Startup code for use with GNU tools.
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************
#include "TIVA.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
extern void Default_Handler_Forwarded();
static void Default_Handler();
void Reset_Handler();
void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, alias("Default_Handler")));

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();
extern void _init(void);
extern void HardwareInitialization();
extern int main();

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
extern unsigned _estack;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
   (void*)&_estack, /*!< The initial stack pointer, 0x20008000 32K */
   Reset_Handler, /*!< The reset handler */
   Default_Handler, /*!< NMI_Handler,               The NMI handler */
   Default_Handler, /*!< HardFault_Handler,         The hard fault handler */
   Default_Handler, /*!< MemManage_Handler,         The MPU fault handler */
   Default_Handler, /*!< BusFault_Handler,          The bus fault handler */
   Default_Handler, /*!< UsageFault_Handler,        The usage fault handler */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   SVC_Handler, /*!< SVC_Handler,               SVCall handler */
   Default_Handler, /*!< DebugMon_Handler,          Debug monitor handler */
   0, /*!< Reserved */
   PendSV_Handler, /*!< PendSV_Handler,            The PendSV handler */
   SysTick_Handler, /*!< SysTick_Handler,           The SysTick handler */
   Default_Handler, /*!< GPIOA_Handler,             GPIO Port A */
   Default_Handler, /*!< GPIOB_Handler,             GPIO Port B */
   Default_Handler, /*!< GPIOC_Handler,             GPIO Port C */
   Default_Handler, /*!< GPIOD_Handler,             GPIO Port D */
   Default_Handler, /*!< GPIOE_Handler,             GPIO Port E */
   Default_Handler, /*!< UART0_Handler,             UART0 Rx and Tx */
   Default_Handler, /*!< UART1_Handler,             UART1 Rx and Tx */
   Default_Handler, /*!< SSI0_Handler,              SSI0 Rx and Tx */
   Default_Handler, /*!< I2C0_Handler,              I2C0 Master and Slave */
   Default_Handler, /*!< PMW0_FAULT_Handler,        PWM Fault */
   Default_Handler, /*!< PWM0_0_Handler,            PWM Generator 0 */
   Default_Handler, /*!< PWM0_1_Handler,            PWM Generator 1 */
   Default_Handler, /*!< PWM0_2_Handler,            PWM Generator 2 */
   Default_Handler, /*!< QEI0_Handler,              Quadrature Encoder 0 */
   Default_Handler, /*!< ADC0SS0_Handler,           ADC Sequence 0 */
   Default_Handler, /*!< ADC0SS1_Handler,           ADC Sequence 1 */
   Default_Handler, /*!< ADC0SS2_Handler,           ADC Sequence 2 */
   Default_Handler, /*!< ADC0SS3_Handler,           ADC Sequence 3 */
   Default_Handler, /*!< WDT_Handler,               Watchdog timer */
   Default_Handler, /*!< TIMER0A_Handler,           Timer 0 subtimer A */
   Default_Handler, /*!< TIMER0B_Handler,           Timer 0 subtimer B */
   Default_Handler, /*!< TIMER1A_Handler,           Timer 1 subtimer A */
   Default_Handler, /*!< TIMER1B_Handler,           Timer 1 subtimer B */
   Default_Handler, /*!< TIMER2A_Handler,           Timer 2 subtimer A */
   Default_Handler, /*!< TIMER2B_Handler,           Timer 2 subtimer B */
   Default_Handler, /*!< COMP0_Handler,             Analog Comparator 0 */
   Default_Handler, /*!< COMP1_Handler,             Analog Comparator 1 */
   0, /*!< Reserved */
   Default_Handler, /*!< SYSCTL_Handler,            System Control (PLL, OSC, BO) */
   Default_Handler, /*!< FLASH_Handler,             FLASH Control */
   Default_Handler, /*!< GPIOF_Handler,             GPIO Port F */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< UART2_Handler,             UART2 Rx and Tx */
   Default_Handler, /*!< SSI1_Handler,              SSI1 Rx and Tx */
   Default_Handler, /*!< TIMER3A_Handler,           Timer 3 subtimer A */
   Default_Handler, /*!< TIMER3B_Handler,           Timer 3 subtimer B */
   Default_Handler, /*!< I2C1_Handler,              I2C1 Master and Slave */
   Default_Handler, /*!< QEI1_Handler,              Quadrature Encoder 1 */
   Default_Handler, /*!< CAN0_Handler,              CAN0 */
   Default_Handler, /*!< CAN1_Handler,              CAN1 */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< HIB_Handler,               Hibernate */
   Default_Handler, /*!< USB0_Handler,              USB0 */
   Default_Handler, /*!< PWM0_3_Handler,            PWM Generator 3 */
   Default_Handler, /*!< UDMA_Handler,              uDMA Software Transfer */
   Default_Handler, /*!< UDMAERR_Handler,           uDMA Error */
   Default_Handler, /*!< ADC1SS0_Handler,           ADC1 Sequence 0 */
   Default_Handler, /*!< ADC1SS1_Handler,           ADC1 Sequence 1 */
   Default_Handler, /*!< ADC1SS2_Handler,           ADC1 Sequence 2 */
   Default_Handler, /*!< ADC1SS3_Handler,           ADC1 Sequence 3 */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< SSI2_Handler,              SSI2 Rx and Tx */
   Default_Handler, /*!< SSI3_Handler,              SSI3 Rx and Tx */
   Default_Handler, /*!< UART3_Handler,             UART3 Rx and Tx */
   Default_Handler, /*!< UART4_Handler,             UART4 Rx and Tx */
   Default_Handler, /*!< UART5_Handler,             UART5 Rx and Tx */
   Default_Handler, /*!< UART6_Handler,             UART6 Rx and Tx */
   Default_Handler, /*!< UART7_Handler,             UART7 Rx and Tx */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< I2C2_Handler                I2C2 Master and Slave */
   Default_Handler, /*!< I2C3_Handler,               I2C3 Master and Slave */
   Default_Handler, /*!< TIMER4A_Handler,            Timer 4 subtimer A */
   Default_Handler, /*!< TIMER4B_Handler,            Timer 4 subtimer B */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< TIMER5A_Handler,           Timer 5 subtimer A */
   Default_Handler, /*!< TIMER5B_Handler,           Timer 5 subtimer B */
   Default_Handler, /*!< WTIMER0A_Handler,          Wide Timer 0 subtimer A */
   Default_Handler, /*!< WTIMER0B_Handler,          Wide Timer 0 subtimer B */
   Default_Handler, /*!< WTIMER1A_Handler,          Wide Timer 1 subtimer A */
   Default_Handler, /*!< WTIMER1B_Handler,          Wide Timer 1 subtimer B */
   Default_Handler, /*!< WTIMER2A_Handler,          Wide Timer 2 subtimer A */
   Default_Handler, /*!< WTIMER2B_Handler,          Wide Timer 2 subtimer B */
   Default_Handler, /*!< WTIMER3A_Handler,          Wide Timer 3 subtimer A */
   Default_Handler, /*!< WTIMER3B_Handler,          Wide Timer 3 subtimer B */
   Default_Handler, /*!< WTIMER4A_Handler,          Wide Timer 4 subtimer A */
   Default_Handler, /*!< WTIMER4B_Handler,          Wide Timer 4 subtimer B */
   Default_Handler, /*!< WTIMER5A_Handler,          Wide Timer 5 subtimer A */
   Default_Handler, /*!< WTIMER5B_Handler,          Wide Timer 5 subtimer B */
   Default_Handler, /*!< FPU_Handler,               FPU */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< PWM1_0_Handler,           PWM 1 Generator 0 */
   Default_Handler, /*!< PWM1_1_Handler,           PWM 1 Generator 1 */
   Default_Handler, /*!< PWM1_2_Handler,           PWM 1 Generator 2 */
   Default_Handler, /*!< PWM1_3_Handler,           PWM 1 Generator 3 */
   Default_Handler, /*!< PWM1_FAULT_Handler        PWM 1 Fault */
};

#pragma GCC diagnostic pop

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void Reset_Handler()
{
    uint32_t *pui32Src, *pui32Dest;

    // Disable interrupts
    __asm volatile("cpsid i");

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pui32Src = &_etext;
    for (pui32Dest = &_data; pui32Dest < &_edata;)
    {
        *pui32Dest++ = *pui32Src++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =_bss\n"
            "    ldr     r1, =_ebss\n"
            "    mov     r2, #0\n"
            "    .thumb_func\n"
            "zero_loop:\n"
            "        cmp     r0, r1\n"
            "        it      lt\n"
            "        strlt   r2, [r0], #4\n"
            "        blt     zero_loop");

    (void)_bss;
    (void)_ebss; // get rid of unused warnings

    //
    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).  Any configuration of the floating-point unit using DriverLib
    // APIs must be done here prior to the floating-point unit being enabled.
    //
    // Note that this does not use DriverLib since it might not be included in
    // this project.
    //
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));

    __asm volatile("cpsie i");

    //
    // Call the hardware's initialization function.
    //
    HardwareInitialization();

    //
    // Call the application's entry point.
    //
    main();
}

void __libc_init_local(void)
{
    size_t count;
    size_t i;

    count = (size_t)(__preinit_array_end - __preinit_array_start);
    for (i = 0; i < count; i++)
        __preinit_array_start[i]();

#ifdef HAVE_INIT_FINI
    _init();
#endif

    count = (size_t)(__init_array_end - __init_array_start);
    for (i = 0; i < count; i++)
        __init_array_start[i]();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void Default_Handler()
{
    Default_Handler_Forwarded();
}
