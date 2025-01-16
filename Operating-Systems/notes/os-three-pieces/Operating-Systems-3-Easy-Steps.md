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
  - [Scheduling: Introcution](#scheduling-introcution)
    - [Steps taken before building a scheduling policy](#steps-taken-before-building-a-scheduling-policy)
      - [Determining the Workload](#determining-the-workload)
      - [Scheduling metrics (Turnaround and Fairness)](#scheduling-metrics-turnaround-and-fairness)
        - [Turnaround](#turnaround)
        - [Fairness](#fairness)
        - [Response time](#response-time)
      - [First in First Out (First Comes First Served (FCFS))](#first-in-first-out-first-comes-first-served-fcfs)
        - [Example how we calculate the turnaround time](#example-how-we-calculate-the-turnaround-time)
        - [Why is FIFO not that great (FCFS - First Comes First Served)](#why-is-fifo-not-that-great-fcfs-first-comes-first-served)
      - [Shortest Job Next (SJN)](#shortest-job-next-sjn)
        - [Problem with pure SJN (No preemption)](#problem-with-pure-sjn-no-preemption)
      - [Shortest time to Completion First (STCF - has preemption)](#shortest-time-to-completion-first-stcf-has-preemption)
      - [Round Robin](#round-robin)
        - [Crux: How can we build a scheduler that is sensitive to response time](#crux-how-can-we-build-a-scheduler-that-is-sensitive-to-response-time)
        - [Example:](#example)
          - [RR Response Time](#rr-response-time)
          - [SJF Response Time](#sjf-response-time)
        - [Amortization](#amortization)
          - [Amortization: Example](#amortization-example)
        - [RR Example with turnaround time](#rr-example-with-turnaround-time)
        - [Trade off](#trade-off)
        - [Overview](#overview)
      - [Incorporating I/O](#incorporating-io)
        - [Example](#example)
        - [Overlapping](#overlapping)
      - [Unknown Job Length](#unknown-job-length)
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

## Scheduling: Introcution

**Crux: How to Develop the Scheduling Policy?**

- What are the key assumptions?
- What metrics are important?
- What basic approaches have been used in the earliest of computer systems?

Processes running in the system are collectively called **workload**.

### Steps taken before building a scheduling policy

**Goal: is to make fully-operational scheduling discipline(policy)**

1. Determine the Workload
   By determining the workload we can optimize our policies accordingly
2. Scheduling Metrics
   **Turnaround time**
   **Fairness**
   **Response time**

#### Determining the Workload

We are making the assumptions about the workload.

**Jobs running on the system:**

1. Each job runs for the same amount of time.
2. All jobs arrive at the same time.
3. All jobs only use CPU. (they perform no I/O)
4. The run-time of each job is known.

#### Scheduling metrics (Turnaround and Fairness)

##### Turnaround

**Turnaround time is the performance metric of the scheduled jobs.**

**The turnaround time of a job is defined as time at which the job completes minus the time job has arrived.**

**T = turnaround**
**T = 'T completion' - 'T arrived'**

##### Fairness

**Fairness is another metric that measures how equally is CPU spread across scheduled jobs.**

##### Response time
**Response time is defined as the time from when the job arrives in a system to the first time it is scheduled.**

T = Response-time

T = T(first-run) - T(arrival)


#### First in First Out (First Comes First Served (FCFS))

The scheduler is implemented with a Queue (FIFO).
So the job comes in and goes right out.

##### Example how we calculate the turnaround time

There are three jobs A B C, they come in that order to the queue, calculate the turnaround time.
**Each job runs or 10 seconds each**

**Queue looks something like this:**

- **0s**

A - takes 10 seconds to execute

- **10s**

B - takes 10 seconds to execute

- **20s**

C - takes 10 seconds to execute

- **30s**

**A = 10s (10)**
**B = A + 10s (10 + 10) -> 20s**
**C = B + 10s (10 + 10 + 10) -> 30s**

**T = (A + B + C) / 3**

T = 60 / 3

**T = 20s** - performance metric calculated.

##### Why is FIFO not that great (FCFS - First Comes First Served)

When we have different lengths of the jobs, for example
A - 100s
B - 10s
C - 10s

Turnaround time: 110s (not great)

So the B and C job is behind one big A job

#### Shortest Job Next (SJN)

Here comes SJN to fix the stuff.

The SJN will execute jobs in this order:
B, C, A

T = 10 + 20 + 120 / 3
Turnaround time now is 50s.
More then factor of two improvement.

Still we have one problem with the SJN.

##### Problem with pure SJN (No preemption)

If A comes a bit earlier then the B and C. The A will run before the shorter B and C processes.
So again we get bigger turnaround time

#### Shortest time to Completion First (STCF - has preemption)

**Preemptive Shortest Job First (PSJF)**

So we are **pausing (preempting)** the job A and running B and C, then go back to A.

**Any time the new job enters the system, it compares which job has least time left and then schedules that one.**

STCF preempts the A and run B and C to completion; only when they are finished would A's remaining time be scheduled.

**So the A came first, and we take for granted that remaining time left for A is 90s before the B and C came onto the queue.**

A starts execution immediately because it is the first to arrive (from t = 0 to t = 10). However, at t = 10, processes B and C arrive, and SJF prioritizes the processes with the shortest execution time (10 seconds each).
Thus, A is preempted at t = 10.

B gets the CPU and executes from t = 10 to t = 20.

C is next and executes from t = 20 to t = 30.

A then resumes its execution from t = 30 to t = 120 (with 90 seconds of execution time remaining).

T = ((120 - 0) + (20-10) + (30-10)) / 3 = 50s

#### Round Robin 

Round robin fixes key Responsibility for scheduler.

##### Crux: How can we build a scheduler that is sensitive to response time 

Round robin runs jobs in **time-slice** way (sometimes called **scheduling quantum**).
It runs currently executing process for some time, then switches to another one and runs it for some time...
It repeatedly does so until jobs are finished.

> [!NOTE]
> The length of the time-slice needs to be multiple with timer-interrupt period.
> Example:
> If the timer-interrupt period is 10ms, the time-slice could be 10,20 or any other multiple of 10ms.

##### Example:

Assume that three jobs A,B, and C arrive at the time in the system, and that they each wish to run for 5 seconds.
An SJF scheduler runs each job to completion before running other one.

In Contrast RR with cycle through the jobs quickly.

So the Response time for both scheduler algorithms:

###### RR Response Time

(0 + 1 + 2) / 3 = 1

###### SJF Response Time

(0 + 5 + 10) / 3 = 5

##### Amortization

The cost of context-switch might dominate the overall performance of the RR scheduler.
Because switching the processes takes time and resources.

Make it long enough to **amortize** the cost of switching without
making it so long that the system is no longer responsive.

###### Amortization: Example


If the **time slice is set to 10ms, and the context-switch cost is 1ms, roughly 10% of time is spent context switching and is thus wasted**.

But if we set **time slice to 100ms**, and the **context-switch is at 1ms**, briefly time spent is **1% on context-switching** and thus the **cost of time-slicing has been amortized**.

##### RR Example with turnaround time

A,B, and C each running 5 seconds arrive at the same time.
RR time-slice scheduler is set to 1 second.

A finishes at 13, B at 14 and C at 15 seconds each. With an average of 14. AWFUL!

RR is indeed one of the **worst** policies if **turnaround time is our metric**.
RR is stretching out jobs as long as it can, by only running each job for a short bit of time before moving to the next one.

Because turnaround time only cares about when jobs finish, RR is nearly pessimal, even worse then FCFS (FIFO) in many cases.

Any policy that is **fair**, eventually divides the CPU among active processes on a small time scale, will perform **poorly on metrics such as turnaround time**.

So we need to make a trade off.

##### Trade off

If you are willing to be **unfair**, you can **run shorter jobs to completion**, but at the **cost of response time**.

But if we **value fairness**, **response time is lowered, but at cost of turnaround time**. 


##### Overview

We have developed two types of schedulers. 
With assumptions in mind:

- Jobs do no I/O -> Assumption 3
- Runtime of each job is known -> Assumption 4

1. SJF (Shortest Job First), STCF (Shortest time to Completion First)
  **optimizes turnaround time (unfair)**
2. RR (Round Robin)
  **optimizes response time (fairness)**

#### Incorporating I/O

All programs perform I/O.


Currently running process won't be using CPU during the I/O; it is **blocked**, waiting for **I/O request completion**.

##### Example

If the **I/O request is sent to a hard-disk drive**, the process **might be blocked for a few milliseconds or longer, depending on the current I/O load of the drive**.

> [!TIP] Scheduling another process when CPU is blocked for currently running one
> **Thus the scheduler when the currently-running-process is blocked waiting for the I/O response; should probably schedule another job on the CPU at that time.**

> [!TIP] Scheduler also makes the decision when the I/O completes.
> **When that occurs interrupt is raised, and the OS runs and moves the process that issued the I/O from blocked back to the ready state.**


##### Overlapping

We have A and B, which needs **50ms of CPU time**.
**A runs for 10ms and then issues an I/O request (assume here that I/O take 10ms)**, whereas **B simply uses the CPU for 50ms and performs no I/O**.

How can we optimize the scheduler?

If we are just going to build a STCF and just run one job and then the other without considering the I/O into account makes little sense.

> [!TIP] Take I/O into account and build the scheduler (Overlapping)

When the CPU is **blocked** for process A we should run a process B. When the I/O finishes and the process from **blocked state** becomes **ready** we should continue running the process A until I/O request again occurs.
That way we effectively use our system. 

#### Unknown Job Length

The OS rearly knows about the job time to execute.

## Scheduling: The Multi-Level Feedback Queue
