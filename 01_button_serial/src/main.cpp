#include <Arduino.h>

// Pins
const int LED_PIN = 38;      // Onboard NeoPixel (adjust if needed)
const int BTN_PIN = 0;       // BOOT button

// Debounce configuration
const unsigned long DEBOUNCE_MS = 25;
bool btnStable       = HIGH;   // INPUT_PULLUP -> HIGH = released
bool btnLastReading  = HIGH;
unsigned long lastDebounceTime = 0;

// LED state machine
enum LedColor { RED = 0, GREEN, BLUE };
LedColor currentColor = RED;

// Set LED color using onboard NeoPixel
void setLed(LedColor c) {
    switch (c) {
        case RED:
            neopixelWrite(LED_PIN, 255, 0, 0);
            break;
        case GREEN:
            neopixelWrite(LED_PIN, 0, 255, 0);
            break;
        case BLUE:
            neopixelWrite(LED_PIN, 0, 0, 255);
            break;
    }
}

// Cycle LED through RGB sequence
void nextColor() {
    currentColor = static_cast<LedColor>((currentColor + 1) % 3);
    setLed(currentColor);
}

// Falling-edge button detection with debounce
bool buttonPressedEdge() {
    bool reading = digitalRead(BTN_PIN);

    if (reading != btnLastReading) {
        lastDebounceTime = millis();
        btnLastReading   = reading;
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
        if (reading != btnStable) {
            btnStable = reading;
            if (btnStable == LOW) {
                return true;  // pressed (falling edge)
            }
        }
    }

    return false;
}

void setup() {
    pinMode(BTN_PIN, INPUT_PULLUP);
    setLed(RED);  // start in RED

    Serial.begin(115200);
    while (!Serial) {
        // wait for USB serial
    }
}

void loop() {
    // Handle serial input
    while (Serial.available() > 0) {
        char ch = Serial.read();

        if (ch == 'n') {
            nextColor();
        } else {
            Serial.write(ch);  // echo unknown characters
        }
    }

    // Handle button press
    if (buttonPressedEdge()) {
        Serial.println("Hello from Mahmoud!");
    }

    delay(1);  // small idle delay
}
