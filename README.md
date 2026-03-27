# STM32 Bare-Metal RTOS (From Scratch)

> *Context switching is an identity swap performed right under the CPU’s nose.*

![os switch context scheme](Documents/OS_scheme.jpg)

## Overview

A **from-scratch bare-metal RTOS kernel** running on **STM32 NUCLEO-F446RE (Cortex-M4)**.

This project explores how a Cortex-M behaves at runtime:  
manual scheduling, context switching, stack orchestration, and interrupt-driven execution —  
**without FreeRTOS, CMSIS-OS, or external abstractions.**

The focus is on understanding the system **from the exception level down to the stack frame**.

For architecture diagrams and FSM visualizations, see the [Documents](Documents/) directory.

---

## Kernel Highlights

- Pure bare-metal implementation  
- Manual context switching via **PendSV**
- Explicit **PSP / MSP separation**
- Handcrafted **initial stack frames (synthetic exception frames)**
- Deterministic scheduler behavior
- Debugger-driven, hardware-first development

---

## Architecture

### RTOS Core
- TIM2 as system tick (1 kHz time base)
- SVC used for controlled first task start
- PendSV as the only context switching mechanism
- Static TCB array with explicit stack ownership per task

### Tasks
- LED task (visible execution)
- Button task (EXTI-driven + debounce handling)
- **Interrupt-driven SPI task (loopback validation, high-rate transactions)**
- Interrupt-driven I2C Master task (write-only, V1)
- Idle task (WFI-based)

---

## Context Switching Model

- Hardware-stacked registers: **R0–R3, R12, LR, PC, xPSR**
- Software-saved registers: **R4–R11**
- Tasks initialized via **synthetic exception frames**
- PendSV runs at lowest priority to ensure safe preemption

> *Context switching is a chain — if the first task is broken, the next one never starts.*

---

## Scheduling & Synchronization

- Priority-based preemptive scheduling  
- Fair round-robin within equal priority levels  
- Semaphore-based signaling (event-style with pending awareness)  

Lightweight stub tasks are used to validate scheduling behavior under different loads.

---

## Interrupt-Driven SPI (Primary Workload)

![SPI Driver FSM](Documents/SPI_MASTER_DRVR_FSM.jpg)

The SPI driver is implemented as an interrupt-driven FSM, progressing on TXE/RXNE events.

The system is primarily validated using a high-rate **ISR-driven SPI driver**,  
running continuous transactions in loopback configuration (MOSI → MISO).

This setup stresses the system under heavy interrupt load and exposes real scheduling constraints.

### Characteristics

- Fully interrupt-driven (**TXE / RXNE**)
- Continuous transaction flow initiated from task context
- ISR-driven progression without polling
- Loopback used for deterministic validation

The SPI workload revealed system-level effects such as interrupt storms,  
scheduler starvation, and sensitivity to interrupt priority configuration.

### Dual-FSM Architecture

The design follows a strict separation between:

- **Task-level FSM**  
  Defines *what* transaction to perform

- **Driver/ISR-level FSM**  
  Defines *how* the hardware progresses through the transaction

This separation decouples scheduling from hardware execution while preserving full control over timing.

> *On microcontrollers, parallelism is an art.*

---

## Interrupt-Driven I2C Master (Write-Only, V1)

As an additional validation path, the kernel includes an  
**ISR-driven I2C Master FSM**.

- Task prepares transaction parameters
- ISR advances strictly on hardware events (**SB / ADDR / TXE / BTF**)

---

## Debugging Focus

- HardFault root-cause analysis (stack + exception return)
- Stack alignment and corruption tracking  
- PSP/MSP transition correctness under interrupt load  
- Interrupt priority tuning and storm analysis  

The goal is not just to make it run — but to understand **why it runs, and when it breaks.**

---

## Hardware

- STM32 NUCLEO-F446RE  
- TIM2 scheduler tick  
- GPIO + EXTI  
- SPI2  
- I2C1  

---

## Status

Actively evolving.  
Features are added only after full architectural validation.

