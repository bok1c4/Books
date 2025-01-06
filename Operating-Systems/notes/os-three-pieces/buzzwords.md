Registers

Program counter register (PC/Instruction pointer) - tells us which instruction of the program is being executed

Register context

Machine state
Address space (Memory that process can address)

Stack pointer and frame pointer (managing the stack - function parameters, return address, local variables)

Process API - every modern OS has

Eagerly executing the program and Lazily executing the program

For Lazily executing the code we need to understand these two:

- Paging
- Swapping

**So the heap memory in the C programs is used when using the malloc() and free().**
**And all of the variables, function parameters and return addresses are in the stack.**

Scheduled process
De-Scheduled process
CPU Scheduler

Process API
System Calls
There are cases where wait() returns before the child exits.
