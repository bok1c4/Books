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

[[../os-three-pieces/CPU-Virtualization.md|Virtualizing-CPU]]

