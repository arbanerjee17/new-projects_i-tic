// PIR Motion Sensor + LED + Servo - ESP32-C6 (Interrupt-based, low latency)
// Requires "ESP32Servo" library (Install via Library Manager: Sketch > Include Library > Manage Libraries > search "ESP32Servo" by Kevin Harrington)

#include <ESP32Servo.h>

const int PIR_PIN   = 4;   // PIR OUT -> GPIO2
const int LED_PIN   = 5;   // LED -> GPIO3 (through resistor)
const int SERVO_PIN = 6;   // Servo signal -> GPIO4

Servo myServo;

volatile bool motionDetected = false;
volatile bool motionCleared  = false;

const int ANGLE_IDLE     = 0;    // resting position
const int ANGLE_TRIGGERED = 90;  // rotate to this on detection

void IRAM_ATTR handleMotion() {
  if (digitalRead(PIR_PIN) == HIGH) {
    motionDetected = true;
  } else {
    motionCleared = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  myServo.setPeriodHertz(50);        // standard 50Hz servo signal
  myServo.attach(SERVO_PIN, 500, 2400); // min/max pulse width in µs (adjust if servo jitters/doesn't reach full range)
  myServo.write(ANGLE_IDLE);

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), handleMotion, CHANGE);

  Serial.println("PIR warming up (stabilizing)...");
  delay(20000);   // required for PIR sensor to settle - not skippable
  Serial.println("Ready. Monitoring for motion...");
}

void loop() {
  if (motionDetected) {
    motionDetected = false;
    digitalWrite(LED_PIN, HIGH);
    myServo.write(ANGLE_TRIGGERED);
    Serial.println("Object detected!");
  }

  if (motionCleared) {
    motionCleared = false;
    digitalWrite(LED_PIN, LOW);
    myServo.write(ANGLE_IDLE);
    Serial.println("Motion ended.");
  }
}