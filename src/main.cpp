/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief Pedal for exoscope robot. 
 * 
 * Note for windows users: After pairing, removing device and repairing required...
 * @version 0.1
 * @date 2024-11-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

// Can also be GPIO38
#define LED_PIN 38           // GPIO pin for addressable LED (Neopixel/WS2812)
#define NUMPIXELS 1          // bultin led
#define BRIGHTNESS 50        // Brightness level for the LED

void startBlinkingBlue();
void blinkTask(TimerHandle_t xTimer);
void setLEDBlueDim();
void flashLEDGreen();

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

BleKeyboard bleKeyboard("Poljin", "Celppu", 100);

#define DEBOUNCE_DELAY 20    // Debounce time in milliseconds

// Define GPIO pins for inputs (GPIO5 to GPIO18)
int inputPins[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

// Define corresponding key mappings (adjustable)
char keyMap[] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F'};

// Store button states for debouncing
bool buttonState[14];         // Current button state
bool lastButtonState[14];     // Last button state
unsigned long lastDebounceTime[14];  // Last debounce time

// FreeRTOS timer handle
TimerHandle_t blinkTimer;
bool ledState = false;        // State to track LED on/off in blinking

void setup() {
  Serial.begin(115200);

  // Initialize BLE keyboard
  bleKeyboard.begin();

  // Initialize input pins as INPUT_PULLUP
  for (int i = 0; i < 14; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
    lastButtonState[i] = HIGH;  // Buttons are unpressed initially
    buttonState[i] = HIGH;      // Buttons are unpressed initially
    lastDebounceTime[i] = 0;    // Initialize debounce times
  }

  // Initialize Neopixel
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);

  // Start blinking blue LED before BLE connection
  startBlinkingBlue();

  // Set up the timer to handle LED blinking every 500ms
  blinkTimer = xTimerCreate("BlinkTimer", pdMS_TO_TICKS(500), pdTRUE, (void*)0, blinkTask);
  //xTimerStart(blinkTimer, 0);  // Start the timer
}

bool BLEConnected = false;


void loop() {
  yield();
  // Check BLE connection status
  if (bleKeyboard.isConnected()) {
    // pass

    // Stop blinking and set constant dim blue LED once connected
    //xTimerStop(blinkTimer, 0);  // Stop the blinking timer
    setLEDBlueDim();
    if(!BLEConnected) {
      Serial.println("BLE connected");
      BLEConnected = true;
    }

    // Handle button press and keypress event
    for (int i = 0; i < 14; i++) {
      int reading = digitalRead(inputPins[i]); // Read the current state of the button

      // Debouncing logic
      if (reading != lastButtonState[i]) {
        lastDebounceTime[i] = millis(); // Update debounce timer
      }

      if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
        if (reading != buttonState[i]) {
          buttonState[i] = reading;

          // If the button is pressed (falling edge)
          if (buttonState[i] == LOW) {
            Serial.print("Button ");
            Serial.print(i);
            Serial.print(" pressed, sending key: ");
            Serial.println(keyMap[i]);

            // Send the corresponding key press and flash green LED
            bleKeyboard.press(keyMap[i]);
            flashLEDGreen();
          } else {
            Serial.print("Button ");
            Serial.print(i);
            Serial.print(" released, releasing key: ");
            Serial.println(keyMap[i]);

            // Release the key when button is released
            bleKeyboard.release(keyMap[i]);
          }
        }
      }

      // Save the current reading as the last state for the next loop iteration
      lastButtonState[i] = reading;
    }
  } else {
    //Serial.println("Waiting for BLE connection...");
    if(BLEConnected) {
      Serial.println("Lost connection");
      BLEConnected = false;
    }
  }

  delay(20);  // Small delay to reduce loop execution speed
  yield();  // Yield to allow other tasks to run
}

// Function definitions

void startBlinkingBlue() {
  // Initial blink (off) to prepare for the blinking loop
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));    // Off
  pixels.show();
  ledState = false;
}

void blinkTask(TimerHandle_t xTimer) {
  // Toggle the LED state for blue blinking effect
  if (ledState) {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Blue
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));    // Off
  }
  pixels.show();
  ledState = !ledState;  // Toggle state for the next blink
}

void setLEDBlueDim() {
  // Dim blue LED when connected. Only once, check current color before setting
  uint32_t currentColor = pixels.getPixelColor(0);
  if (currentColor != pixels.Color(0, 0, 100)) {
    pixels.setPixelColor(0, pixels.Color(0, 0, 100));  // Dim blue
    pixels.show();
  }
}

void flashLEDGreen() {
  // Flash green for keypress feedback
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Green
  pixels.show();
  delay(100);  // Short pulse
  setLEDBlueDim();  // Return to dim blue after pulse
}

