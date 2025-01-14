<!--toc:start-->

- [Concepts](#concepts)
  - [Key Topics](#key-topics)
    - [1. Virtualization](#1-virtualization)
    - [2. Concurrency](#2-concurrency)
    - [3. Persistence](#3-persistence)
  - [Projects](#projects)
    - [xv6 MIT Programming](#xv6-mit-programming)
    - [Project Execution Methods](#project-execution-methods)
- [Core Concepts](#core-concepts)
  - [Virtualizing the CPU](#virtualizing-the-cpu)
    - [Example](#example)
  - [Virtualizing Memory](#virtualizing-memory)
- [Process Management](#process-management)
  - [What is a Process?](#what-is-a-process)
    - [Process Lifecycle](#process-lifecycle)
    - [States](#states)
    - [Context Switching](#context-switching)
  - [Threading Example](#threading-example)
  - [Persistence and File Systems](#persistence-and-file-systems)
- [Summary](#summary)
- [Interlude: Process API](#interlude-process-api)
  - [fork()](#fork)
  - [wait()](#wait)
  - [exec()](#exec)
  - [fork() and exec()](#fork-and-exec)
    - [Shells (Scripting)](#shells-scripting)
    - [Redirecting and Writing to a File](#redirecting-and-writing-to-a-file)
- [Mechanism: Limited Direct Execution](#mechanism-limited-direct-execution)
  - [Challenges of CPU Virtualization](#challenges-of-cpu-virtualization)
    - [1. Performance](#1-performance)
    - [2. Control](#2-control)
    - [Crux: Efficient CPU Virtualization with Control](#crux-efficient-cpu-virtualization-with-control)
    - [Basic Technique: Limited Direct Execution](#basic-technique-limited-direct-execution)
      - [Program Execution at a Low Level](#program-execution-at-a-low-level)
        - [Cons of Limited Direct Execution](#cons-of-limited-direct-execution)
          - [Problem 1: Restricted Operations](#problem-1-restricted-operations)
          - [Problem 2: Context Switching](#problem-2-context-switching)
        - [First Approach: Cooperative Multitasking](#first-approach-cooperative-multitasking)
        - [Second Approach: Preemptive Multitasking](#second-approach-preemptive-multitasking)
      - [Context Switching and Process Management](#context-switching-and-process-management)
- [Scheduling](#scheduling)
<!--toc:end-->

# Concepts

## Key Topics

### 1. Virtualization

- Operating system (OS) transforms physical resources into virtual, user-friendly ones.
- Simplifies system usage via APIs for memory allocation, program execution, and device access.

### 2. Concurrency

- Enables multiple functions to execute simultaneously within the same memory space.
- **Challenges:**
  - Understand CPU and memory virtualization.
  - Grasp address spaces, processes, and context switching.
- **Key Example:**

  ```c
  volatile int counter = 0;
  pthread_create(&p1, NULL, worker, NULL);
  pthread_create(&p2, NULL, worker, NULL);
  ```

  - Race conditions arise due to overlapping instructions.

### 3. Persistence

- File systems handle disk locations, tracking structures, and I/O requests.
- Key methods include **journaling** and **copy-on-write** for fault tolerance.

---

## Projects

### xv6 MIT Programming

- Focus: Kernel development and low-level programming.

### Project Execution Methods

1. Systems programming.
2. xv6 programming.
3. A combination of both.

---

# Core Concepts

## Virtualizing the CPU

- Even with a single processor, OS creates the illusion of multiple CPUs.
- **Mechanisms:** Enable simultaneous execution of programs.
- **Policy:** Determines which program runs during competition for CPU time.

### Example

```bash
prompt> ./cpu A & ; ./cpu B & ; ./cpu C & ; ./cpu D &
```

## Virtualizing Memory

- Physical memory organized as an array of bytes accessed via addresses.
- **Virtual Address Space:**
  - Each program has private memory mapped to physical memory.
  - Provides isolation and efficient resource sharing.

---

# Process Management

## What is a Process?

- A running program with its own memory, including:
  - Code, data, stack, and heap.
- Created by the OS, loaded into RAM from disk, and executed via the Program Counter (PC).

### Process Lifecycle

1. **Create:** Program becomes a process.
2. **Destroy:** Process ends naturally or via manual termination.
3. **Wait:** Pause until a process stops running.
4. **Control:** Suspend or resume processes as needed.
5. **Status:** Retrieve process state or runtime information.

### States

- **Running:** Actively executing instructions.
- **Ready:** Prepared to run but waiting for CPU.
- **Blocked:** Awaiting an event (e.g., I/O completion).

### Context Switching

- Mechanism for the OS to pause and resume processes.
- **Policies:** Algorithms for scheduling decisions.

---

## Threading Example

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

**Compilation and Execution:**

```bash
prompt> gcc -o thread thread.c -Wall -pthread
prompt> ./thread 1000
Initial value: 0
Final value: 2000
```

---

## Persistence and File Systems

- **Responsibilities:**

  1. Determine disk locations for data.
  2. Maintain tracking structures.
  3. Issue I/O requests.

- **Key System Calls:** `open()`, `write()`, `close()`.

---

# Summary

- OS virtualization simplifies resource management through abstraction.
- Process and memory management ensure efficiency and isolation.
- Concurrency enables multitasking but requires mechanisms to handle race conditions.
- Persistence ensures data durability and fault tolerance.

# Interlude: Process API

**Programming with Process API** involves three fundamental system calls:

- **_fork()_**
- **_wait()_**
- **_exec()_**

---

## fork()

The `fork()` system call is used to **create a new process**, but it is one of the most unique routines you will encounter. The process that is created, known as the **child**, is an almost exact copy of the **parent** process that invoked `fork()`. From the OS’s perspective, it looks as though two copies of the program are running, both about to **return from the `fork()` system call**.

Key details about the child process:

- It does not start execution at `main()`.
- It has its own memory space, registers, and other resources.
- The return value from `fork()` distinguishes the parent from the child:
  - Parent receives the PID of the child.
  - Child receives 0.

---

## wait()

The `wait()` system call is used by the parent process to **pause execution until its child process completes**. This ensures proper synchronization between parent and child processes.

For example, if the parent process prints output after the child finishes, the sequence is maintained because the parent waits for the child to terminate.

> [!NOTE]
> There are cases where **`wait()`** may return **before** the child exits. Refer to the man page for more details.

---

## exec()

The `exec()` system call allows a process to **replace its memory space with a new program**. This is useful when the intention is to run a different program from the one currently executing.

When `exec()` is invoked:

- The current program’s code and static data are replaced.
- The heap, stack, and other memory regions are reinitialized.
- The process continues execution with the new program’s entry point and arguments.

---

## fork() and exec()

Combining `fork()` and `exec()` provides a powerful mechanism for creating multi-process programs. Typically:

1. The parent process uses `fork()` to create a child process.
2. The child process invokes `exec()` to load and execute a new program.

For detailed explanations, refer to **Stevens and Rago chapters on Process Control, Process Relationships, and Signals**.

---

#### Shells (Scripting)

Explore the bash man pages to learn more about shell scripting and its capabilities.

---

#### Redirecting and Writing to a File

Unix systems start searching for free file descriptors at zero. When `open()` is called, the first available file descriptor is assigned, such as **`STDOUT_FILENO`**.

---

# Mechanism: Limited Direct Execution

In time-sharing systems, **CPU virtualization** is achieved by sharing the CPU among multiple processes.

## Challenges of CPU Virtualization

### 1. Performance

How can virtualization be implemented efficiently without excessive overhead?

### 2. Control

How can the OS retain control of the CPU to ensure fair sharing and prevent monopolization by any single process?

---

### Crux: Efficient CPU Virtualization with Control

The OS must virtualize the CPU efficiently while maintaining control. This requires cooperation between **hardware** and **software (OS)**.

---

### Basic Technique: Limited Direct Execution

This approach involves running the program directly on the CPU while maintaining OS control.

#### Program Execution at a Low Level

When the OS starts a program, it:

1. Creates a process entry in the process list.
2. Allocates memory pages for the process.
3. Loads the program into memory.
4. Sets up the stack with `argc` and `argv`.
5. Clears registers.
6. Executes the program by calling `main()`.

When the program finishes: 7. The OS frees the process memory. 8. Removes the process from the list.

---

##### Cons of Limited Direct Execution

1. **Restricted Operations**
   - How can the OS ensure that processes don’t perform unauthorized actions, like accessing hardware directly?
2. **Time-Sharing**
   - How can the OS interrupt a running process to switch to another?

---

###### Problem 1: Restricted Operations

While running directly on the CPU is fast, processes must not have unrestricted access to hardware. Instead, processes should use system calls to request OS-controlled operations.

> [!NOTE] > **Protected Control Transfer**: The hardware provides **user mode** (restricted access) and **kernel mode** (full access) to enforce security.

**Example Scenario:**

- A user program wants to write to a file.
- It calls a system function, triggering a **trap** instruction (e.g., `0x80` in x86).
- The trap switches the CPU to **kernel mode**, allowing the OS to handle the request securely.

---

###### Problem 2: Context Switching

**Challenge:** How does the OS regain CPU control to enable process switching?

##### First Approach: Cooperative Multitasking

The process voluntarily yields control to the OS using a system call, such as `yield()`. This approach was used in older systems like early MacOS.

**Drawback:** A misbehaving process could monopolize the CPU indefinitely.

##### Second Approach: Preemptive Multitasking

The OS forcefully takes control using **timer interrupts**:

- The OS configures a timer during boot.
- When the timer fires, an interrupt occurs, pausing the current process and invoking the OS interrupt handler.

---

#### Context Switching and Process Management

When the OS regains control (via cooperative or preemptive methods), it uses a **scheduler** to decide which process to run next.

**Steps for Context Switching:**

1. **Save state** of the current process (registers, program counter, kernel stack).
2. **Restore state** of the next process.

This process ensures smooth switching between processes without corruption or data loss.

**Visualization:**

- Saving the current state and switching to another process ensures that all processes run as though they have exclusive CPU access.

1. **Save state:** save a few register values for the currently-executing process (onto its kernel-stack)
2. **Restore state:** restore a few for the soon-to-be-executing process (from its kernel-stack)

To **save the context of the currently running process**, the OS will execute some low-level assembly code to: **save the general purpose registers, PC (program counter), as well as the kernel stack pointer of the currently running process**
and then **restore said registers, PC and switch to kernel stack for the soon to be executing process**

# Scheduling
