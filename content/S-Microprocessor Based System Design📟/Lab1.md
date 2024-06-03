---
title: Development Tools for STM32 MCU Developing
draft: false
tags:
---

System used: Ubuntu

## Pre-requirements

- c language server (for completion, diagnostics), e.g. `clangd`

- A code editor that use language server, e.g. `vscode`, `vim/neovim`

  

## Install Related Software

### STM32CubeMX

STM32CubeMX is mainly responsible for generating the project with your configuration.

  

For Distro like Ubuntu/Debain, you can go to the [ST official site](https://www.st.com.cn/content/st_com/zh/stm32cubemx.html)

 

### Compiler

Use `arm-none-eabi-gcc`

```bash
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi gdb-multiarch
```

  

### Debugger

Use `OpenOCD` to burn and debug STM32 through STLink v2 (the blue USB device)

> ST-LINK/V2 is an **in-circuit debugger and programmer for the STM8 and STM32 microcontroller families**. The single wire interface module (SWIM) and JTAG/serial wire debugging (SWD) interfaces are used to communicate with any STM8 or STM32 microcontroller located on an application board.

### Step 1: Install OpenOCD

```bash
sudo apt update
sudo apt install openocd
```

### Step 2: Create udev Rules for ST-Link

```bash
sudo nano /etc/udev/rules.d/49-stlinkv2.rules

# STM32 ST-Link v2
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="3748", MODE="0666", GROUP="plugdev"
```

### Step 3: Reload udev Rules

Reload the udev rules and trigger them:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Unplug and replug the ST-Link device.

### Step 4: Verify OpenOCD Installation

Verify that OpenOCD is correctly installed and can recognize your ST-Link device:

```bash
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg
```

You should see output indicating that OpenOCD has connected to the ST-Link device.

### Step 5: Build Your Project

Navigate to your project directory and build your project. Ensure your `lab1.elf` file is generated:

```bash
cd ~/Documents/SJTU/373/lab/lab1 
cmake -S ./ -B ./build 
cmake --build ./build
```

### Step 6: Burn and Debug Your Firmware

Use OpenOCD to burn and debug the firmware:

```bash
`sudo openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "program ./build/lab1.elf reset exit 0x8000000"`
```

## Setup Your STM32 Project

Open your **STM32CubeMX**, configure your project.  

**NOTE**: In `Project Manager -> Project -> Project Settings -> Toolchain / IDE`, use `Makefile/CMake`.

Generate the code and go to the project directory (with `Makefile`/`CMakeLists.txt` in the directory).

Then you need to generate the `compile_commands.json` for `clangd` to recognize the project.
### Makefile

```bash
bear -- make
```
### CMake

```bash
cmake -S ./ -B ./build
```


## Build Project
### Makefile

```bash
make
```

Then target binary file is `./build/<Project Name>.elf`
 
### CMake

```bash
cmake --build ./build
```

Then target binary file is `./build/<Project Name>.elf`

  

## Load to STM32F103C8T6

Use `OpenOCD` to load the binary file to the board.

```bash
sudo openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "program ./build/<Project Name>.elf reset exit 0x8000000"
```

result:

```bash
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
For bug reports, read
http://openocd.org/doc/doxygen/bugs.html
Info : auto-selecting first available session transport "hla_swd". To override use 'transport select <transport>'.
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
Info : clock speed 1000 kHz
Info : STLINK V2J37S7 (API v2) VID:PID 0483:3748
Info : Target voltage: 3.222587
Info : [stm32f1x.cpu] Cortex-M3 r1p1 processor detected
Info : [stm32f1x.cpu] target has 6 breakpoints, 4 watchpoints
Info : starting gdb server for stm32f1x.cpu on 3333
Info : Listening on port 3333 for gdb connections
[stm32f1x.cpu] halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x08000dc8 msp: 0x20005000
** Programming Started **
Info : device id = 0x20036410
Info : flash size = 64 KiB
Warn : no flash bank found for address 0x10000000
Warn : no flash bank found for address 0x10001460
** Programming Finished **
** Resetting Target **
shutdown command invoked
```

  

**NOTE**: In different Distro, the `cfg` file for `OpenOCD` may locate in different directories. You need to find it by yourselves.