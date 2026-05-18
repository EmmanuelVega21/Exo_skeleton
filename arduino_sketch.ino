// Arduino Sketch - Exoskeleton Control
// This code runs on the Arduino microcontroller

// Pin definitions

// Requires the Adafruit PWM Servo Driver library:
//   install via Arduino Library Manager or from
//   https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50

// =========================
// PCA9685 channels
// =========================
const int servo1Channel = 0;
const int servo2Channel = 1;

// =========================
// FSR input
// =========================
const int fsrPin = A0;

// =========================
// Servo tuning
// Change these after testing
// =========================
int servo1Rest = 240;
int servo2Rest = 240;

int servo1Lift = 320;
int servo2Lift = 320;

// If one servo is mirrored later, change only that servo's values.
// Example:
// int servo2Rest = 320;
// int servo2Lift = 240;

// =========================
// FSR tuning
// =========================
int fsrThreshold = 140;

// =========================
// Smooth motion tuning
// =========================
int stepSize = 2;        // smaller = smoother/slower
int stepDelayMs = 12;    // larger = slower

// =========================
// Debounce / stability
// =========================
int pressCountNeeded = 4;
int releaseCountNeeded = 4;

// =========================
// Serial timing
// =========================
unsigned long lastSerialTime = 0;
const unsigned long serialIntervalMs = 50;   // send data to Pi every 50 ms

// =========================
// Current state
// =========================
int currentPos1 = 240;
int currentPos2 = 240;

bool isLifted = false;
int pressCounter = 0;
int releaseCounter = 0;

void setup() {
  Serial.begin(115200);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  delay(500);

  currentPos1 = servo1Rest;
  currentPos2 = servo2Rest;

  pwm.setPWM(servo1Channel, 0, currentPos1);
  pwm.setPWM(servo2Channel, 0, currentPos2);

  delay(1000);

  Serial.println("ARDUINO_READY");
}

void loop() {
  int fsrValue = analogRead(fsrPin);

  // -------------------------
  // Send data to Pi continuously
  // -------------------------
  if (millis() - lastSerialTime >= serialIntervalMs) {
    Serial.print("FSR:");
    Serial.println(fsrValue);
    lastSerialTime = millis();
  }

  // -------------------------
  // Detect press/release
  // -------------------------
  if (fsrValue > fsrThreshold) {
    pressCounter++;
    releaseCounter = 0;
  } else {
    releaseCounter++;
    pressCounter = 0;
  }

  // -------------------------
  // Lift when pressed
  // -------------------------
  if (!isLifted && pressCounter >= pressCountNeeded) {
    moveBothServosSmooth(servo1Lift, servo2Lift);
    isLifted = true;
    pressCounter = 0;
  }

  // -------------------------
  // Return when released
  // -------------------------
  if (isLifted && releaseCounter >= releaseCountNeeded) {
    moveBothServosSmooth(servo1Rest, servo2Rest);
    isLifted = false;
    releaseCounter = 0;
  }

  delay(10);
}

void moveBothServosSmooth(int target1, int target2) {
  while (currentPos1 != target1 || currentPos2 != target2) {

    if (currentPos1 < target1) {
      currentPos1 += stepSize;
      if (currentPos1 > target1) currentPos1 = target1;
    } else if (currentPos1 > target1) {
      currentPos1 -= stepSize;
      if (currentPos1 < target1) currentPos1 = target1;
    }

    if (currentPos2 < target2) {
      currentPos2 += stepSize;
      if (currentPos2 > target2) currentPos2 = target2;
    } else if (currentPos2 > target2) {
      currentPos2 -= stepSize;
      if (currentPos2 < target2) currentPos2 = target2;
    }

    pwm.setPWM(servo1Channel, 0, currentPos1);
    pwm.setPWM(servo2Channel, 0, currentPos2);

    delay(stepDelayMs);
  }
}