
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "platform.h"
#include "xparameters.h"
#include "xstatus.h"
#include "microblaze_sleep.h"
#include "nexys4IO.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "CameraIP.h"

/************************** Constant Definitions ****************************/

// Clock frequencies
#define CPU_CLOCK_FREQ_HZ		XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define AXI_CLOCK_FREQ_HZ		XPAR_CPU_M_AXI_DP_FREQ_HZ

// AXI timer parameters
#define AXI_TIMER_DEVICE_ID		XPAR_AXI_TIMER_0_DEVICE_ID
#define AXI_TIMER_BASEADDR		XPAR_AXI_TIMER_0_BASEADDR
#define AXI_TIMER_HIGHADDR		XPAR_AXI_TIMER_0_HIGHADDR
#define TmrCtrNumber			0

// Definitions for peripheral NEXYS4IO
#define NX4IO_DEVICE_ID		XPAR_NEXYS4IO_0_DEVICE_ID
#define NX4IO_BASEADDR		XPAR_NEXYS4IO_0_S00_AXI_BASEADDR
#define NX4IO_HIGHADDR		XPAR_NEXYS4IO_0_S00_AXI_HIGHADDR

// Definition for Camera IP
#define OV7670_DEVICE_ID	XPAR_CAMERAIP_0_DEVICE_ID
#define OV7670_BASEADDR		XPAR_CAMERAIP_0_S00_AXI_BASEADDR
#define OV7670_HIGHADDR		XPAR_CAMERAIP_0_S00_AXI_HIGHADDR

// Fixed Interval timer - 100 MHz input clock, 40KHz output clock
// FIT_COUNT_1MSEC = FIT_CLOCK_FREQ_HZ * .001
#define FIT_IN_CLOCK_FREQ_HZ	CPU_CLOCK_FREQ_HZ
#define FIT_CLOCK_FREQ_HZ		40000
#define FIT_COUNT				(FIT_IN_CLOCK_FREQ_HZ / FIT_CLOCK_FREQ_HZ)
#define FIT_COUNT_1MSEC			40

// Interrupt Controller parameters
#define INTC_DEVICE_ID			XPAR_INTC_0_DEVICE_ID
#define FIT_INTERRUPT_ID		XPAR_MICROBLAZE_0_AXI_INTC_FIT_TIMER_0_INTERRUPT_INTR

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/
// Microblaze peripheral instances
uint64_t 	timestamp = 0L;
XIntc 		IntrptCtlrInst;				// Interrupt Controller instance
XTmrCtr		AXITimerInst;				// PWM timer instance


// The following variables are shared between non-interrupt processing and
// interrupt processing such that they must be global(and declared volatile)
// These variables are controlled by the FIT timer interrupt handler


volatile u32			gpio_in;			// GPIO input port


/************************** Function Prototypes *****************************/
void PMDIO_itoa(int32_t value, char *string, int32_t radix);
int	 do_init(void);											// initialize system
void FIT_Handler(void);										// fixed interval timer interrupt handler
int AXI_Timer_initialize(void);

void RunTest1(void);

/************************** MAIN PROGRAM ************************************/
int main(void)
{
    init_platform();

	uint32_t sts;

	sts = do_init();
	if (XST_SUCCESS != sts)
	{
		exit(1);
	}

	microblaze_enable_interrupts();

	// Run an iteration of the test
	RunTest1();
	
	while (1)
	{

		// check whether the center button is pressed.  If it is then
		// exit the loop.
		if (NX4IO_isPressed(BTNC))
		{
			break;
		}
		else
		{
			// increment the timestamp and delay 100 msecs
			usleep(100 * 1000);
		}
	}

	//NX4IO_setLEDs(0x0000);

	usleep(5000 * 1000);
	
	// cleanup and exit
    cleanup_platform();
    exit(0);
}

/************************ TEST FUNCTIONS ************************************/

/****************************************************************************/
/**
* Test 1 - Test the LEDS (LD15..LD0)
*
* Checks the functionality of the LEDs API with some constant patterns and
* a sliding patter.  Determine Pass/Fail by observing the LEDs on the Nexys4
*
* @param	*NONE*
*
* @return	*NONE*
*
*****************************************************************************/
void RunTest1(void)
{
	OV7670_setup(0x1280);
	OV7670_setup(0x1280);
	OV7670_setup(0x1204);
	OV7670_setup(0x1100);
	OV7670_setup(0x0c00);
	OV7670_setup(0x3e00);
	OV7670_setup(0x8c00);
	OV7670_setup(0x0400);
	OV7670_setup(0x40d0);
	OV7670_setup(0x3a04);
	OV7670_setup(0x1418);
	OV7670_setup(0x4f40);
	OV7670_setup(0x5034);
	OV7670_setup(0x510c);
	OV7670_setup(0x5217);
	OV7670_setup(0x5329);
	OV7670_setup(0x5440);
	OV7670_setup(0x581e);
	OV7670_setup(0x3dc0);
	OV7670_setup(0x1100);
	OV7670_setup(0x1711);
	OV7670_setup(0x1861);
	OV7670_setup(0x32a4);
	OV7670_setup(0x1903);
	OV7670_setup(0x1a7b);
	OV7670_setup(0x030a);
	OV7670_setup(0x030a);
	OV7670_setup(0x1e00);
	OV7670_setup(0x330b);
	OV7670_setup(0x3c78);
	OV7670_setup(0x6900);
	OV7670_setup(0x7400);
	OV7670_setup(0xb084);
	OV7670_setup(0xb10c);
	OV7670_setup(0xb20e);
	OV7670_setup(0xb380);

	OV7670_setup(0x703a);
	OV7670_setup(0x7135);
	OV7670_setup(0x7211);
	OV7670_setup(0x73f0);
	OV7670_setup(0xa202);

	OV7670_setup(0x7a20);
	OV7670_setup(0x7b10);
	OV7670_setup(0x7c1e);
	OV7670_setup(0x7d35);
	OV7670_setup(0x7e5a);
	OV7670_setup(0x7f69);
	OV7670_setup(0x8076);
	OV7670_setup(0x8180);
	OV7670_setup(0x8288);
	OV7670_setup(0x838f);
	OV7670_setup(0x8496);
	OV7670_setup(0x85a3);
	OV7670_setup(0x86af);
	OV7670_setup(0x87c4);
	OV7670_setup(0x88d7);
	OV7670_setup(0x89e8);

	OV7670_setup(0x13e0);
	OV7670_setup(0x0000);
	OV7670_setup(0x1000);
	OV7670_setup(0x0d40);
	OV7670_setup(0x1418);
	OV7670_setup(0xa505);
	OV7670_setup(0xab07);
	OV7670_setup(0x2495);
	OV7670_setup(0x2533);
	OV7670_setup(0x26e3);
	OV7670_setup(0x9f78);
	OV7670_setup(0xa068);
	OV7670_setup(0xa103);
	OV7670_setup(0xa6d8);
	OV7670_setup(0xa7d8);
	OV7670_setup(0xa8f0);
	OV7670_setup(0xa990);
	OV7670_setup(0xaa94);
	OV7670_setup(0x13e5);
	OV7670_setup(0x13e5);
	OV7670_setup(0x13e5);
	OV7670_setup(0xffff);
}

/**************************** HELPER FUNCTIONS ******************************/

/****************************************************************************/
/**
* initialize the system
*
* This function is executed once at start-up and after resets.  It initializes
* the peripherals and registers the interrupt handler(s)
*****************************************************************************/

int	 do_init(void)
{
	uint32_t status;				// status from Xilinx Lib calls

	// initialize the Nexys4 driver and (some of)the devices
	status = (uint32_t) NX4IO_initialize(NX4IO_BASEADDR);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	status = (uint32_t) OV7670_initialize(OV7670_BASEADDR);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	status = AXI_Timer_initialize();
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// initialize the interrupt controller
	status = XIntc_Initialize(&IntrptCtlrInst, INTC_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
	   return XST_FAILURE;
	}
	
	// connect the fixed interval timer (FIT) handler to the interrupt
	status = XIntc_Connect(&IntrptCtlrInst, FIT_INTERRUPT_ID,
						   (XInterruptHandler)FIT_Handler,
						   (void *)0);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;

	}

	// start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts.
	status = XIntc_Start(&IntrptCtlrInst, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// enable the FIT interrupt
	XIntc_Enable(&IntrptCtlrInst, FIT_INTERRUPT_ID);
	return XST_SUCCESS;
}
/*
 * AXI timer initializes it to generate out a 4Khz signal, Which is given to the Nexys4IO module as clock input.
 * DO NOT MODIFY
 */
int AXI_Timer_initialize(void){

	uint32_t status;				// status from Xilinx Lib calls
	u32		ctlsts;		// control/status register or mask

	status = XTmrCtr_Initialize(&AXITimerInst,AXI_TIMER_DEVICE_ID);
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	status = XTmrCtr_SelfTest(&AXITimerInst, TmrCtrNumber);
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	ctlsts = XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_EXT_GENERATE_MASK | XTC_CSR_LOAD_MASK |XTC_CSR_DOWN_COUNT_MASK ;
	XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TmrCtrNumber,ctlsts);

	//Set the value that is loaded into the timer counter and cause it to be loaded into the timer counter
	XTmrCtr_SetLoadReg(AXI_TIMER_BASEADDR, TmrCtrNumber, 24998);
	XTmrCtr_LoadTimerCounterReg(AXI_TIMER_BASEADDR, TmrCtrNumber);
	ctlsts = XTmrCtr_GetControlStatusReg(AXI_TIMER_BASEADDR, TmrCtrNumber);
	ctlsts &= (~XTC_CSR_LOAD_MASK);
	XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TmrCtrNumber, ctlsts);

	ctlsts = XTmrCtr_GetControlStatusReg(AXI_TIMER_BASEADDR, TmrCtrNumber);
	ctlsts |= XTC_CSR_ENABLE_TMR_MASK;
	XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TmrCtrNumber, ctlsts);

	XTmrCtr_Enable(AXI_TIMER_BASEADDR, TmrCtrNumber);
	return XST_SUCCESS;

}

/*********************** DISPLAY-RELATED FUNCTIONS ***********************************/

/****************************************************************************/
/**
* Converts an integer to ASCII characters
*
* algorithm borrowed from ReactOS system libraries
*
* Converts an integer to ASCII in the specified base.  Assumes string[] is
* long enough to hold the result plus the terminating null
*
* @param 	value is the integer to convert
* @param 	*string is a pointer to a buffer large enough to hold the converted number plus
*  			the terminating null
* @param	radix is the base to use in conversion, 
*
* @return  *NONE*
*
* @note
* No size check is done on the return string size.  Make sure you leave room
* for the full string plus the terminating null in string
*****************************************************************************/
void PMDIO_itoa(int32_t value, char *string, int32_t radix)
{
	char tmp[33];
	char *tp = tmp;
	int32_t i;
	uint32_t v;
	int32_t  sign;
	char *sp;

	if (radix > 36 || radix <= 1)
	{
		return;
	}

	sign = ((10 == radix) && (value < 0));
	if (sign)
	{
		v = -value;
	}
	else
	{
		v = (uint32_t) value;
	}
	
  	while (v || tp == tmp)
  	{
		i = v % radix;
		v = v / radix;
		if (i < 10)
		{
			*tp++ = i+'0';
		}
		else
		{
			*tp++ = i + 'a' - 10;
		}
	}
	sp = string;
	
	if (sign)
		*sp++ = '-';

	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	
  	return;
}



/**************************** INTERRUPT HANDLERS ******************************/

/****************************************************************************/
/**
* Fixed interval timer interrupt handler
*
* Reads the GPIO port which reads back the hardware generated PWM wave for the RGB Leds
*
* @note
* ECE 544 students - When you implement your software solution for pulse width detection in
* Project 1 this could be a reasonable place to do that processing.
 *****************************************************************************/

void FIT_Handler(void)
{

}

