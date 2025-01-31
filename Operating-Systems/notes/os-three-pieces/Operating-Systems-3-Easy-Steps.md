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
  - [Scheduling: Introduction](#scheduling-introduction)
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
  - [Scheduling: The Multi-Level Feedback Queue (MLFQ)](#scheduling-the-multi-level-feedback-queue-mlfq)
    - [MLFQ: Basic Rules](#mlfq-basic-rules)
      - [MLFQ: Assigning Behavior Example](#mlfq-assigning-behavior-example)
        - [Rule 1 and Rule 2](#rule-1-and-rule-2)
    - [Attempt #1: How to change Priority with Rules 3 and 4](#attempt-1-how-to-change-priority-with-rules-3-and-4)
      - [Problems with this current development of MLFQ](#problems-with-this-current-development-of-mlfq)
    - [Attempt #2: The Priority Boost (Time Period - Fixing Starvation)](#attempt-2-the-priority-boost-time-period-fixing-starvation)
    - [Attempt #3: Better Accounting (Time-Slice used - Fixing Gaming the Scheduler)](#attempt-3-better-accounting-time-slice-used-fixing-gaming-the-scheduler)
      - [Explained](#explained)
  - [Scheduling: Proportional Share (Fair-Share scheduler) Lottery Scheduling](#scheduling-proportional-share-fair-share-scheduler-lottery-scheduling)
    - [Basic Concept: Tickets Represent Your Share](#basic-concept-tickets-represent-your-share)
    - [Ticket Mechanism](#ticket-mechanism)
      - [Ticket Currency](#ticket-currency)
      - [Ticker transfer (Ticket inflation)](#ticker-transfer-ticket-inflation)
    - [Implementation of Lottery Scheduling Decision](#implementation-of-lottery-scheduling-decision)
    - [Unfairness](#unfairness)
    - [Stride Scheduling](#stride-scheduling)
    - [Stride Scheduling vs Lottery Scheduling](#stride-scheduling-vs-lottery-scheduling)
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

## Scheduling: Introduction

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

We are making some blatantly non-realistic assumptions about the workload.

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

**We care about the response-time because for example if we are in the terminal and we type ls for example
that process is scheduled on the processor and if we use STCF, we need first to wait for some other process to execute instead of our current one.**

##### Crux: How can we build a scheduler that is sensitive to response time 

Round robin runs jobs in **time-slice** way (sometimes called **scheduling quantum**).
It runs currently executing process for some time, then switches to another one and runs it for some time...
It repeatedly does so until jobs are finished.

> [!NOTE]
> The length of the time-slice needs to be multiple with timer-interrupt period.
> Example:
> If the timer-interrupt period is 10ms, the time-slice could be 10,20 or any other multiple of 10ms.

Example:

Assume that three jobs A,B, and C arrive at the time in the system, and that they each wish to run for 5 seconds.
An SJF scheduler runs each job to completion before running other one.

In Contrast RR with cycle through the jobs quickly.

So the Response time for both scheduler algorithms:

##### RR Response Time

(0 + 1 + 2) / 3 = 1

##### SJF Response Time

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

1. SJF (Shortest Job First) and STCF (Shortest time to Completion First)
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

## Scheduling: The Multi-Level Feedback Queue (MLFQ)

Problems that MLFQ tries to solve are:

1. Optimize the turnaround time (running shorter jobs first) - we don't know the length
2. Make a system feel responsive to the user (lower response time)

Given that in general, **we don't know anything about a process, how can we build a scheduler to achieve those goals? How can the scheduler learn, as the system runs, the characteristics of the jobs it is running, and thus make better scheduling decisions?**


**Crux: How can we design a scheduler that both minimizes response time for interactive jobs, while also minimizing turnaround time without a priority knowledge of job length?**

> [!NOTE]
> Multi-Level Feedback is excellent example of a system that learns from the past to predict the future. That is common in operating systems and many other places in CS.
> Including: Hardware branch predictors, caching algorithms.

### MLFQ: Basic Rules

MLFQ has a number of distinct **queues**, each assigned a different **priority level**.

**Job that is ready to run, it is on a single queue.**

**MLFQ uses priorities to decide which job should run at any given time.**
**Job with higher priority (a job on a higher queue) is chosen to run.**

**More then one job can be on a given queue, in that case we use RR to time-slice them**

**Key lies in how MLFQ assigns priorities.**
**Rather then giving fixed priority to a queue (jobs), MLFQ varies them with observed behavior.**

#### MLFQ: Assigning Behavior Example

**If a job repeatedly relinquishes the CPU while waiting for input from the keyboard, MLFQ will keep its priority high (set on the highest queue). (Interactive Process)**

**If a job on the other hand uses the CPU intensively for long periods of time, MLFQ   will reduce its priority.**

In this way **MLFQ will try to lean about process as they run and thus use the history of the job to predict is future behavior.**

##### Rule 1 and Rule 2

**Rule 1: If Priority(A) > Priority(B), A runs (B doesn't)**
**Rule 2: If Priority(A) == Priority(B), A & B run in RR**

PRIORITIES CHANGE OVER TIME.

### Attempt #1: How to change Priority with Rules 3 and 4

**Rule 3: When job enters the system it is placed as a highest priority queue. (the topmost queue)**
**Rule 4a: If a job uses up an entire time-slice while running, its priority is reduced. (moves down one queue)**
**Rule 4b: If a job gives up the CPU before the time slice is up, it stays at the same priority level.**

#### Problems with this current development of MLFQ

1. **Starvation:**
    If there are **too many interactive processes the long-running ones would never get to be processed (they will consume all of the CPU time)**

2. **Gaming the Scheduler:**
  Someone can rewrite the program to **game the scheduler**. It means that it can write it to **use 99% of the time slice and in last possible moment to issue an I/O request, with that keeping the same priority.**

### Attempt #2: The Priority Boost (Time Period - Fixing Starvation)

To not starve the long-running process, with many interactive ones. We need to add a new rule to the scheduler.

**Rule 5: After some period of time, move all the jobs in the system to the topmost queue.**

The **Time Period** is the time length, which tells to the scheduler **when all the jobs should be pushed to the topmost queue (highest priority).**

**There is also a concern what time is set for Time Period, if its sets too high, long-running jobs could starve. If its set too low, an interactive jobs may not get a proper share of the CPU.**

The values like time-period are sometimes referred to as **voo-doo constants**.

### Attempt #3: Better Accounting (Time-Slice used - Fixing Gaming the Scheduler)

The solution here is to perform **better accounting of the CPU time at each level of MLFQ.**
**Instead of forgetting how much of a time-slice a process used at given level, the scheduler should keep track of it.**

**Rule 4: One a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU), its priority is reduced**

So we have a state in the node for the time that is used in time-slice and if the currently-running-process takes same time to run it is moved down the queue,
that way the gaming the scheduler is fixed.

#### Explained

Without any protection from
gaming, a process can issue an I/O just before a time slice ends and thus
dominate CPU time. With such protections in place, regardless of the
I/O behavior of the process, it slowly moves down the queues, and thus
cannot gain an unfair share of the CPU.

> [!TIP] Avoid Voo-Doo Constants
> Time Period should be avoided if possible (says in the book)
> Too much complication with tables, configuring everything

## Scheduling: Proportional Share (Fair-Share scheduler) Lottery Scheduling

**Instead of optimizing for Turnaround time, or Response time. The Proportional Share scheduler tries to guarantee that each job obtain a certain percentage of CPU time.**

An excellent modern example of proportional-share scheduling is known as **lottery-scheduling**.

**Hold a lottery to determine which process should get to run next; processes that should run more often should be given more chances to win the lottery.**

**Crux: How to Share the CPU Proportionally?**
**How can we design the scheduler to share the CPU in a proportional manner? What are the key mechanisms for doing so? How effective are they?**

### Basic Concept: Tickets Represent Your Share

Underlying lottery-scheduling is one very basic concept: **tickets**, which are used to represent the **share of a resource that a process should receive.**

Example: **Imagine two processes A and B. A has 75 tickets while B has only 25. Thus, what we would like is for A to receive 75% of CPU time and B the remaining 25% of CPU time.**

Lottery scheduling achieves this probabilistically by holding a lottery every so often. Holding a lottery is straightforward: the scheduler must know how many total tickets are (in our example 100).
The scheduler then picks

> [!TIP] Use Randomness
> It is efficient, fast, and takes less memory then traditional algorithms.

Assuming A holds tickets 0 through 74 and B 75 through 99, the winning ticket simply determines whether A or B runs. The scheduler then loads the state of that winning process and runs it.

Here is an example of output how lottery scheduler's winning tickets.

A  B  A  A  B  A  A  A  A  A  A  B  A  B  A  A  A  A A  A
63 85 70 39 76 17 29 41 36 39 10 99 68 83 63 62 43 0 49 49

So as we can see the lottery-scheduler needs randomness. The use of randomness in lottery-scheduling leads to a probabilistic correctness in meeting the desired proportion, but no guarantee.

In our example: B only gets to run 4 out of 20 times slices (20%), instead of the desired 25% allocation.
However the longer these two jobs compete, the more likely they are to achieve the desired percentages.

> [!TIP] Use tickets to represent a share


### Ticket Mechanism

Lottery scheduling also provides a number of mechanisms to manipulate tickets in different and sometimes useful ways.

One way is with the concept of **ticket currency.**

#### Ticket Currency

Currency allows a user with a set of tickets to allocate tickets among their own jobs in whatever currency they would like; 

Example:

**Users A and B have each been given 100 tickets. (System currency 200 tickets).**
**User A is running two jobs, A-1 and A-2 and gives them 500 tickets in A currency. (A currency there are 1000 tickets, A-1 500 and A-2 500).**
**User B in running only one job, B-1 and gives it 10 tickets in B currency. (B currency there are 10 tickets and B-1 took it all).**

**Lottery works on system currency. (200 tickets)**

User A -> 500 (A's currency) to A-1, A-1 -> 50 (global currency)
User A -> 500 (A's currency) to A-2, A-2 -> 50 (global currency)
User B -> 10 (A's currency) to B-1, B-1 -> 100 (global currency)

**Total: 50 + 50 + 100 = 200 tickets**

#### Ticker transfer (Ticket inflation)

**With transfer tickets can temporarily hand off its tickets to another process.**

Example use-case:

**This ability is especially useful in a client/server setting,
where a client process sends a message to a server asking it to do some work on the client's behalf.
To speed up the work, the client passes the tickets to the server and thus try to maximize the performance on the server, while the server is handling the clients request.**

**When finished the server then transfers back the tickets to client behalf.**
Though sometimes ticket inflation can sometimes be a useful technique.

But if the **processes do not trust each other**, this makes **little sense**; one greedy process could give itself a vast number of tickets and take over the machine.

### Implementation of Lottery Scheduling Decision

For implementing the Lottery Scheduling Decision, all we need is:

1. Good random number generator to pick the winning ticket.
2. Data structure to track the processes of the system (e.g linked-lists)
3. Total number of tickets.

We are assuming that we are keeping the processes in a list (linked-lists).

IMPLEMENTING THIS:
**Head -> Node (Job:A Tickets: 100) -> Node (Job: B Tickets: 50) -> Node (Job: C Tickets: 250) -> NULL (Tail)**

To make a scheduling decision, we have to:
1. Pick a random number (the winner), from the total number of tickets.
   Lets say we pick the number of tickets 300.
   Then we simply traverse the list, with a simple counter used to help us find the winner.

2. Code walks (traverses) the list of processes, adding each ticket value to **counter** until the value **exceeds winner**.

3. Once that is the case, the current list element is the winner.

Example:

Winning ticket is 300.

1. Counter is incremented to 100 to account for A's tickets; because the 100 is less then 300, the loop continues.
2. Then counter would be updated to 150 (B's tickets), still less then 300 and thus we again continue.
3. Counter is updated to 400 (greater then 300) and thus we break out of the loop with current pointing at C (the winner)

To make this process more efficient, it might generally be best to organize the list in sorted order. From the highest number of tickets to lowest.

### Unfairness

**Time of the first completed job, divided by the time that the second job completes. That's the unfairness metric**

Example:
R-1 = 10 - first job finishes at 10 seconds
R-2 = 20 - second job finishes at 20 seconds

**U = R-1 / R-2** = 0,5 seconds

When both jobs finishes nearly at the same time U (the scheduler) will be quite close to 1. 
That is our goal.

**Perfectly fair scheduler, would have achieved 1**


### Stride Scheduling 

**Stride Scheduling is deterministic fair-share scheduler.**

The Stride Scheduling is created because the randomness is not 100% accurate especially not on short time scales.

Each job in the system **has a stride**, which is **inverse in proportion to the number of tickets it has**.

How it Works:

Jobs A, B, and C with 100, 50 and 250 tickets, 
respectively we can compute the stride of each by
dividing some large number by the number of tickets each process has been assigned.

A - 100 tickets
B - 50 tickets
C - 250 tickets

> [!NOTE] 10.000 is an example number used, it is recommended to use big values for getting stride

Example:
If we divide each of those ticket values by 10.000. We get the **stride value** of each process.

How stride scheduler executes jobs:

At iteration 0 the pass value for all of the processes is 0.
When all of the pass values are 0, the scheduler picks the lowest stride value or random pick and executes that process.

1. Runs the lowest stride value first Or we can implement randomization for process that will run 
   Process running
2. Increment the **counter** value for the process (called **pass** value) by its stride to track its global process.
3. The scheduler then uses the **stride** and **pass** values to run next process.

And now at any given time the scheduler runs the process that has the lowest pass value so far.

### Stride Scheduling vs Lottery Scheduling

So from the precision of the Stride Scheduling, why should we use Lottery Scheduling?

Well lottery scheduling has one nice property that stride scheduling does not: **no global state**.
Imagine new job enters in middle of the stride scheduling; what should its pass value be? Should it be set to 0? If so, it will monopolize the CPU. 
With Lottery Scheduling there is no global state per process;
we simply add a new process with whatever ticket it has, update the single global variable to track how many total tickets we have and go from there.
In this way lottery scheduling makes it much easier to incorporate new processes in a sensible manner. 

# Multiprocessor Scheduling (Advanced)

Multicore processors are in which multiple CPU cores are packed onto a single CPU chip.
When writing a program adding more CPU's will not run the program faster, but threads and parallel programming will.

**Crux: How should the OS schedule jobs on multiple CPUs? What new problems arise? Do the same old techniques work, or are new ideas required?**

## Background: Multiprocessor Architecture

Difference between single CPU hardware and multi-CPU hardware.
**The difference centers around the use of hardware caches**, and exactly how data is shared across multiple processors.

Caches are small, fast memories hold copies of popular data that is found in the main memory of the system.

Main memory in contrast holds all the data, but access to this larger memory is slower. By keeping frequently accessed data in cache, the system can make the large slow memory appear to be a fast one.

When the program is initially loaded, data is being loaded from the main-memory into CPU. After the initial fetch, if the program is common (instruction) the data will be placed in CPU cache for faster access.

### Cache

Caches are based on the notion of **locality**, of which there are two kinds: **temporal locality** and **spatial locality**.

#### Temporal Locality

When a piece of data is accessed, it is likely to be accessed again in the near future; variables or even instruction themselves being accessed over and over again in loop.

The idea is:
If a program access a data item at address 'x', it is likely to access data items near 'x' as well.

**Cache coherence** happens when we have multiple CPU's working. For example:
CPU-1 fetches something form main memory and puts it in its cache. Then the instructions says it needs to update that value (that is fetched) so writing to main memory is slow so it scheduled for later.
Now all of a sudden before the writing to main memory, program is switched to CPU-2 then when we retrieve the value that CPU-1 retrieved we will get the old avlue instead of new one, and that is **Cache coherence**.

One of the ways to fix this is to use **bus snooping**

**Bus Snooping: ** Each cache pays attention to memory updates by observing the bus that connects them to main memory. **When CPU sees an update for a data item it ho in its cache, it will notice the change and either invalidate its copy  (remove it from its own cache) or update it (put the new value into its cache too)**

**Write-back** caches make this more complicated (because the write to main memory isn't visible until later).

## Don't Forget synchronization

When accessing (and in particular updating) shared data items or structures across CPUs, mutual exclusion primitives (such as locks) should likely be used.
**Use lock when need to update something concurrently (guarantees correctness).**

If thread-1 executes the first line, it will have the current value of **head** stored in its tmp variable. 
If thread-2 then executes the first line as well, it also will have the same value of head stored in its own private tmp.  
(variable tmp is allocated on the stack, and thus each thread will have its own private storage for it).
Thus instead of each thread removing an element from the head of the list, each thready will try to remove the same head element;
Leading to all sorts of problems; like:

1. double free of the head element.
2. potentially returning the same data value twice 

The solution of course is to **use locks** as bolted above. Make routines correct with locks.
Allocating a simple mutex (e.g, pthread_mutex_tm) and then adding a lock(&m) at the beginning of the routine and unlock(&m) at the end of the routine.
Unfortunately such approach is not without problems, in particular in regards to performance.
As the number of CPU's grows, access to a synchronized shared data structure becomes quite slow.


## Cache Affinity

When process have already been run already on the CPU. Some of its state is saved in CPU cache.
For gaining maximum performance and minizing the time of execution of the program, we need to ensure that the process (program) is run on the same CPU.
If the program is run on some different CPU, there will be state invalidation and the data will need to be fetched again.

## Single Queue Scheduling (SQMS)

**SQMS - Single Queue Multi-Processor Scheduling**

Pros: Simple

Cons: 
1. **Scalability**
    To ensure that the scheduler does a good job we need to have locks.
    Problem with SQMS and locks is that the performance is reduced.
    As the number of systems grows, the system spends more and more time on in lock overhead and less time doing the work the system should be doing.
2. **Cache affinity**
    So we have five jobs  (A, B, C, and D) and four processors.
    Our scheduling queue looks something like this:
    A -> B -> C -> D -> E -> NULL. (LL)
    So we assume that each jobs runs for a time slice and then another job is chosen, here is possible job schedule across CPUs:

    CPU-0: A, E, D, C, B
    CPU-1: B, A, E, D, C
    CPU-2: C, B, A, E, D
    CPU-3: D, C, B, A, E

    So because each CPU simply picks the next job to run from the globally shared queue, each job ends up bouncing around from CPU to CPU, thus doing exactly the opposite of what would make sense from the stand point of cache affinity.
    
    To handle this problem SQMS schedulers includes some kind of affinity mechanisms to try to make it more likely that process will continue to run on the same CPU if possible. Specifically one might provide affinity for some jobs, but move others around to balance the load.

    CPU-0: A, E, A, A, A
    CPU-1: B, B, E, B, B
    CPU-2: C, C, C, E, C
    CPU-3: D, D, D, D, E

    With this approach jobs A through D are not moved across processors, with only job E **migrating** from CPU to CPU. Thus preserving the affinity for most.
    Also we can have randomized migration for the jobs, so we can include fairness in our scheduler.


## Multi-Queue Scheduling

Multiple Queues, one per CPU. Multi-queue multiprocessor scheduling (MQMS)

Each queue follows particular scheduling algorithm, such as RR, or any other. 
When job enters the system, it is placed on exactly one scheduling queue.
Then it is scheduling essentially independently, thus avoiding the problems of information sharing and synchronization found in single-queue approach. 

This is an example of how are the jobs scheduled in the job schedulers.

Queue-0 [Node(A), Node(C)]
Queue-1 [Node(B), Node(D)]

Depending on the queue scheduling policy each CPU now has two jobs to choose from when deciding what should run.
With **RR** the system might produce a schedule that looks like this:

CPU-0 [A, A, C, C, A, A, C, C, A, A, C, C]
CPU-1 [B, B, D, D, B, B, D, D, B, B, D, D]

**MQMS has advantage of SQMS in that it should be inherently more scalable.
As the number of CPU's grows, so too does the number of queues.
And thus lock and cache contention should not become a central problem.**

**MQMS additionally provides cache affinity.**

The one problem that MQMS might have is the **load imbalance**.
We go back to this:

CPU-0 [A, A, C, C, A, A, C, C, A, A, C, C]
CPU-1 [B, B, D, D, B, B, D, D, B, B, D, D]

If C finishes first we would have something like this:

CPU-0 [A, A, A, A, A, A]
CPU-1 [B, B, D, D, B, B, D, D, B, B, D, D]

As we can see A gets twice as much CPU then the process B and D which is not the desired outcome.

Even worse is if the A and C finishes there will be only B and D left. But because the A and C was on the CPU-0, it will be left idle, because the remaining jobs are in the CPU-1.
So we need to fix **load imbalance**.

An obvious thing to do is **migration**, move the jobs around the processors.
So for **migrations**, best thing to have is **work stealing**. One CPU should peek at another one, and see if there are notably more jobs then the other one, it takes some jobs and schedules them (steals them).
> [!TIP] Peeking is expensive and it might cove to extensive overload if the peeking is too much present.

## Linux Multiprocessor Schedulers

O(1) scheduler the Completely Fair Scheduler (CFS), and the BF Scheduler (BFS).
Both O(1) scheduler and CFS have multiple queues, whereas BFS has only one queue, showing that both approaches can be successful.
For example:
**The O(1) scheduler is a priority based scheduler (similar to MLFQ)**
**The CFS scheduler is more like lottery scheduler (Stride scheduling)**
**BFS the only single queue approach among the three is also proportional share, but based on a more complicated scheme**
