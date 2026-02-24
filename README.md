# STM32 Bare-Metal RTOS (From Scratch)

> *Context switching is an identity swap performed right under the CPU’s nose.*

![os switch context scheme](Documents/OS_scheme.jpg)

## Overview
This repository contains a **from-scratch bare-metal RTOS kernel** running on an **STM32 NUCLEO-F446RE (Cortex-M4)**.

The focus is the **kernel itself** — scheduling, context switching, stack control, and interrupt-driven execution — without relying on FreeRTOS, CMSIS-OS, or any existing RTOS framework.

The goal is to demonstrate **how a Cortex-M system works under the hood**, not just how to use one.

---

## Key Features
- Pure **bare-metal RTOS kernel**
- Manual context switching using **PendSV**
- Explicit **PSP / MSP** separation
- Handcrafted task stack frames (fake exception frames)
- Deterministic and inspectable scheduler behavior
- Hardware-first, debugger-driven development approach

---

## System Architecture

**RTOS Kernel**
- Scheduler + time base (TIM2)
- SVC for first-task kickoff
- PendSV context switching
- Task Control Blocks (TCBs)
- Explicit stack management

**User Tasks**
- LED task (visible execution)
- Button task (external interrupt stimulus)
- **I2C Master task (interrupt-driven peripheral workload)**
- Idle task

---

## Context Switching Model
- PendSV runs at the lowest priority
- Software saves **R4–R11**
- Hardware automatically stacks:
  **R0–R3, R12, LR, PC, xPSR**
- Tasks start from a **fake exception frame**, identical to a real interrupt return

> *Context switching is a chain — if the first task is broken, the next one never starts.*

---

## Interrupt-Driven I2C Master (V1)

To validate the kernel under a realistic peripheral workload, the project includes an
**I2C Master Write-only driver**, implemented as a fully **ISR-driven finite state machine**.

![I2C Master transmitter FSM](Documents/I2C_MASTER_FSM.jpg)

### Hierarchical Dual-FSM Design (Task + ISR)

- A **transaction-level FSM**, running as an RTOS task, prepares and schedules I2C transfer requests.
- A **driver-level FSM**, running entirely inside the I2C ISR, advances strictly on hardware events
  (**SB / ADDR / TXE / BTF**).

This structure cleanly separates **orchestration (task world)** from **execution (interrupt world)**,
with **no polling loops** and no blocking delays.

Design principles:

- The task only submits a transaction request (buffer + length)
- The I2C peripheral generates hardware events
- The ISR advances the driver state machine step-by-step
- No polling loops, no blocking delays

This creates a true embedded scenario where **interrupt timing and scheduling interact directly**.

> *On microcontrollers, parallelism is an art — interrupts are the choreography.*

---

## Debugging & Bring-Up Focus
This project intentionally tackles real system-level issues:
- HardFault root-cause analysis
- Stack alignment and corruption bugs
- Exception return edge cases
- Safe PSP/MSP transitions under interrupt load

The emphasis is not only on making it run — but on understanding **why it runs**.

---

## Hardware
- STM32 NUCLEO-F446RE
- GPIO + EXTI for visible task behavior
- TIM2 as scheduler time base
- I2C peripheral as an interrupt-driven workload

---

## Status
Actively evolving — features are added only after full understanding and validation.

