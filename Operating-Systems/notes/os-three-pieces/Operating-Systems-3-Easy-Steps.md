<!--toc:start-->

- [Concepts](#concepts)
  - [Projects in This Book](#projects-in-this-book)
    - [xv6 MIT Programming](#xv6-mit-programming)
    - [Project Execution Methods](#project-execution-methods)
  - [Concurrency Challenges](#concurrency-challenges)
  - [What Happens During Program Execution?](#what-happens-during-program-execution)
- [Core Problem: How to Virtualize Resources?](#core-problem-how-to-virtualize-resources)
  - [Why Virtualize?](#why-virtualize)
  - [Key Questions](#key-questions)
- [Topics](#topics)
  - [Introduction to Operating Systems](#introduction-to-operating-systems)
    - [How the OS Virtualizes Resources](#how-the-os-virtualizes-resources)
      - [Mechanism](#mechanism)
      - [Benefits](#benefits)
  - [Virtualizing the CPU](#virtualizing-the-cpu)
    - [Key Concepts](#key-concepts)
    - [Example](#example)
  - [Virtualizing Memory](#virtualizing-memory)
    - [Physical Memory](#physical-memory)
    - [Virtual Address Space](#virtual-address-space)
  - [Concurrency](#concurrency)
    - [What is a Thread?](#what-is-a-thread)
    - [Example Program](#example-program)
      - [Compilation and Execution](#compilation-and-execution)
  - [Persistence](#persistence)
    - [File System Responsibilities](#file-system-responsibilities)
    - [Key System Calls](#key-system-calls)
      - [Fault Tolerance Techniques](#fault-tolerance-techniques)
    - [Essential OS Design Principles](#essential-os-design-principles)
  - [Virtualization: The Process](#virtualization-the-process)
    - [CPU Virtualization](#cpu-virtualization)
    - [Time Sharing vs. Space Sharing](#time-sharing-vs-space-sharing)
    - [Context Switching](#context-switching)
      - [A Process](#a-process)
      - [Process API](#process-api)
        - [Create](#create)
        - [Destroy](#destroy)
        - [Wait](#wait)
        - [Miscellaneous Control](#miscellaneous-control)
        - [Status](#status)
      - [Process Creation: A little more detail](#process-creation-a-little-more-detail)
        - [Loaded into the memory, What now?](#loaded-into-the-memory-what-now)
      - [Process States](#process-states)
        - [Running](#running)
        - [Ready](#ready)
        - [Blocked](#blocked)
      - [Data structures](#data-structures)
      - [Summary](#summary)
      - [Interlude: Process API](#interlude-process-api)
        - [fork()](#fork)
        - [wait()](#wait)
        - [exec()](#exec)
        - [fork() and exec()](#fork-and-exec)
- [Stevens and Rago chapters: Process Control, Process Relationships and Signals](#stevens-and-rago-chapters-process-control-process-relationships-and-signals) - [Shells (Scripting)](#shells-scripting) - [Redirecting and writing to a file](#redirecting-and-writing-to-a-file) - [Mechanism: Limited Direct Execution](#mechanism-limited-direct-execution) - [Challenges when building the virtualization machinery](#challenges-when-building-the-virtualization-machinery) - [Performance](#performance) - [Control](#control) - [Crux: How to efficiently virtualize the CPU with Control?](#crux-how-to-efficiently-virtualize-the-cpu-with-control) - [Basic Technique: Limited Direct Execution](#basic-technique-limited-direct-execution) - [Cons of Limited Direct Execution](#cons-of-limited-direct-execution) - [Problem 1: Restricted Operations](#problem-1-restricted-operations) - [Problem 2: Switching between Processes (context-switching)](#problem-2-switching-between-processes-context-switching)
<!--toc:end-->

# Concepts

1. Virtualization
2. Concurrency
3. Persistence

## Projects in This Book

### xv6 MIT Programming

- Focus: Kernel development and low-level programming.

### Project Execution Methods

1. Systems programming
2. xv6 programming
3. A mix of both

---

## Concurrency Challenges

To understand concurrency effectively, you must first grasp:

- CPU and memory virtualization
- Address spaces and processes
- Context switching at arbitrary points

**Once these concepts are understood, threading becomes easier to comprehend.**

---

## What Happens During Program Execution?

1. The processor fetches an instruction from memory.
2. Decodes the instruction to understand its purpose.
3. Executes the instruction.

---

# Core Problem: How to Virtualize Resources?

### Why Virtualize?

- The operating system (OS) simplifies system usage.

### Key Questions

1. What mechanisms and policies does the OS use to achieve virtualization?
2. How does the OS ensure efficiency?
3. What hardware support is required?

**The OS acts as a resource manager, providing a programmable interface (APIs) for applications.**

---

# Topics

## Introduction to Operating Systems

### How the OS Virtualizes Resources

The OS transforms physical resources (e.g., processor, memory, disk) into generalized, powerful, and easy-to-use virtual forms. This is known as **virtualization.**

#### Mechanism

- The OS exports a set of system calls (APIs) for applications to:
  - Run programs
  - Allocate memory
  - Access devices

#### Benefits

- Enables concurrent program execution
- Provides private memory and device access for each program
- Establishes the OS as a **resource manager**

---

## Virtualizing the CPU

Even with a single processor, the OS creates the illusion of multiple virtual CPUs, enabling concurrent program execution.

### Key Concepts

- **Policy:** Determines which program runs when multiple programs compete for CPU time.
- **Mechanisms:** Implement basic functionalities, like running multiple programs simultaneously.

### Example

To run four processes concurrently in a shell:

```bash
prompt> ./cpu A & ; ./cpu B & ; ./cpu C & ; ./cpu D &
```

---

## Virtualizing Memory

### Physical Memory

- Organized as an array of bytes
- Accessed using an address for read/write operations

### Virtual Address Space

- Each program has its own private virtual address space.
- The OS maps this virtual space to physical memory, ensuring:
  - Memory isolation
  - Efficient sharing of physical memory

**This abstraction allows programs to behave as if they have exclusive access to physical memory.**

---

## Concurrency

### What is a Thread?

- A thread is a function running within the same memory space as other functions, allowing multiple functions to execute concurrently.

### Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

volatile int counter = 0;
int loops;

void *worker(void *arg) {
    for (int i = 0; i < loops; i++) {
        counter++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: threads <value>\n");
        exit(1);
    }
    loops = atoi(argv[1]);
    pthread_t p1, p2;

    printf("Initial value: %d\n", counter);

    pthread_create(&p1, NULL, worker, NULL);
    pthread_create(&p2, NULL, worker, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value: %d\n", counter);
    return 0;
}
```

#### Compilation and Execution

```bash
prompt> gcc -o thread thread.c -Wall -pthread
prompt> ./thread 1000
Initial value: 0
Final value: 2000

prompt> ./thread 100000
Initial value: 0
Final value: 143012
```

**Why the inconsistency?**

- The order of code execution leads to race conditions.
- Incrementing a shared counter involves multiple instructions, which can overlap between threads.

---

## Persistence

### File System Responsibilities

1. Determine disk locations for new data.
2. Maintain tracking structures.
3. Issue I/O requests to storage devices.

### Key System Calls

1. `open()`
2. `write()`
3. `close()`

#### Fault Tolerance Techniques

1. **Journaling**
2. **Copy-On-Write**

### Essential OS Design Principles

1. Abstraction
2. Performance optimization
3. Protection and security
4. Reliability
5. Energy efficiency
6. Mobility

---

## Virtualization: The Process

A **process** is a running program. The OS brings static code and data to life, enabling execution.

### CPU Virtualization

- The OS uses **time sharing** to switch between processes, creating the illusion of multiple CPUs.

### Time Sharing vs. Space Sharing

- **Time Sharing:** Allocates CPU time among processes.
- **Space Sharing:** Allocates distinct portions of resources (e.g., disk space) to processes.

### Context Switching

- Enables the OS to pause one program and resume another, facilitating time sharing.

On top of the **Context Switch** we have something called **Policies**.
**Policies** are algorithms for making some kind of decision withing the OS.

For example:

> [!NOTE]
> Question:
> Given the number of possible programs to run on a CPU, which program should OS run?
> Answer:
> **Scheduling policy** in the OS will make that decision.
> It works likely using the historical information (e.g what types of programs are run, performance metrics, is the system optimizing for interactive performance).

#### A Process

Instructions lie's in memory, as well as the data that the running program reads and writes sits in memory as well.
**Thus the memory that a process can address** (called its **address space**) is part of the process.

#### Process API

Every modern Operating System needs to have these API calls:

- Create
- Destroy
- Wait
- Miscellaneous Control
- Status

##### Create

An Operating System must include some method for creating new process.
When the command in shell is executed or user double clicks on the icon of the application the disred process needs to be created under the hood.

##### Destroy

As there is an interface for creating the process, we also need an interface for destroying the process.
Of course programs kill their selfs after execution. But for some reason if they don't there is available interface
for users to manually kill them.

##### Wait

Sometimes it is useful to wait for a process to stop running; thus some kind of interface is needed for that.

##### Miscellaneous Control

Other than killing the process or waiting for it. There are sometimes other controls that are possible; for example:
most operating systems provide some kind of method to suspend a process (stop it from running) and then resume it.

##### Status

There are usually interfaces to get some status information about a process as well, such as how long it has run for, or what state is it in.

#### Process Creation: A little more detail

How is the program transformed into processes?

This is how OS transforms the program into the process

When program is compiled or application is installed, it lives on the hdd or ssdor just stack.
When we double click this is what happens:

Remember, CPU can read from RAM not from external disk like hdd or ssd.

1. **OS first loads the code, static data (initialized variables) all into the memory, into address space of the process**
2. **OS reads bytes from the disk (hdd or ssd) and then places them into the RAM**

   - **Once the code and static data are loadeded into the memory, there are few other things the OS needs to do before running the process.**

3. **Some memory must be allocated for the programs run-time stack (or just stack)**

4. **And then CPU reads intructions from RAM** -> There comes into play PC (Program Counter/Instruction Counter PC/IP)

- In early opertaing systems this process is done **eagerly**, that means all of the code and all of the process are initialized asap.

- Today in this modern operating systems, this process is done more **lazily** by loading the
  pieces of the code and static data only as they are needed for the program execution.

To properly understand **Lazily** running processes (programs) we need to understand
**Pagging & Swapping**.

##### Loaded into the memory, What now?

C programs **use Stack** for local:

- **Variables**
- **Function Parameters**
- **Return Addresses**

The OS allocates this memory and gives it to the process.
The OS will also likely initialize the stack with arguments; specifically, it will fill in the parameters to the main() function,
i.e argc and argv array.

The OS may also create some initial memory for the programs **heap**.
In C programs, the heap is used for explicitly requested dynamically
allocated data; programs request such space by calling **malloc()** and free it explicitly by calling **free()**.

**So the heap memory in the C programs is used when using the malloc() and free().**
**And all of the variables, function parameters and return addresses are in the stack.**

In UNIX systems each process has by default three open file descriptors. For:

1. Standard Input
2. Output
3. Error

And after all of the things completed, OS starts the program from main().

#### Process States

These are the states in what the process can be in when it is created

- Running
- Ready
- Blocked

##### Running

In **Running state**, a process is running on a processor. This means it is executing instructions.

##### Ready

In **Ready state**,a process is ready but for some reason the OS has chosen not to run it at this given moment.

##### Blocked

In **Blocked state**, a process has performed some kind of operation that makes it not ready to run until some other event takes place.
A common example: when a process initialises an I/O request to a disk, it becomes blocked and thus some other process can use the processor.

Blocked state explained: In the blocked state,
the CPU lets a process wait for an I/O operation to complete and switches to another process; once the I/O task finishes, the waiting process moves to the ready state and is scheduled to run again by the CPU.

Being moved from _Ready state_ to _Running state_ it means the process has been _Scheduled_.
Being moved from _Running state_ to _Ready state_ it means the process has been _De-Scheduled_.

#### Data structures

To track the state of each process, for example, the OS likely will keep some kind of **process list for all processes that are ready**, as well as **some additional information to track which process is currently running**.

The OS must also track the **blocked processes**. The OS **must also track when an I/O devices completed the OS should make sure to wake the correct processes and ready to run it again**.

**Register Context** hold a stopped process, the contents of its register state. When a process is stopped, its register state will be saved to this memory location; by restoring these registers (i.e placing their values back into the actual physical registers), the OS can resume running the process.
We will cover this more later in the _Context switch_.

#### Summary

We have introduced the most basic abstraction of the OS: the process.
It is quite simply viewed as a running program. With this conceptual
view in mind, we will now move on to the nitty-gritty: the low-level
mechanisms needed to implement processes, and the higher-level poli-
cies required to schedule them in an intelligent way. By combining mech-
anisms and policies, we will build up our understanding of how an oper-
ating system virtualizes the CPU.

#### Interlude: Process API

Programming with Process API

- _fork()_
- _wait()_
- _exec()_

##### fork()

The _fork_ system call is used to _create a new process_, however it is the strangest routine you will ever call.

The process that is created its _almost exact copy of the calling process_. To the OS that looks like there are two copies of the program running, and both are about to _return from the fork() system call_.

The new process called _child_ in contrast to the creating _parent_ _does not start running at main()_.
_The child is not the exact copy, because it has its own address space, its own memory, its own registers and so forth._
The value it returns to the caller of fork() is different.

##### wait()

The _wait_ system call when called the parent no metter what waits for the child to complete its execution and just then it proceeds with its execution.
That's why we have output I am child /Projects/Examples/parent-child, because the parent waits for the child which returns just 0.

> [!NOTE]
> But there are few cases where **wait()** returns BEFORE the child exits. Read the man page for more info.

##### exec()

The _exec_ system call is useful when you want to run a program that is different from the calling program.
However often you want to run a different program, _exec()_ does just that.

The _exec_ system call, loads the code and the static data of another program, and overwrites the its current code segment (and current static data).
The heap and stack and other parts of the memory space of the program are re-initialized. And then the OS just simply runs the program with specified arguments.

##### fork() and exec()

The fork() and exec() calls are very powerful way for creating multi processed program. And I should make something fun of that.

# Stevens and Rago chapters: Process Control, Process Relationships and Signals

#### Shells (Scripting)

Go to the man pages of bash and learn some more about it.

#### Redirecting and writing to a file

Unix systems start looking for free file descriptors at zero. **STDOUTFILENO** will be the first available one and thus get assigned when open() is called.

#### Mechanism: Limited Direct Execution

As we know with the **time sharing** approach the CPU virtualization is achieved.

##### Challenges when building the virtualization machinery

###### Performance

How can we implement virtualization without adding excessive overhead to the system?

###### Control

How can we run processes efficiently while retaining control over the CPU?
By control it means that the process is not using the whole CPU.

### Crux: How to efficiently virtualize the CPU with Control?

So the OS must virtualize the CPU in an efficient manner, but while retaining control over the system.
For that both hardware and the software (operating system) will be required.

#### Basic Technique: Limited Direct Execution

**So this means just running the program directly on the CPU.**

This is how it is done;

When the OS wishes to start a program running:

**OS:**

1. **Creates a process entry for it in a process list.**
2. **Allocates memory pages for it.**
3. **Loads the program into memory**
4. **Set up stack with argc and argv**
5. **Clear registers**
6. **Executes call main()**

7. Free memory of process - after program execution
8. Remove from process list - after program execution

**Executed Program:**

1. runs main()
2. executes return from main

##### Cons of Limited Direct Execution

1. **If we just run a program, how can the OS make sure that the program doesn't do anything that we don't want it to do?**
2. **When a process is running, how OS stops it from running and switch to another process (time sharing)?**

###### Problem 1: Restricted Operations

Running directly on the CPU is very fast, but very risky. What if
the process wishes to perform some kind of restricted operation, such
as issuing an I/O request to a disk, or gaining access to more system
resources such as CPU or memory?

**Crux: A process must be able to perform I/O and some other restricted operations,
but without giving the process complete control over the system.
How can the OS and hardware work together to do so?**

> [!NOTE] > Protected Control Transfer

Hardware assists the OS by providing different modes of execution.

In **user mode**, Applications does not have full access to hardware resources.
In **kernel mode**, the OS has access to the full resources of the machine.

**Special instructions** to **trap** into the kernel and **return-from-trap**
back to **user mode** programs are also provided, as well as instructions
that allows the OS to tell the hardware where the **trap table** resides in memory.

**Special instructions and user mode explained by example:**
Example Scenario: User program is running on our computer that wants to write data to a file.
**The application itself cannot directly access the disk(a hardware resource)** because it is running in a user-mode which has restricted privileges.

And this is how trap and trap table comes into play:

1. **User program request a services to write to a disk (calls system function (system call)).**
2. **Trap instruction is triggered:**
   system call to a trap: **0x80 (in x86) or svc (in ARM)**
   this trap instruction causes the hardware to:
   - **Save the current state of the user program (program counter, registers)**
   - Switch the CPU to **kernel mode** granting the OS full control of the system.
   - Use the **trap number** provided by the instruction to look up the corresponding handler in trap table.

###### Problem 2: Switching between Processes (context-switching)
