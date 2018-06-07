# zedboard-swsleds-irq
## Control state of LEDs (LD0-LD7) with switches (SW0-SW7)

In order to make use of content of this repository you need to:
1. Have ZedBoard Rev. D
1. Have Xilinx SDK installed. (For this version, I worked with with Xilinx SDK 2018.1)
1. You don’t need Vivado, as output from Vivado is committed here.

### Design
The design of hardware is documented in [`zedboard_swsleds_irq.pdf`](https://github.com/zdzislaw-s/zedboard-swsleds-irq/blob/master/zedboard_swsleds_irq.pdf). In a nutshell, the GPIO IP (axi_gpio_0) is configured for a dual-channel mode where channel 1 is connected to the DIP switches and channel 2 is connected to the LEDs. The GPIO IP has interrupts enabled (_axi_gpio_0/ip2intc_irpt_) and hooked up to ZYNQ (_processing_system7_0/IRQ_F2P[0:0]_).

### Installation
After cloning the repository you need to:
1. Start Xilinx SDK, and
1. Import the Eclipse projects with **File > Open Projects from File System...** from:  `zedboard_swsleds_irq_hw_platform_0`, `zedboard-swsleds-irq_bsp`, `zedboard-swsleds-irq`, `zedboard-swsleds-freertos_bsp` and `zedboard-swsleds-freertos` directories.

### Building
Build the project _zedboard-swsleds-irq_ and/or _zedboard-swsleds-freertos_ with **Project > Build Project**.

Note 1: As indicated by its name, the project _zedboard-swsleds-freertos_ uses FreeRTOS in order achieve the required functionality.

Note 2: In the project _zedboard-swsleds-irq_ , the source file [`zedboard-swsleds-irq/src/zedboard-swsleds-irq.c`](https://github.com/zdzislaw-s/zedboard-swsleds-irq/blob/master/zedboard-swsleds-irq/src/zedboard-swsleds-irq.c) has on in its first line the following #define:
```c
#define IS_IRQ_ON 1
```
This #define controls operation of the software - for `IS_IRQ_ON == 1` the interrupts are enabled, for `IS_IRQ_ON == 0` the interrupts are disabled and the LEDs are turned off/on in a polling mode.

### Running
1. Program your FPGA with **Xilinx > Program FPGA** with the pre-built image [`zedboard_swsleds_irq.bit`](https://github.com/zdzislaw-s/zedboard-swsleds-irq/blob/master/zedboard_swsleds_irq.bit) (the blue LED Done should light up).
1. Deploy the resultant `zedboard-swsleds-irq.elf` or `zedboard-swsleds-freertos.elf` file built in the Building step to the board with **Run As > Launch on Hardware (GDB)**.
1. You should be able to turn the LEDs on and off with the DIP switches.

### Booting of QSPI flash memory
One might want to boot their program of the flash memory that is available on ZedBoard. For this purpose I added extra two projects, namely _zedboard-swsleds-fsbl_bsp_ and _zedboard-swsleds-fsbl_, that allow for creation of a BOOT.mcs file, i.e. the file for booting of the QSPI flash. The procedure for enabling such a boot mode is as follow:
1. Build one of the projects _zedboard-swsleds-irq_ or _zedboard-swsleds-freertos_ (and their dependencies).
1. Build the project _zedboard-swsleds-fsbl_ (and its dependency _zedboard-swsleds-fsbl_bsp_).
1. Create the boot image by means of **Xilinx > Create Boot Image** dialog.
1. In this dialog select:  
  4.1 Output format: MCS.  
  4.2 Boot image partitions: (bootloader) `zedboard-swsleds-fsbl.elf`, `zedboard_swsleds_irq.bit`, either `zedboard-swsleds-irq.elf` or `zedboard-swsleds-freertos.elf` (in this order).  
  4.3 Name your output .mcs file.
    + Once that information is entered, click the **Create Image** button.

1. Use **Xilinx > Program Flash** to program the memory with the created image.
1. Make sure that the _Boot Mode_ jumpers are set for booting of the flash (`MIO3: 0`, `MIO4: 0`, `MIO5: 1`) before next powering of the board.
