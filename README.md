# 1 Introduction
The software code of a little LED array clock,which is designed based on STM32 F1 series MCU. Some pictures are provided to show the outlook of the clock and corresponding design documents are provided to give more information about the structure and PCB of the clock. 

**Click [here](https://pan.baidu.com/s/1aZvg27yC6-ILFsCilPI19w) to get the PCB project,extraction code `j5gw`**

**Click [here](https://www.bilibili.com/video/BV1YA411E7dm) to see a brief video of the clock**

![img1](https://github.com/MiskaMoska/LEDArrayClock/img1.png)

![img2](https://github.com/MiskaMoska/LEDArrayClock/img2.png)

# 2 Functions
- **Time Display：**	The clock can display the time in real time by minute,which won't be interrupted when power off.
- **Time Setting：**  By pressing the key on the PCB,user can set every digit of the time independently.
- **Temperature Display：** The clock is equipped with a temperature sensor so as to capture the environment temperature in real time.
- **Timing：** In timer mode,timing within 60 minutes by second is supported.
# 3 File Specifications
The code project contains several folders:
- **CORE：** Files for Cortex M3 to run.
- **HARDWARE：** Files for each peripheral,in which user functions and variables are defined and declared.
- **FWLIB：** STM32 F1 series' favor library,providing APIs for each peripheral.
- **SYSTEM：** Files and functions for system including main function.

Before compiling,please make sure  that you've included all source files in **CORE, HARDWARE, SYSTEM** and all necessary files in **FWLIB** to the project and included all head file paths inside.
Hope you have fun!
