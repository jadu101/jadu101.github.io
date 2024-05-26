---
title: Lab 1 RISC-V Assembly Instructions
draft: false
tags:
  - assembly
  - ripes
aliases:
---
## Purpose

This lab is intended to help you have a better understanding of the RISC-V assembly instructions, and get familiarized with **Ripes** which is a visual computer architecture simulator and assembler built for the RISC-V instruction set architecture.

## Installation

Download the appimage from [here](https://github.com/mortbopet/Ripes/releases)

## Task

> Write a short assembly program. Create a string of characters of your choice in the .data segment of memory using .string directive. Then copy the string to a different memory section starting at address 0x10000100. Click the Select Processor button on the upper left corner, and select 32-bit Single-cycle processor to debug your program.


## Code

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-22-36.png)


```
.data
str: .string "hello World!"

.text
la a0, str
lui a1, 0x10000
addi a1, a1, 0x100

loop:
lb t0, 0(a0)
sb t0, 0(a1)
addi a0, a0, 1
addi a1, a1, 1
bgtz t0, loop
```

## Breakdown

### Data Segment

```
.data
str: .string "hello World!"
```

- **`.data`**: This directive indicates the beginning of the data segment.
- **`str: .string "hello World!"`**: This defines a null-terminated string "hello World!" in memory at the label `str`.
### Text Segment

```
.text
la a0, str
lui a1, 0x10000
addi a1, a1, 0x100
```

- **`.text`**: This directive indicates the beginning of the code (text) segment.
- **`la a0, str`**: This instruction loads the address of the label `str` into register `a0`. The `la` (load address) instruction is pseudo-instruction and may be translated into multiple instructions to compute the address.
- **`lui a1, 0x10000`**: This instruction loads the upper 20 bits of the immediate value `0x10000` into register `a1`. The `lui` (load upper immediate) instruction is used to set the higher bits of a register.
- **`addi a1, a1, 0x100`**: This instruction adds the immediate value `0x100` to the current value in register `a1` and stores the result back in `a1`. This sets `a1` to `0x10000100`.

Address of `str` loads to `a0` and the value changes into `0x100000000`.

`a1` value changes to `0x10000000` as well because upper 20 bits of the immediate value `0x10000` is saved to register `a1`.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-10-49.png)



This instruction adds the immediate value `0x100` to the current value in register `a1` and stores the result back in `a1`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-11-02.png)




### Loop for copying string

```
loop:
lb t0, 0(a0)
sb t0, 0(a1)
addi a0, a0, 1
addi a1, a1, 1
bgtz t0, loop
```

- **`loop:`**: This is a label marking the beginning of the loop.
- **`lb t0, 0(a0)`**: This instruction loads a byte from the address in `a0` into register `t0`. `lb` stands for "load byte".
- **`sb t0, 0(a1)`**: This instruction stores the byte in register `t0` to the address in `a1`. `sb` stands for "store byte".
- **`addi a0, a0, 1`**: This instruction increments the address in `a0` by 1. `addi` stands for "add immediate".
- **`addi a1, a1, 1`**: This instruction increments the address in `a1` by 1. `addi` stands for "add immediate".
- **`bgtz t0, loop`**: This instruction branches to the label `loop` if the value in `t0` is greater than zero. `bgtz` stands for "branch if greater than zero". However, this might not work as intended because the loop should terminate when a null-terminator (0) is encountered. A more appropriate instruction would be `bnez t0, loop` (branch if not equal to zero), but let's analyze the given code as requested.

instruction loads a byte from the address in `a0` into register `t0`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-17-24.png)




Instruction increments the address in `a0` by 1:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-17-39.png)




Instruction increments the address in `a1` by 1:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/ComputerOrg/Screenshot%20from%202024-05-22%2014-17-50.png)


### Analysis Summary

1. **`.data` segment**: Defines a string "hello World!" in memory.
2. **`.text` segment**: Contains the executable code.
3. **Address Loading**:
    - `la a0, str`: Loads the address of the string into `a0`.
    - `lui a1, 0x10000`: Initializes the upper part of the address in `a1` to `0x10000`.
    - `addi a1, a1, 0x100`: Completes the address in `a1` to `0x10000100`.
4. **Loop**:
    - **`loop:`**: Marks the start of the loop.
    - `lb t0, 0(a0)`: Loads a byte from the source string into `t0`.
    - `sb t0, 0(a1)`: Stores the byte from `t0` to the destination address.
    - `addi a0, a0, 1`: Increments the source address.
    - `addi a1, a1, 1`: Increments the destination address.
    - `bgtz t0, loop`: Continues the loop if `t0` is greater than zero.