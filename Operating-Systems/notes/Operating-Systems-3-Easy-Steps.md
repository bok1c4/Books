<!--toc:start-->

- [Concepts](#concepts)
  - [Projects in this book](#projects-in-this-book)
  - [Problem with Concurrency (Problem in knowledge of the student)](#problem-with-concurrency-problem-in-knowledge-of-the-student)
  - [High level what happens to the code and hardware when the program is executed](#high-level-what-happens-to-the-code-and-hardware-when-the-program-is-executed)
- [The CRUX OF THE PROBLEM: How to virtualize resources?](#the-crux-of-the-problem-how-to-virtualize-resources)
- [Topics](#topics)
  - [Introduction to Operating Systems](#introduction-to-operating-systems) - [Clix: How to virtualize Resources](#clix-how-to-virtualize-resources) - [Vritualizaing the CPU](#vritualizaing-the-cpu) - [Questions about Virtualizing the CPU](#questions-about-virtualizing-the-cpu) - [Vritualizaing the Memory](#vritualizaing-the-memory) - [Virtual Address Space](#virtual-address-space) - [Concurrency](#concurrency)
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

When building an operating system the key concepts that needs to be covered in developing are:

1. **Abstraction**
2. **Performance (Minimize the overheads)**
3. **Protection and Security**
4. **Reliability**
5. Energy-Efficiency
6. Mobility
