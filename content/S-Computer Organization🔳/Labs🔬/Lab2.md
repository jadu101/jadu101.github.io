---
title: Lab2 Assembly Programming
draft: false
tags:
  - assembly
  - ripes
---
## Tasks
Develop a RISC-V assembly program that operates on a data segment consisting of an array of 32-bit signed integers. In the text (program) segment of memory, write a procedure called main that implements the main() function as well as procedures for other subroutines described below.

Assemble, simulate, and carefully comment the file. Screen print your simulation results and
explain the results by annotating the screen prints. You should compose an array whose size is determined by you in the main function and is not less than 30 elements.

In this lab, you are allowed to use only RV32I BASE INTEGER INSTRUCTIONS. Any Extension
Instruction Sets or pseudo-instructions should be avoided.
### Guide Code

```
main() {
	int size = ...; //determine the size of the array here
	int hotDay, coldDay, comfortDay;
	int tempArray[size] = {36, 25, -6,
					... //compose your own array here
						//test data will be provided at demonstration
						   };
	//hotDay is the number of values greater than or equal to 30 in tempArray[]
	hotDay = countArray (______, ______, _______);

	//coldDay is the number of values less than or equal to 5 in tempArray[]
	coldDay = countArray (______, ______, _______);

	//comfortDay is the number of values between 5 and 30 in tempArray[]
	comfortDay = countArray (______, ______, _______);
}

int countArray(int A[], int numElements, int cntType) {

/*************************************************************************
* countArray(int A[], int numElements, int cntType); *
* Count specific elements in the integer array A[] whose size is *
* numElements and return the following: *
* *
* When cntType = 1, count the elements greater than or equal to 30; *
* When cntType = -1, count the elements less than or equal to 5; *
* When cntType = 0, count the elements greater than 5 and less than 30. *
*************************************************************************/
	int i, cnt = 0;
	
		...... //complete the code here
			   //must call functions hot(), cold(), and comfort()
	return cnt;
}

int hot(int x) {
	if(x>=30) return 1;
	else return 0;
}

int cold(int x) {
	if (x<=5) return 1;
	else return 0;
}

int comfort(int x) {
	if (x>5 && x<30) return 1;
	else return 0;
}
```

## Plan

1. **Define the main procedure**: The main procedure should contain the main logic of your program. It should determine the size of the array, initialize the array with data, and call other subroutines to perform specific tasks.
    
2. **Implement subroutines**: You need to implement additional subroutines (procedures/functions) as described in the task:
    
    - `countArray`: This subroutine will count specific elements in the array based on certain conditions.
    - `hot`, `cold`, and `comfort`: These subroutines will determine if a given temperature value represents a hot day, cold day, or a comfort day.

### Main Procedure:

The main procedure is the entry point of your program. Here's what it should do:

1. **Determine the size of the array**: You can either hardcode the size of the array or prompt the user to enter the size. Since the task specifies that the size should not be less than 30 elements, you can set a minimum size of 30.
    
2. **Initialize the array with data**: You need to compose an array of temperature values. You can do this by directly initializing the array in the code or by reading input from the user or a file. Make sure the array is populated with at least 30 elements as specified.
    
3. **Call other subroutines**: Call the `countArray` subroutine three times: once to count hot days, once to count cold days, and once to count comfort days. Pass the array, its size, and a parameter indicating the type of count (hot, cold, or comfort).
    

### Subroutines:

You need to implement four subroutines: `countArray`, `hot`, `cold`, and `comfort`.

1. **countArray**:
    
    - This subroutine iterates over the array and counts elements based on specific conditions.
    - It takes the array, its size, and a parameter indicating the type of count (hot, cold, or comfort).
    - Inside this subroutine, you'll iterate through the array and call either `hot`, `cold`, or `comfort` subroutine based on the `cntType` parameter to determine whether to count the element or not.
    - Increment a counter each time an element meets the specified condition.
    - Finally, return the count.
2. **hot**, **cold**, and **comfort**:
    
    - Each of these subroutines takes a single temperature value as input.
    - `hot` checks if the temperature is greater than or equal to 30.
    - `cold` checks if the temperature is less than or equal to 5.
    - `comfort` checks if the temperature is between 5 and 30.
    - Each subroutine returns 1 if the condition is true, and 0 otherwise.

## Code

```assembly
.data
testArray: .word 47,40,4,48,0,0,21,26,0,-18,26,0,0,-46,27,32,-45,-19,0,41,0,14,-21,28,-37,0,38,21,0,-18,0,0,0,-12,18,-39,-29,26,0,-1
size: .word 40
hot: .word 1
cold: .word 0
comfort: .word -1

.text
main:
    auipc x18 0x10000
    addi x18 x18 0
    lw x19, size
    lw x27, hot
    lw x26, cold
    lw x25, comfort
    # cntType: x6
    addi x6, x0, 1
    jal x1, countArray
    add x24, x10, x0
    addi x6, x0, 0
    jal x1, countArray
    add x23, x10, x0
    addi x6, x0, -1
    jal x1, countArray
    add x22, x10, x0
    jal x1, sumArray
    beq x0, x0, end

countArray:
    # return value: x10
    addi sp, sp, -4
    sw x1, 0(sp)
    addi x5, x0, 0
    addi x10, x0, 0
    beq x6, x27, hotCount
    beq x6, x26, coldCount
    beq x6, x25, comfortCount
exit:
    lw x1, 0(sp)
    addi sp, sp, 4
    jalr x0, x1(0)
    
hotCount:
    # i: x5; offset: x7; return value: x10
    slli x7, x5, 2
    add x7, x7, x18
    lw x8, 0(x7)
    jal x1, isHot
    add x10, x10, x11
    addi x5, x5, 1
    blt x5, x19, hotCount
    bge x5, x19, exit
isHot:
    # x: x8; return value: x11
    addi x11, x0, 0        # Initialize x11 to 0 (default to not hot)
    li x9, 30              # Load immediate value 30 into x9
    bge x8, x9, hotTrue    # If x8 >= 30, jump to hotTrue
    blt x8, x9, hotFalse   # If x8 < 30, jump to hotFalse
hotTrue:
    addi x11, x0, 1
hotFalse:
    jalr x0, x1(0)
    
coldCount:
    # i: x5; offset: x7; return value: x10
    slli x7, x5, 2
    add x7, x7, x18
    lw x8, 0(x7)
    jal x1, isCold
    add x10, x10, x11
    addi x5, x5, 1
    blt x5, x19, coldCount
    bge x5, x19, exit
isCold:
    # x: x8; return value: x11
    addi x11, x0, 0        # Initialize x11 to 0 (default to not cold)
    li x9, 5               # Load immediate value 5 into x9
    ble x8, x9, coldTrue   # If x8 <= 5, jump to coldTrue
    bgt x8, x9, coldFalse  # If x8 > 5, jump to coldFalse
coldTrue:
    addi x11, x0, 1
coldFalse:
    jalr x0, x1(0)

comfortCount:
    # i: x5; offset: x7; return value: x10
    slli x7, x5, 2
    add x7, x7, x18
    lw x8, 0(x7)
    jal x1, isComfort
    add x10, x10, x11
    addi x5, x5, 1
    blt x5, x19, comfortCount
    bge x5, x19, exit

isComfort:
    # x: x8; return value: x11
    addi x11, x0, 0        # Set return value to 0
    addi x9, x0, 5         # Temporary register x9 with value 5
    blt x8, x9, comfortFalse  # If x8 < 5, go to comfortFalse
    addi x9, x0, 30        # Temporary register x9 with value 30
    bge x8, x9, comfortFalse  # If x8 >= 30, go to comfortFalse
    addi x11, x0, 1        # If x8 is between 5 and 30, set return value to 1
comfortFalse:
    jalr x0, x1(0)         # Return to caller

sumArray:
    # i: x5; return value: x10
    addi sp, sp, -4
    sw x1, 0(sp)
    addi x5, x0, 0
    addi x10, x0, 0
sumLoop:
    slli x7, x5, 2
    add x7, x7, x18
    lw x8, 0(x7)
    add x10, x10, x8
    addi x5, x5, 1
    blt x5, x19, sumLoop
    bge x5, x19, exitLoop
exitLoop:
    lw x1, 0(sp)
    addi sp, sp, 4
    jalr x0, x1(0)

end:
    # HotCnt: x24, ColdCnt: x23, ComfortCnt: x22, Sum: x21
    add x21, x10, x0
```