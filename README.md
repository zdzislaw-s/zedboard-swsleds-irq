# zedboard-swsleds-irq
## Control state of LEDs (LD0-LD7) with switches (SW0-SW7)

In order to make use of content of this repository you need to:
1. Have ZedBoard Rev. D
1. Have Xilinx SDK installed. (For this version, I worked with with Xilinx SDK 2018.1)
1. You don’t need Vivado, as output from Vivado is committed here.

### Design
The design of hardware is documented in the file zedboard_swsleds_irq.pdf. In a nutshell, the GPIO IP (axi_gpio_0) is configured for a dual-channel mode where the channel 1 is connected to the DIP switches and the channel 2 is connected to the LEDs. The GPIO IP has interrupts enabled (axi_gpio_0/ip2intc_irpt) and hooked up to ZYNQ (processing_system7_0/IRQ_F2P[0:0]).

### Installation
After cloning the repository you need to:
1. Start Xilinx SDK, and
1. Import the Eclipse projects (File > Open Projects from File System...) from: zedboard-swsleds-irq, zedboard-swsleds-irq_bsp and zedboard_swsleds_irq_hw_platform_0 directories.

### Building
Build the project zedboard-swsleds-irq with Project > Build Project.

Note: The source file zedboard-swsleds-irq/src/zedboard-swsleds-irq.c has on in its first line the following #define:
```c
#define IS_IRQ_ON 1
```
This define controls operation of the software - for IS_IRQ_ON = 1 the interrupts are enabled, for IS_IRQ_ON = 0 the interrupts are disabled and the LEDs are turned off/on in a polling mode.

### Running
1. Program your FPGA (Xilinx > Program FPGA) with the pre-built image zedboard_swsleds_irq.bit (the blue LED Done should light up).
1. Deploy the .elf file built in the Building step to the board (Run As > Launch on Hardware (GDB))
1. You should be able to turn the LEDs on and off with the DIP switches.
