# STM32 Bare-Metal RTOS (From Scratch)

> *"it's not demo, it's system"*

## Overview
This project is a **from-scratch bare-metal RTOS kernel** implemented on an **STM32 (NUCLEO-F446RE)**.

It is a **standalone RTOS project**, not tied to any specific application.
The focus is the **kernel itself**: context switching, scheduling, stack management, and interrupt-driven execution — without relying on FreeRTOS, CMSIS-OS, or any existing kernel.

The goal is to demonstrate **how a Cortex-M system actually works under the hood**, not just how to use it.

---

## Key Highlights
- Pure **bare-metal RTOS kernel**
- No FreeRTOS / no CMSIS-OS
- Manual context switching using **PendSV**
- Explicit **PSP / MSP** separation
- Handcrafted task stack frames (fake exception frames)
- Deterministic, inspectable scheduling model
- Hardware-first, debugger-driven development approach

---

## System Architecture

User Tasks
│
├── Task A (example)
├── Task B (example)
├── Idle Task
│
RTOS Kernel
│
├── Scheduler (time & state based)
├── PendSV Context Switch
├── Task Control Blocks (TCB)
├── Stack Management
│
Hardware Layer
│
├── Timer (SysTick / TIM)
├── NVIC
├── GPIO (for visible task behavior)
│
MCU Hardware (STM32F446)


---

## RTOS Design Details

### Context Switching
- **PendSV** is used as the context switch exception (lowest priority)
- Software saves **R4–R11**
- Hardware automatically stacks:
  - **R0–R3, R12, LR, PC, xPSR**
- Each task is initialized with a **fake stack frame** identical to a real exception return frame

This allows tasks to start execution as if they were resumed from an interrupt.

---

### Stack Model
- Full descending stack
- 8-byte aligned (AAPCS compliant)
- Explicit handling of the **first task entry edge-case**

> **Insight:**  
> *Context switching is a chain — if the first task is broken, the next one never starts.*

---

## Scheduler Philosophy
- Time is handled centrally (single source of truth)
- Tasks never manage delays directly
- The scheduler decides **who runs next**, not *what* tasks do internally

This avoids hidden coupling between task logic and timing logic.

---

## Debugging Focus
This project intentionally exposes and solves **non-trivial system-level problems**:
- HardFault root-cause analysis
- Stack corruption and misalignment bugs
- Exception return path errors
- PSP / MSP transition hazards
- First-task bootstrap issues

The emphasis is not just on making it work — but on **understanding why it works**.

---

## Hardware
- STM32 NUCLEO-F446RE
- GPIO / LEDs used for visible task execution and timing behavior

---

## Why This Project Exists
This repository serves as a **learning platform and professional showcase**.

It demonstrates:
- System-level thinking
- Hardware-aware software design
- Deep debugging and bring-up skills
- Understanding beyond typical application-level firmware

---

## Author
**Gadi Teicher**  
Embedded / Firmware Engineer  
Strong hardware & debugging background
@gadigidi@gmail.com

---

## Status
Actively evolving — features are added only after full understanding and validation.

