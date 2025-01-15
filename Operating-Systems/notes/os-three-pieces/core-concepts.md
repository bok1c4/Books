## Some necessary concepts that needs to be covered

Necessary Concepts in Low-Level Programming

    Stack Pointer (SP) and Base Pointer (BP):
        Track the stack's current position and the base of the current stack frame, respectively.
        Used for navigating and managing the stack during function calls.

    Call Stack:
        Sequence of stack frames, one for each function call. Manages nested or recursive function calls.

    Memory Layout of a Program:
        Understanding the text, data, bss, stack, and heap sections.
        How the stack grows downward and the heap grows upward.

    Stack Overflow and Buffer Overflow:
        When the stack exceeds its limit, leading to program crashes.
        Security vulnerabilities caused by writing beyond allocated stack memory.

    Heap Fragmentation:
        Occurs when frequent allocations and deallocations leave gaps in memory, reducing efficiency.

    Alignment and Padding:
        Memory alignment for performance.
        Padding added to maintain alignment.

    ABI (Application Binary Interface):
        Defines how functions call conventions, including stack usage, are implemented.

---

## Stack

**Stack is block of memory used for function execution and local variables. It operates in LIFO (Last in First out).**

### Key Characteristics

- **Automatic memory management**: Memory is allocated and deallocated as functions are called and returned.
- **Fixed Size**: Limited in size, defined when a program starts and can lead to stack overflow if exceeded.
- **Fast access**: Since memory allocation happens in a predictable manner, the stack is faster then the heap.
- **Scoped Memory**: Variables on the stack are destroyed automatically once the function scope ends.

### Usage

1. Functions calls: When a function is called, a **stack frame is created** containing:

- _Return address: Where the program resumes after the function ends._
- _Function arguments_
- _Local variables_

2. Control flow: The **stack pointer keeps track of the top of the stack**, while the **base pointer marks the start of the current function's stack frame.**

---

## Heap

**Heap is large pool of memory for dynamic memory allocation. It is managed manually (or semi-automatic with garbage collected languages).**

### Key Characteristics

- _Manual memory management_: Allocation (malloc/new) and deallocation (free/delete) are explicitly controlled by the programmer.
- _Variable size_: Unlike the stack the heap uses whole system memory.
- _Slower access_: Allocating memory in the heap involved more complex bookkeeping.
- _Presistent data_: Data exists until explicitly deallocated, meaning it lives even after the program ends if its not deallocated.

### Usage

_Dynamic Memory_: Use for large or variably sized data structures like linked lists, trees or large arrays.
_Cross scope data_: Data that needs to outlive the scope of the single function.

---

## Registers

### What are CPU registers?

**Registers** are super-fast memory locations built directly into the CPU. They store data that the CPU needs to access quickly during the computation.
We should think of them as of CPU's 'scratchpad' for performing calculations and managing execution.

### Types of Registers

**General Purpose Registers (GPR's)**

- Used to hold temporary data, variables or intermediate results of computations.
- Examples of GPR (in x86 architecture): **eax**, **ebx**, **ecx**, **edx**

**Special Purpose Registers**

- Instruction Pointer (IP) / Program Counter (PC): Holds the memory of the next instruction to execute.
- Stack Pointer (SP): Points to the top of the stack, used for function calls and local variables change.
- Base Pointer (BP):

## Program Execution

**Process** is just a running program.

This is how OS runs the program.

1. OS makes the entry on process list.
2. Allocates memory for the process on the memory page.
3. Creates the stack for process with argv and argc
4. Loads the programs code into RAM.
5. Clears registers()
6. Runs main()

Program: runs main, and returns

7. Frees memory of the process
8. Removes it from process list.

### System call (user-mode, kernel-mode)

To execute the system call, a program must execute a special **trap** instruction.
This instruction simultaneously jumps into the kernel mode and raises the privilege level to kernel mode; once in the kernel, the system can now perform whatever privileged operations are needed (if allowed).
When finished, the OS calls a **special return from trap** instruction which as you might expect returns into the calling **user-mode** while simultaneously reducing the privileged level back to user mode.

**System call is just an procedure call.**

#### How is system call executed (Procedure call)

It is a procedure call but hidden inside that procedure call is the famous trap instruction.
Example procedure call for open():

1. You are executing the procedure call into the C library.
2. There for system call (open()) the library uses **agreed-upon** calling convention.
3. **kernel puts the arguments on the well-known locations (stack or registers)**
4. **kernel puts the system call number on the stack or register**
5. **executes the trap instruction**.
6. The code in the library after the trap **unpacks return values and returns control to the program that issued that system call.**

**Every system call has its own number for the trap (step 4.)**
open() - system call to a trap: **0x80 (in x86) or svc (in ARM)**

All that code is hard-coded and written in assembly, so we don't need to write assembly for it.

**When the system call is running the processor will push the needed registers, counters, flags and few other registers onto the per-process kernel-stack;** after it finished it will pop them and return to the user-mode execution.

#### How does trap know which code to run inside the OS?

1. The kernel setups the **trap table** at boot time.
   When the machine boots up, it does so in privileged (kernel-mode), and thus is free to configure machine hardware as need be.
   OS thus does is to tell the hardware what code to run when certain event occurs.
   When the keyboard button is pressed, when disk is asked to read and etc...
2. The OS informs the hardware of **the locations of these trap handlers**
3. Once the hardware is informed it remembers until the machine is rebooted. And that's how the hardware knows what to do.

#### Context Switch

When the **scheduler** actually decides to switch to another process that is called context switching. **The job of the context switch is to save and restore**.

1. Save a few register values for the **currently-executing process** onto kernel-stack
2. Restore a few register values for **soon to be executing process** from kernel-stack

##### What is saved and what is restored?

**Saved: general purpose registers, PC (program-counter), kernel stack pointer (base) of the currently-executing process**

**Restored: said registers, PC and switch to kernel-stack for the soon to be executing process**

When the OS finally executes the **return from trap the soon to be running process becomes currently-executing process**

And with that context switch is completed.

##### Vizualization of the Context Switch

OS:

1. boots
2. initializes trap table
   starts interrupt timer

3. Program:
   User executes Process A (Interrupted by the timer-interrupt)
   User executes Process B (Switch from A to B)

4. Handle the trap call (syscall) **switch()**:
   **save reges(A) to proc-struct(A)**
   **restore reges(B) from proc-struct(B)**
   **switch to k-stack(B)**
   **return-from-trap(B)**

Hardware:

1. remembers addresses of syscall handler (remembers where the trap table is)
   time handler
   starts the timer
   interrupt CPU in X seconds
2. timer interrupt
3. **save reges(A) to k-stack(A)**
   **move to kernel-mode**
   **jump to trap handler**
   **restore regs(B) from k-stack(B)**
   **move to user mode**
   **jump B's PC**

Process A is interrupted and the OS decides to switch to Process B.
System call switch() does next: **context switch**

1. saves the register values into the process structure of A
2. restores the registers of Process B from its process structure
3. **switch contexts** (return from trap)

##### Switching Contexts

1. Changing the **stack pointer to use B's kernel stack (and not A's - current one)**
2. Finally the OS **returns from trap which restores B's register state and starts running it**

> [!NOTE] There are two types of register saves/restores that happen during this protocol.
> **First: when the timer interrupt occurs; in this case, the user register state of the running process is implicitly saved by the hardware**
>
> **Second: when the OS decides to switch from A to B; in this case, the kernel register state is explicitly saved by the software. But this time into memory in the process structure of the process**
