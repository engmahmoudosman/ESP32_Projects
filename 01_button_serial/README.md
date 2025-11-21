# RGB LED, Button & Serial Interaction (ESP32 + PlatformIO)

## Overview

This project is a small experiment I built to explore basic I/O handling on the ESP32
using the Arduino framework under PlatformIO.

The goal was to combine three fundamental embedded concepts:

- Reading a button with software debouncing  
- Interacting with the device through the serial monitor  
- Controlling the onboard RGB LED using a simple state machine  

It serves as a compact demonstration of user input, serial communication, and visual
feedback on the ESP32.

---

## Project Behavior

### 1. Button → Serial Output

The BOOT button (GPIO 0) is used as an input with `INPUT_PULLUP`.  
Whenever the button is pressed (falling edge), the device prints:

```text
Hello from Mahmoud!
```

This happens exactly **once per button press**, thanks to edge detection and
debouncing.

---

### 2. Serial Input → LED Color Cycling

The board’s RGB NeoPixel LED begins in **RED**.

- Sending the character `'n'` through the serial monitor advances the LED color:

```
RED → GREEN → BLUE → RED → …
```

- Any other character is simply echoed back to the serial monitor.

This creates a simple interactive loop for testing hardware and
serial-driven state changes.

---

### 3. Serial Configuration

The program uses the following serial settings:

- **Baud rate:** 115200  
- **Format:** 8N1  

---

## Why I Built This

I wanted a minimal yet practical example that combines:

- Clean button debouncing  
- Event-driven logic  
- A color state machine  
- Serial input/output behavior  

This project serves as a foundation for more advanced features such as command
parsers, task management, or interactive device diagnostics.

---

## Tools Used

- **PlatformIO** (VS Code)
- **Arduino Framework**
- **ESP32 Development Board**

---





