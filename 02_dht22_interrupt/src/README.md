# DHT22 Interrupt-Based Reader with Logic Analyzer Marker (ESP32 + PlatformIO)

## Overview

In this project I want to go beyond using a ready-made DHT22 library and instead
implement the sensor protocol myself. The goal is to:

- Read temperature and humidity from a DHT22 sensor
- Implement the DHT22 one-wire protocol using **interrupts** and precise timing
- Sample new values every 5 seconds
- Visualize the timing of the communication using a **logic analyzer**
- Print the decoded values to the serial terminal


---

## Project Behavior

### 1. DHT22 Protocol & Data Decoding

- The ESP32 sends the standard DHT22 **start signal** by pulling the data line low
  for ~20 ms and then releasing it.
- The DHT22 responds with a sequence of pulses encoding 40 bits:

  - 16 bits humidity  
  - 16 bits temperature  
  - 8 bits checksum  

- An interrupt is attached to the data pin and triggers on **both edges**.
- On each falling edge, the code measures the length of the preceding HIGH pulse:
  - If the pulse is **longer than ~50 µs**, it is interpreted as a logic **1**
  - Otherwise it is interpreted as a logic **0**
- Bits are shifted into a 5-byte buffer and verified using the checksum.

The whole decoding happens in the ISR using `micros()` for timing.

---

### 2. Periodic Measurements

- A new reading is triggered every **5 seconds**.
- After each successful frame:
  - Humidity and temperature are calculated (including support for negative
    temperatures).
  - The values are printed over the serial port in human-readable form, e.g.:

  ```text
  Humidity: 45.3 %  |  Temp: 22.8 °C
    ```

If decoding fails or the checksum does not match, an error message is printed.

### 3. Logic Analyzer Integration

To better understand and debug the timing of the protocol, a dedicated pin is used
as a marker output for a logic analyzer:

LA_PIN goes HIGH during a DHT22 transaction and briefly toggles between
readings.

By connecting this pin and the DHT22 data line to a logic analyzer, it is
possible to visualize:

The start signal sent by the ESP32

The DHT22 response sequence

The pulse widths representing logic 0 and 1

This makes the project useful not only as a sensor demo, but also as a timing and
protocol-analysis exercise.