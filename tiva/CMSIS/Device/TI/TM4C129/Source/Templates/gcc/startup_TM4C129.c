//*****************************************************************************
//
// startup_TM4C129.c - Startup code for use with GNU tools.
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
extern void __libc_init_local();
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
   Default_Handler, /*!< GPIO Port A */
   Default_Handler, /*!< GPIO Port B */
   Default_Handler, /*!< GPIO Port C */
   Default_Handler, /*!< GPIO Port D */
   Default_Handler, /*!< GPIO Port E */
   Default_Handler, /*!< UART0 Rx and Tx */
   Default_Handler, /*!< UART1 Rx and Tx */
   Default_Handler, /*!< SSI0 Rx and Tx */
   Default_Handler, /*!< I2C0 Master and Slave */
   Default_Handler, /*!< PWM Fault */
   Default_Handler, /*!< PWM Generator 0 */
   Default_Handler, /*!< PWM Generator 1 */
   Default_Handler, /*!< PWM Generator 2 */
   Default_Handler, /*!< Quadrature Encoder 0 */
   Default_Handler, /*!< ADC Sequence 0 */
   Default_Handler, /*!< ADC Sequence 1 */
   Default_Handler, /*!< ADC Sequence 2 */
   Default_Handler, /*!< ADC Sequence 3 */
   Default_Handler, /*!< Watchdog timer */
   Default_Handler, /*!< Timer 0 subtimer A */
   Default_Handler, /*!< Timer 0 subtimer B */
   Default_Handler, /*!< Timer 1 subtimer A */
   Default_Handler, /*!< Timer 1 subtimer B */
   Default_Handler, /*!< Timer 2 subtimer A */
   Default_Handler, /*!< Timer 2 subtimer B */
   Default_Handler, /*!< Analog Comparator 0 */
   Default_Handler, /*!< Analog Comparator 1 */
   Default_Handler, /*!< Analog Comparator 2 */
   Default_Handler, /*!< System Control (PLL, OSC, BO) */
   Default_Handler, /*!< FLASH Control */
   Default_Handler, /*!< GPIO Port F */
   Default_Handler, /*!< GPIO Port G */
   Default_Handler, /*!< GPIO Port H */
   Default_Handler, /*!< UART2 Rx and Tx */
   Default_Handler, /*!< SSI1 Rx and Tx */
   Default_Handler, /*!< Timer 3 subtimer A */
   Default_Handler, /*!< Timer 3 subtimer B */
   Default_Handler, /*!< I2C1 Master and Slave */
   Default_Handler, /*!< CAN0 */
   Default_Handler, /*!< CAN1 */
   Default_Handler, /*!< Ethernet */
   Default_Handler, /*!< Hibernate */
   Default_Handler, /*!< USB0 */
   Default_Handler, /*!< PWM Generator 3 */
   Default_Handler, /*!< uDMA Software Transfer */
   Default_Handler, /*!< uDMA Error */
   Default_Handler, /*!< ADC1 Sequence 0 */
   Default_Handler, /*!< ADC1 Sequence 1 */
   Default_Handler, /*!< ADC1 Sequence 2 */
   Default_Handler, /*!< ADC1 Sequence 3 */
   Default_Handler, /*!< External Bus Interface 0 */
   Default_Handler, /*!< GPIO Port J */
   Default_Handler, /*!< GPIO Port K */
   Default_Handler, /*!< GPIO Port L */
   Default_Handler, /*!< SSI2 Rx and Tx */
   Default_Handler, /*!< SSI3 Rx and Tx */
   Default_Handler, /*!< UART3 Rx and Tx */
   Default_Handler, /*!< UART4 Rx and Tx */
   Default_Handler, /*!< UART5 Rx and Tx */
   Default_Handler, /*!< UART6 Rx and Tx */
   Default_Handler, /*!< UART7 Rx and Tx */
   Default_Handler, /*!< I2C2 Master and Slave */
   Default_Handler, /*!< I2C3 Master and Slave */
   Default_Handler, /*!< Timer 4 subtimer A */
   Default_Handler, /*!< Timer 4 subtimer B */
   Default_Handler, /*!< Timer 5 subtimer A */
   Default_Handler, /*!< Timer 5 subtimer B */
   Default_Handler, /*!< FPU */
   0, /*!< Reserved */
   0, /*!< Reserved */
   Default_Handler, /*!< I2C4 Master and Slave */
   Default_Handler, /*!< I2C5 Master and Slave */
   Default_Handler, /*!< GPIO Port M */
   Default_Handler, /*!< GPIO Port N */
   0, /*!< Reserved */
   Default_Handler, /*!< Tamper */
   Default_Handler, /*!< GPIO Port P (Summary or P0) */
   Default_Handler, /*!< GPIO Port P1 */
   Default_Handler, /*!< GPIO Port P2 */
   Default_Handler, /*!< GPIO Port P3 */
   Default_Handler, /*!< GPIO Port P4 */
   Default_Handler, /*!< GPIO Port P5 */
   Default_Handler, /*!< GPIO Port P6 */
   Default_Handler, /*!< GPIO Port P7 */
   Default_Handler, /*!< GPIO Port Q (Summary or Q0) */
   Default_Handler, /*!< GPIO Port Q1 */
   Default_Handler, /*!< GPIO Port Q2 */
   Default_Handler, /*!< GPIO Port Q3 */
   Default_Handler, /*!< GPIO Port Q4 */
   Default_Handler, /*!< GPIO Port Q5 */
   Default_Handler, /*!< GPIO Port Q6 */
   Default_Handler, /*!< GPIO Port Q7 */
   Default_Handler, /*!< GPIO Port R */
   Default_Handler, /*!< GPIO Port S */
   Default_Handler, /*!< SHA/MD5 0 */
   Default_Handler, /*!< AES 0 */
   Default_Handler, /*!< DES3DES 0 */
   Default_Handler, /*!< LCD Controller 0 */
   Default_Handler, /*!< Timer 6 subtimer A */
   Default_Handler, /*!< Timer 6 subtimer B */
   Default_Handler, /*!< Timer 7 subtimer A */
   Default_Handler, /*!< Timer 7 subtimer B */
   Default_Handler, /*!< I2C6 Master and Slave */
   Default_Handler, /*!< I2C7 Master and Slave */
   Default_Handler, /*!< HIM Scan Matrix Keyboard 0 */
   Default_Handler, /*!< One Wire 0 */
   Default_Handler, /*!< HIM PS/2 0 */
   Default_Handler, /*!< HIM LED Sequencer 0 */
   Default_Handler, /*!< HIM Consumer IR 0 */
   Default_Handler, /*!< I2C8 Master and Slave */
   Default_Handler, /*!< I2C9 Master and Slave */
   Default_Handler, /*!< GPIO Port T */
   Default_Handler, /*!< Fan 1 */
   0, /*!< Reserved */
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
    // C/C++ objects, sections .fini, .fini_array and .dtors.
    //
    __libc_init_local();

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
