#if IS_IRQ_ON
/***************************** Include Files *********************************/
#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"

/************************** Constant Definitions *****************************/
#define LEDS_MASK   0x07
#define SWS_MASK    0x07

#define DELAY       10000000

/************************** Variable Definitions *****************************/

/*
 * The following are declared globally so they are zeroed and so they are
 * easily accessible from a debugger
 */

XGpio GpioLeds; /* The Instance of the GPIO Driver for LEDs (LD0 - LD7) */
XGpio GpioSws; /* The Instance of the GPIO Driver for switches (SW0 - SW7) */

/*****************************************************************************/

/**
 *
 * The purpose of this function is to illustrate how to use the GPIO driver to
 * read status of switches and turn on and off a corresponding LED.
 *
 * The code of this function is largely based on an example from the
 * xgpio_example.c file.
 *
 * @return
 *   XST_FAILURE to indicate that the GPIO Initialization had failed.
 *
 * @note
 *   This function does not return if the test is running.
 */
int main(void)
{
	int status;
	volatile int delay;

	/* Initialize the GPIO driver for LEDs. */
	status = XGpio_Initialize(&GpioLeds, XPAR_LEDS_GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	/* Initialize the GPIO driver for switches. */
	status = XGpio_Initialize(&GpioSws, XPAR_SWS_GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	/*
	 * Set the direction for switches signals as input, and for LEDs signals as
	 * output.
	 */
	XGpio_SetDataDirection(&GpioLeds, 1, ~LEDS_MASK);
	XGpio_SetDataDirection(&GpioSws, 1, SWS_MASK);

	while (1) {
		u32 sws_status;

		/* Read status of switches and write that status directly to LEDs. */
		sws_status = XGpio_DiscreteRead(&GpioSws, 1);
		XGpio_DiscreteWrite(&GpioLeds, 1, sws_status);

		/* Wait a small amount of time before next polling. */
		for (delay = 0; delay < DELAY; delay++);
	}

	xil_printf("Successfully ran Gpio Example\r\n");
	return XST_SUCCESS;
}
#endif
