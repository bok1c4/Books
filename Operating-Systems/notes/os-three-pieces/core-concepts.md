<!--toc:start-->

- [Some necessary concepts that needs to be covered](#some-necessary-concepts-that-needs-to-be-covered)
- [Stack](#stack)
  - [Key Characteristics](#key-characteristics)
  - [Usage](#usage)
- [Heap](#heap)
  - [Key Characteristics](#key-characteristics)
  - [Usage](#usage)
- [Registers](#registers)
  - [What are CPU registers?](#what-are-cpu-registers)
  - [Types of Registers](#types-of-registers)
  <!--toc:end-->

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
