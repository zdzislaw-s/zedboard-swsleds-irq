#define IS_IRQ_ON 1

/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xgpio.h"
#include "xil_exception.h"
#include "xscugic.h"

/************************** Constant Definitions ******************************/
/*
 * The following constants define the GPIO channel that is used for the switches
 * and the LEDs. They allow the channels to be changed easily.
 */
#define SWS_CHANNEL             1    /* Channel 1 of the GPIO Device */
#define LEDS_CHANNEL            2    /* Channel 2 of the GPIO Device */
#define DIRECTION_IN            0x07
#define DIRECTION_OUT           (~DIRECTION_IN)

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define GPIO_DEVICE_ID          XPAR_GPIO_0_DEVICE_ID

#if IS_IRQ_ON
#define INTC_GPIO_INTERRUPT_ID  XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID

/*
 * GPIO channel mask that is needed by the Interrupt Handler.
 */
#if SWS_CHANNEL == 1
#define IR_CHANNEL_MASK            XGPIO_IR_CH1_MASK
#else
#error "Configure the interrupts mask according to the source of interrupts."
#endif /* SWS_CHANNEL */
#endif /* IS_IRQ_ON */

/******************************************************************************/
/*
 * Read status of switches and write that status directly to LEDs.
 */
static void readSwsWriteLEDs(XGpio *gpio) {
    u32 value;

    value = XGpio_DiscreteRead(gpio, SWS_CHANNEL);
    XGpio_DiscreteWrite(gpio, LEDS_CHANNEL, value);
}

#if IS_IRQ_ON
/******************************************************************************/
/*
 * This is the interrupt handler routine for the GPIO.
 */
static void interruptHandler(void *data) {
    XGpio *gpio = (XGpio *) data;

    readSwsWriteLEDs(gpio);

    /* Clear the Interrupt */
    XGpio_InterruptClear(gpio, IR_CHANNEL_MASK);
}
#endif /* IS_IRQ_ON */

#if IS_IRQ_ON
/******************************************************************************/
/*
 * This function performs the GPIO set up for interrupts.
 */
static int enableInterrupts(XScuGic *gic, XGpio *gpio) {
    int xst;
    XScuGic_Config *gicConfig;

    /*
     * Initialize the interrupt controller driver so that it is ready to
     * use.
     */
    gicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == gicConfig) {
        return XST_FAILURE;
    }

    xst = XScuGic_CfgInitialize(gic, gicConfig, gicConfig->CpuBaseAddress);
    if (xst != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Priority of the interrupts is set to: 0xA0
     * and they are rising edge sensitive (0x3) at the IRQ source.
     */
    XScuGic_SetPriorityTriggerType(gic, INTC_GPIO_INTERRUPT_ID, 0xA0, 0x3);

    /*
     * Connect the interrupt handler that will be called when an
     * interrupt occurs for the device.
     */
    xst = XScuGic_Connect(
        gic,
        INTC_GPIO_INTERRUPT_ID,
        (Xil_ExceptionHandler) interruptHandler,
        gpio
    );
    if (xst != XST_SUCCESS) {
        return xst;
    }

    /* Enable the interrupt for the GPIO device.*/
    XScuGic_Enable(gic, INTC_GPIO_INTERRUPT_ID);

    /*
     * Enable the GPIO channel interrupts so that switch can be detected and
     * enable interrupts for the GPIO device.
     */
    XGpio_InterruptEnable(gpio, IR_CHANNEL_MASK);
    XGpio_InterruptGlobalEnable(gpio);

    /*
     * Initialize the exception table and register the interrupt
     * controller handler with the exception table
     */
    Xil_ExceptionInit();

    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT,
        (Xil_ExceptionHandler) XScuGic_InterruptHandler,
        gic
    );

    /* Enable non-critical exceptions */
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}
#endif /* IS_IRQ_ON */

#if IS_IRQ_ON
/******************************************************************************/
/*
 * This function disables the interrupts for the GPIO.
 */
static void disableInterrupts(XScuGic *gic, XGpio *gpio) {
    XGpio_InterruptDisable(gpio, IR_CHANNEL_MASK);
    /* Disconnect the interrupt */
    XScuGic_Disable(gic, INTC_GPIO_INTERRUPT_ID);
    XScuGic_Disconnect(gic, INTC_GPIO_INTERRUPT_ID);
}
#endif /* IS_IRQ_ON */

/******************************************************************************/
/*
 * Initialise the GPIO.
 */
static int initialiseGPIO(XGpio *gpio) {
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
/**
 * The purpose of this function is to illustrate how to use the GPIO driver to
 * read status of switches and turn on and off a corresponding LED.
 *
 * This function is initialises the GPIO device, sets up interrupts and loops
 * in the foreground such that interrupts can occur in the background.
 *
 * The code of this function is largely based on an example from the
 * xgpio_intr_tapp_example.c file.
 *
 * @return
 *   XST_FAILURE to indicate that the GPIO Initialization had failed.
 *
 * @note
 *   This function returns only on error.
 */
int main(void) {
    int xst;

    static XGpio gpio; /* The Instance of the GPIO Driver */
#if IS_IRQ_ON
    static XScuGic gic; /* The Instance of the Interrupt Controller Driver */
#endif /* IS_IRQ_ON */

    /* Initialize the GPIO driver. Exit if an error occurs. */
    xst = initialiseGPIO(&gpio);
    if (xst != XST_SUCCESS) {
        return XST_FAILURE;
    }

#if IS_IRQ_ON
    xst = enableInterrupts(&gic, &gpio);
    if (xst != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Before waiting for interrupts to occur, show initial status of switches.
     */
    readSwsWriteLEDs(&gpio);
#endif /* IS_IRQ_ON */

    /*
     * Loop for ever.
     */
    while (1) {
#if IS_IRQ_ON
        ;
#else
        readSwsWriteLEDs(&gpio);
#endif /* IS_IRQ_ON */
    }

#if IS_IRQ_ON
    /*
     * Pedantry.
     */
    disableInterrupts(&gic, &gpio);
#endif /* IS_IRQ_ON */

    return XST_SUCCESS;
}
