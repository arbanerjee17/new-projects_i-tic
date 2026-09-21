// Joystick-controlled 9-LED Bar Graph + Servo - ESP32-C6
// Center = LED 5 only, servo at neutral (90°).
// Tilt left -> LEDs 4,3,2,1 fill progressively, servo rotates left (toward 0°).
// Tilt right -> LEDs 6,7,8,9 fill progressively, servo rotates right (toward 180°).

#include <ESP32Servo.h>

const int VRX_PIN = 0;   // Joystick X-axis -> GPIO0 (ADC)
const int VRY_PIN = 1;   // Joystick Y-axis -> GPIO1 (ADC) [unused here]
const int SW_PIN  = 2;   // Joystick button -> GPIO2 [unused here]
const int SERVO_PIN = 20; // Servo signal -> GPIO20

// LED pins, index 0 = leftmost (LED1) ... index 8 = rightmost (LED9)
const int LED_PINS[9] = {3, 4, 5, 6, 7, 10, 11, 18, 19};
const int CENTER_IDX = 4; // index 4 = LED5 = middle

const int ADC_CENTER  = 2048;   // ~mid value for 12-bit ADC (0-4095)
const int DEADZONE    = 250;    // ignore small drift around center
const int ADC_MAX_DEV = 1800;   // deviation at full tilt (tune per your joystick)

const int SERVO_CENTER = 90;    // neutral position
const int SERVO_MIN    = 0;     // full left rotation
const int SERVO_MAX    = 180;   // full right rotation

Servo myServo;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // 0-4095

  for (int i = 0; i < 9; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(SW_PIN, INPUT_PULLUP);

  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(SERVO_CENTER);

  Serial.println("Joystick LED bar + Servo ready.");
}

void loop() {
  int xVal = analogRead(VRX_PIN);
  int deviation = xVal - ADC_CENTER;

  // Turn all LEDs off first
  for (int i = 0; i < 9; i++) digitalWrite(LED_PINS[i], LOW);

  if (abs(deviation) <= DEADZONE) {
    // Stable / centered
    digitalWrite(LED_PINS[CENTER_IDX], HIGH);
    myServo.write(SERVO_CENTER);
    Serial.println("Centered - LED5 ON, Servo at 90°");
  }
  else if (deviation < 0) {
    // Tilted LEFT
    int level = map(constrain(abs(deviation), DEADZONE, ADC_MAX_DEV),
                     DEADZONE, ADC_MAX_DEV, 1, 4);

    for (int i = CENTER_IDX - 1; i >= CENTER_IDX - level; i--) {
      digitalWrite(LED_PINS[i], HIGH);
    }

    int servoAngle = map(level, 1, 4, SERVO_CENTER - 20, SERVO_MIN);
    myServo.write(servoAngle);

    Serial.print("Tilt LEFT - level ");
    Serial.print(level);
    Serial.print(" | Servo angle: ");
    Serial.println(servoAngle);
  }
  else {
    // Tilted RIGHT
    int level = map(constrain(deviation, DEADZONE, ADC_MAX_DEV),
                     DEADZONE, ADC_MAX_DEV, 1, 4);

    for (int i = CENTER_IDX + 1; i <= CENTER_IDX + level; i++) {
      digitalWrite(LED_PINS[i], HIGH);
    }

    int servoAngle = map(level, 1, 4, SERVO_CENTER + 20, SERVO_MAX);
    myServo.write(servoAngle);

    Serial.print("Tilt RIGHT - level ");
    Serial.print(level);
    Serial.print(" | Servo angle: ");
    Serial.println(servoAngle);
  }

  delay(80); // small delay for smooth, non-flickery readings
}
