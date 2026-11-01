# STM32 Discovery Bare-Metal RTC Clock & I2C LCD Driver

A bare-metal C implementation of a DS1307 RTC and an I2C 1602 LCD driver running on the **STM32F407 Discovery (`STM32F407G-DISC1`)** board. Developed strictly at the register level without using HAL, Standard Peripheral Libraries, or ST's auto-generated code.

Includes a custom C startup file (`stm32_startup.c`), linker script (`stm32_ls.ld`), and GNU Toolchain `Makefile` written from scratch.

---

## Features

* **Register-Level Drivers:** Bare-metal GPIO and I2C2 driver implementation from scratch.
* **DS1307 RTC Integration:** BCD conversions, time/date reading and writing.
* **16x2 I2C LCD Driver:** HD44780 instruction set handling via PCF8574 I2C I/O expander.
* **SysTick Interrupt:** Periodic display updates driven by SysTick timer interrupts.
* **Custom Build Pipeline:** Replaced ST's default assembly startup with a minimal C startup and custom linker script.

---

## Hardware Setup

* **MCU:** STM32F407VGT6 (STM32F4 Discovery Board)
* **RTC Module:** DS1307 (I2C)
* **Display:** 16x2 LCD + PCF8574 I2C adapter

### Pinout (I2C2)

| Line | STM32F4 Pin | Notes |
| :--- | :--- | :--- |
| **I2C1_SCL** | `PB6` | Open-Drain, internal Pull-Up |
| **I2C1_SDA** | `PB7` | Open-Drain, internal Pull-Up |
| **I2C2_SCL** | `PB10` | Open-Drain, internal Pull-Up |
| **I2C2_SDA** | `PB11` | Open-Drain, internal Pull-Up |

---

## Building & Flashing

### Prerequisites

* `arm-none-eabi-gcc` toolchain
* `make`
* `openocd`

### 1. Build

Navigate to the `Src` directory and build the project:

```bash
cd Src
make clean
make
```

### 2. Flash via OpenOCD

Start the OpenOCD server to load the binary:

```bash
make load
```

Or run directly in a single commad (without GDB):

```bash
openocd -f board/stm32f4discovery.cfg -c "program final.elf verify reset exit"
```
