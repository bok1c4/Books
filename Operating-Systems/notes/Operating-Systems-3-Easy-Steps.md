<!--toc:start-->

- [Concepts](#concepts)
  - [Projects in this book](#projects-in-this-book)
  - [Problem with Concurrency (Problem in knowledge of the student)](#problem-with-concurrency-problem-in-knowledge-of-the-student)
  - [High level what happens to the code and hardware when the program is executed](#high-level-what-happens-to-the-code-and-hardware-when-the-program-is-executed)
- [The CRUX OF THE PROBLEM: How to virtualize resources?](#the-crux-of-the-problem-how-to-virtualize-resources)
- [Topics](#topics)
  - [Introduction to Operating Systems](#introduction-to-operating-systems)
    - [Clix: How to virtualize Resources](#clix-how-to-virtualize-resources)
    - [Vritualizaing the CPU](#vritualizaing-the-cpu)
      - [Questions about Virtualizing the CPU](#questions-about-virtualizing-the-cpu)
    - [Vritualizaing the Memory](#vritualizaing-the-memory)
      - [Virtual Address Space](#virtual-address-space)
    - [Concurrency](#concurrency)
    - [Presistence](#presistence)
  - [Virtualization](#virtualization)
  <!--toc:end-->

# Concepts

1. Virtualization
2. Concurrency
3. Presistence

## Projects in this book

xv6 MIT programming -> kernel development
Low Level programming

Projects are ran in three different ways:

1. systems programming
2. xv6 programming
3. mix of both

## Problem with Concurrency (Problem in knowledge of the student)

To understand Concurrency properly, you need to have the proper understanding of the virtualization of the CPU and of memory.

Concurrency problem arises If you don't understand what an address space is, what a process is, or why context switches can occur at arbitrary points in time.

But once the student understands all of these topics then the thread topic comes into play, but that is easier to understand

## High level what happens to the code and hardware when the program is executed

**Processor fetches an instruction from memory, decodes it (figures out which instruction this is) and then executes it.**

# The CRUX OF THE PROBLEM: How to virtualize resources?

OS - is there to make the system easier to use.

How does the operating systems virtualize resources? Why the OS - is not the main question: But the answer to that is it makes the system easier to use.

**Thus we focus on the:**

1. What mechanisms and policies are implemented by the OS to attain virtualization?
2. How does the OS do so efficiently?
3. What hardware supports is needed?

Body of the software is called Operating System. (OS)

# Topics

## Introduction to Operating Systems

### Clix: How to virtualize Resources

The primary way of the OS does this is through a general technique that we call **virtualization**.

**Explained:** OS takes physical resource (such as the processor, or memory or a disk) and transforms it into a more general, powerful, and easy to use virtual form of itself. Thus we sometimes refer to the operating system as a virtual machine.

But before allowing users to accessing a file, running program, allocating memory or using any other OS feature. The OS first needs to provide the programmable interface that you can call (API's).

Typical OS in fact exports a few hundred system calls that are available to the applications. Because the OS provides these calls to: run programs, allocating memory and devices and other relatable actions. That we call a standard library to applications.

**Virtualizaiton** allows many programs to run in the same time **(concurrently)** accessing their own instructions and data (thus sharing memory), and many programs to access devices (thus sharing disks and so fourth), the OS is sometimes known as a **resource manager**.

---

### Vritualizaing the CPU

Even though we have only one processor somehow all these programs (from the book - 2,1, 2.2) seem to be running at the same time.

Operating System with some help of the hardware, is in charge of the **illusion**, the illusion that the system has a **very large number of virtual CPUs**. Turning a single CPU (or small set of them) into a seemingly infinitive number of CPUs and thus allowing many programs to seemingly run at once is what we call **Virtualizing the CPU**, the focus of the first major part of this book.

#### Questions about Virtualizing the CPU

If two programs want to run at a particular time, which should run? This question is answered by a **policy of the OS; policies are used in many different places within an OS to answer these types of questions**.

And this we will study them as we learn about the basic **mechanisms** that operating systems implement such as the ability to run multiple programs at once.

Hence the role of the OS as a **resource manager**

> [!NOTE]
> We ran four processes at the same time, by using the & symbol. Doing so runs a job in the background in the shell (tcsh or bash) which means that the user is able to immediately issue their next command, which in this case is another program to run. The semi-colon between commands allows us to run multiple programs at the same time in a shell.
> Example:
> prompt> ./cpu A & ; ./cpu B & ; ./cpu C & ; ./cpu D &

### Vritualizaing the Memory

The model of **physical memory** pre-sented by modern machines is very simple. **Memory is just an array of bytes**; to **read memory**, **one must specify and address (DATA AND ADDRESS)** to be able to access the data stored there; **to write (or update) one must also specify the data to be written to the given address**.

**read - specify the address**
**write - specify the data**
**update - specify the new data, specify the address**

Memory is **accessed all the time when a program is running.** A program keeps all of its **data structures in memory,** and accesses then through various instructions, like loads and stores or other explicit instructions that access memory in doing their work.
Don't forget that each **instruction of the program is in memory too; thus memory is accessed on each instruction fetch.**

**PID - process identification** -> Unique Per running process

> [!NOTE]
> malloc() - implementation from the book
> First it allocates some memory,
> then it prints out the address of the memory
> then puts the number zero into the first slot of the newly allocated memory
> finally it loops delaying for a second and incrementing the value stored at the address held in p.

#### Virtual Address Space

Each running program has its own **private virtual address space (address space)**.

**OS is virtualizing memory.**

**Each process access its own private virtual address space (address space), which the OS somehow maps onto the physical memory of the machine.**

**A memory reference within one running program does not affect the address space of other processes (or the OS itself);**

**As far as the running program is concerned, it has physical memory all to itself.**

**The reality, however is that physical memory is a shared resource, managed by the operating system.**

**Exactly how all of this is accomplished is also the subject of the first part of this book, on the topic of virtualization.**

### Concurrency

**Thread** is a function running withing the same memory space as other functions with more then one active at the time.

```c
1 #include <stdio.h>
2 #include <stdlib.h>
3 #include "common.h"
4
5 volatile int counter = 0;
6 int loops;
7
8 void *worker(void *arg) {
9 int i;
10 for (i = 0; i < loops; i++) {
11 counter++;
12 }
13 return NULL;
14 }
15
16 int
17 main(int argc, char *argv[])
18 {
19 if (argc != 2) {
20 fprintf(stderr, "usage: threads <value>\n");
21 exit(1);
22 }
23 loops = atoi(argv[1]);
24 pthread_t p1, p2;
25 printf("Initial value : %d\n", counter);
26
27 Pthread_create(&p1, NULL, worker, NULL);
28 Pthread_create(&p2, NULL, worker, NULL);
29 Pthread_join(p1, NULL);
30 Pthread_join(p2, NULL);
31 printf("Final value : %d\n", counter);
32 return 0;
33 }
```

```bash
prompt> gcc -o thread thread.c -Wall -pthread
prompt> ./thread 1000
Initial value : 0
Final value : 2000
```

```bash
prompt> ./thread 100000
Initial value : 0
Final value : 143012 // huh??
prompt> ./thread 100000
Initial value : 0
Final value : 137298 // what the??
```

The reason why we are not getting 2x the initial value for the bigger values is because there is the problem with the order of the code execution. (Order execution).

Where the shared counter is incremented, it takes three instructions:

1. One to load the value from memory into a register
2. One to increment it
3. One to store it back into memory

Because these three instructions does not execute in the same times, strange things can happen. But later we will cover why and how to address this.

---

### Presistence

The software in the operating system that manages the disk is called **file system**.
There are three calls that program should make to operating system and those are: **open(), write(), close()**.

These system calls are routed to the part of the operating system that is called the file system.

File system has to do a bit of work and that is:

1. Figuring out where on the disk the new data will live.
2. Then keeping the track of it in various structures the file system maintains.
3. Doing so requires issuing I/O request to the underlying storage device, to either read existing structures or update (write) them.

Getting a I/O device to do something for the developer is not very easy, try writing device driver.

The OS provides standard simple library for all of these calls and that is why it is called **standard library**.

To handle crashes and errors when writing to the disk (in the middle of the process) the file system needs to keep track of what is happening.

1. **Journaling**
2. **Copy-On-Write**

When building an operating system the key concepts that needs to be covered in developing are:

1. **Abstraction**
2. **Performance (Minimize the overheads)**
3. **Protection and Security**
4. **Reliability**
5. Energy-Efficiency
6. Mobility

## Virtualization

### The Abstraction: The Process

In this Topic we are Covering what is the **Process**.

The definition of the process is informally is quite simple: it is a **running program**.

The program itself is a lifeless thing it just sits there on the disk, a bunch of instructions and some static data.
It is the operating system that makes that code full of instructions and data useful.

#### The Crux of the Problem: How to Provide The Illusion of Many CPUs?

Well the Operating System uses something called **Virtualizing the CPU**.
By running one process, then stopping it and running another and so forth.

This basic technique, known as **time sharing** of the CPU allows users to run as many concurrent processes as they would like. The potential cost is the performance, as each will run more slowly if the CPU(s) must be shared.

To implement the virtualization of the CPU and to implement it well, the OS will need both some low-level machinery as well as some high level intelligence.
**We call the Low-Level machinery mechanism**. Those are low-level methods or protocols that implement the needed pieces of functionality.

#### Time Sharing and Space Sharing

> [!NOTE] > **Use Time-Sharing and Space-Sharing**

> **Time sharing** is one of the most basic techniques used by an OS to share a resource.
> By allowing the resource to be used for a little while by one entity and then a little while by another, and so forth.
> The CPU or a network link can be shared by many.

> **Space Sharing** is the natural counter part of the Time Sharing where a resource is divided (in space) among those who wish to use it. For example the disk is the natural **space-shared** resource. As once a block is once assigned to a file.
> It is not likely to be assigned to another file until its actually deleted.

#### Context Switch

**Context Switch** gives the OS ability to stop running one program and start running another on a given CPU; this time sharing mechanism is employed by all modern OSes.

On top of the **Context Switch** we have something called **Policies**.
**Policies** are algorithms for making some kind of decision withing the OS.

For example:

> [!NOTE]
> Question:
> Given the number of possible programs to run on a CPU, which program should OS run?
> Answer:
> **Scheduling policy** in the OS will make that decision.
> It works likely using the historical information (e.g what types of programs are run, performance metrics, is the system optimizing for interactive performance).
