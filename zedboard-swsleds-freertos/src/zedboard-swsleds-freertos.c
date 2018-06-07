/***************************** Include Files **********************************/
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xparameters.h"
#include "xgpio.h"

/************************** Constant Definitions ******************************/
/*
 * The following constants define the GPIO channel that is used for the
 * switches and the LEDs. They allow the channels to be changed easily.
 */
#define SWS_CHANNEL             1    /* Channel 1 of the GPIO Device */
#define LEDS_CHANNEL            2    /* Channel 2 of the GPIO Device */
#define DIRECTION_IN            0x07
#define DIRECTION_OUT           (~DIRECTION_IN)

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily change
 * all the needed parameters in one place.
 */
#define GPIO_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID

/*
 * Delay between invocations of the readSwsWriteLEDs() function.
 */
#define DELAY_100_MS    100UL

/******************************************************************************/
/*
 * Read status of switches and write that status directly to LEDs.
 */
static
void readSwsWriteLEDs(XGpio *gpio) {
    u32 value;

    value = XGpio_DiscreteRead(gpio, SWS_CHANNEL);
    XGpio_DiscreteWrite(gpio, LEDS_CHANNEL, value);
}

/******************************************************************************/
/*
 * Initialise the GPIO.
 */
static
int initialiseGPIO(XGpio *gpio) {
    int xst;

    xst = XGpio_Initialize(gpio, GPIO_DEVICE_ID);
    if (xst != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Set the direction for switches signals as input, and for LEDs signals as
     * output.
     */
    XGpio_SetDataDirection(gpio, SWS_CHANNEL, DIRECTION_IN);
    XGpio_SetDataDirection(gpio, LEDS_CHANNEL, DIRECTION_OUT);

    return xst;
}

/******************************************************************************/
/*
 * The task function.
 */
static
void taskFn(void *params) {

    XGpio *gpio = (XGpio *)params;

    for (;;) {
        readSwsWriteLEDs(gpio);

        /* Delay for 100 milliseconds. */
        vTaskDelay(pdMS_TO_TICKS(DELAY_100_MS));
    }
}
/******************************************************************************/

int main(void) {
    static XGpio gpio; /* The Instance of the GPIO Driver */

    int xst;
    TaskHandle_t taskHandle;

    /* Initialize the GPIO driver. Exit if an error occurs. */
    xst = initialiseGPIO(&gpio);
    if (xst != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Create a task that will read status of switches and turn on/off
     * corresponding LEDs.
     */
    xTaskCreate(
        taskFn, /* The function that implements the task. */
        "SwsLeds", /* Text name for the task. */
        configMINIMAL_STACK_SIZE, /* The stack allocated to the task. */
        &gpio, /* The task parameter. */
        tskIDLE_PRIORITY, /* The task runs at the idle priority. */
        &taskHandle
    );

    /* Start the task running. */
    vTaskStartScheduler();

    /*
     * If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle
     * and/or timer tasks to be created. 
     */
    while (1)
        ;

    return XST_SUCCESS;
}
