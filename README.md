# zedboard-swsleds-irq
## Control state of LEDs (LD0-LD7) with switches (SW0-SW7)

In order to make use of content of this repository you need to:
1. Have ZedBoard Rev. C
1. Have Xilinx SDK installed. (Originally, I worked with with Xilinx SDK 2017.2)
1. (You don’t need Vivado, as output from Vivado is committed here.)

After cloning the repository you need to import the projects to the SDK/Eclipse, and then:
1. Program FPGA with the image design_1_wrapper.bit (the blue LED Done should light up).
1. Build the zedboard-swsleds-irq project.
1. Deploy the .elf file to the board (Run As > Launch on Hardware)
1. Have fun switching the LEDs on and off :)
