#include <Arduino.h>

// Pin configuration
const uint8_t LA_PIN  = 21;   // Logic analyzer marker pin
const uint8_t DHT_PIN = 4;    // DHT22 data pin

// DHT22 data buffer (5 bytes: humidity_hi, humidity_lo, temp_hi, temp_lo, checksum)
volatile uint8_t dht_data[5] = {0};
volatile uint8_t bit_count   = 0;
volatile bool    dht_done    = false;

// Timing + decoding
volatile uint32_t high_start = 0;
const uint16_t BIT_THRESHOLD_US = 50;  // > 50 µs = logic '1', else '0'
volatile uint8_t skip_edges  = 0;

// Interrupt Service Routine for edges on DHT22 data line
void IRAM_ATTR dht_isr() {
    uint32_t now = micros();
    int pin = digitalRead(DHT_PIN);

    // Skip the first two response edges from the sensor (LOW then HIGH)
    if (skip_edges < 2) {
        if (pin == HIGH) {
            high_start = now;
        }
        skip_edges++;
        return;
    }

    if (pin == HIGH) {
        // Rising edge: mark start of HIGH pulse
        high_start = now;
    } else if (bit_count < 40) {
        // Falling edge: measure HIGH pulse width
        uint32_t high_us = now - high_start;
        uint8_t bit = (high_us > BIT_THRESHOLD_US) ? 1 : 0;

        // Shift into current byte
        dht_data[bit_count / 8] <<= 1;
        dht_data[bit_count / 8] |= bit;
        bit_count++;

        // Once 40 bits are received, stop interrupts
        if (bit_count == 40) {
            detachInterrupt(digitalPinToInterrupt(DHT_PIN));
            dht_done = true;
        }
    }
}

// Send start signal and prepare to read a new DHT22 frame
void dht_request() {
    // Reset state
    for (int i = 0; i < 5; ++i) {
        dht_data[i] = 0;
    }
    bit_count  = 0;
    skip_edges = 0;
    dht_done   = false;

    // Start signal: pull line LOW for at least 1 ms (20 ms here), then release
    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(20);
    digitalWrite(DHT_PIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHT_PIN, INPUT_PULLUP);

    // Enable edge interrupts on the data line
    attachInterrupt(digitalPinToInterrupt(DHT_PIN), dht_isr, CHANGE);
}

void setup() {
    Serial.begin(115200);
    pinMode(LA_PIN, OUTPUT);
    digitalWrite(LA_PIN, LOW);

    Serial.println("DHT22 reader with interrupt-based decoding");
}

void loop() {
    // Marker for the logic analyzer: start of a new DHT transaction
    digitalWrite(LA_PIN, HIGH);

    dht_request();

    // Wait until reading is done (interrupts collect the data)
    unsigned long start = millis();
    while (!dht_done && (millis() - start < 100)) {
        // timeout after ~100 ms
    }

    digitalWrite(LA_PIN, LOW);

    if (dht_done) {
        // Verify checksum
        uint8_t sum = dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3];
        if (sum == dht_data[4]) {
            int humidity    = (dht_data[0] << 8) | dht_data[1];
            int temperature = (dht_data[2] << 8) | dht_data[3];

            if (temperature & 0x8000) { // negative temperature
                temperature = -(temperature & 0x7FFF);
            }

            Serial.print("Humidity: ");
            Serial.print(humidity / 10.0);
            Serial.print(" %  |  Temp: ");
            Serial.print(temperature / 10.0);
            Serial.println(" °C");
        } else {
            Serial.println("Checksum error");
        }
    } else {
        Serial.println("Timeout, no response from DHT22");
    }

    // Small logic-analyzer marker pulse between readings (optional)
    digitalWrite(LA_PIN, HIGH);
    delay(1);
    digitalWrite(LA_PIN, LOW);

    delay(5000);  // Read every 5 seconds
}
