/*
  Digital Spirit Level
  ESP32-C6 + MPU6050 (I2C) + MAX7219 4-in-1 LED matrix (SPI, MD_MAX72XX driver)

  Libraries needed (Library Manager):
    - MD_MAX72XX  (majicdesigns)
    - Adafruit MPU6050
    - Adafruit Unified Sensor
    - Adafruit BusIO

  Wiring:
    MAX7219  VCC -> separate 5V supply (not the ESP32's own 5V pin)
    MAX7219  GND -> common GND with ESP32 and MPU6050
    MAX7219  DIN -> GPIO7
    MAX7219  CLK -> GPIO8
    MAX7219  CS  -> GPIO6
    MPU6050  VCC -> 3.3V
    MPU6050  GND -> GND
    MPU6050  SDA -> GPIO2
    MPU6050  SCL -> GPIO3
    MPU6050  AD0 -> GND
*/

#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---------- USER CONFIG ----------
#define HARDWARE_TYPE   MD_MAX72XX::FC16_HW
#define MAX_DEVICES     4
#define CS_PIN          6
#define SPI_CLK_PIN     8
#define SPI_DATA_PIN    7

#define I2C_SDA_PIN     2
#define I2C_SCL_PIN     3

#define MATRIX_INTENSITY 4      // 0 (dim) - 15 (bright)
#define MAX_TILT_DEG     35.0   // tilt angle that maps to the edge of the display
#define SMOOTHING_ALPHA  0.15   // 0..1, lower = smoother/slower response
#define REFRESH_MS       40

// ---------- OBJECTS ----------
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, SPI_DATA_PIN, SPI_CLK_PIN, CS_PIN, MAX_DEVICES);
Adafruit_MPU6050 mpu;

// ---------- STATE ----------
const int cols = MAX_DEVICES * 8;   // 32 columns across 4 panels
const int rows = 8;

float rollOffset = 0, pitchOffset = 0;
float smoothRoll = 0, smoothPitch = 0;

// ---------- HELPERS ----------

void getTiltAngles(float &roll, float &pitch) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  roll  = atan2(ay, az) * 180.0 / PI;
  pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
}

void calibrate() {
  Serial.println("Calibrating... keep the display flat and still.");
  float rSum = 0, pSum = 0;
  const int N = 100;
  for (int i = 0; i < N; i++) {
    float r, p;
    getTiltAngles(r, p);
    rSum += r;
    pSum += p;
    delay(5);
  }
  rollOffset  = rSum / N;
  pitchOffset = pSum / N;
  Serial.printf("Calibration done. rollOffset=%.2f pitchOffset=%.2f\n", rollOffset, pitchOffset);
}

int angleToPixel(float angleDeg, int size) {
  float clamped = constrain(angleDeg, -MAX_TILT_DEG, MAX_TILT_DEG);
  float norm = (clamped + MAX_TILT_DEG) / (2.0 * MAX_TILT_DEG);
  int idx = round(norm * (size - 1));
  return constrain(idx, 0, size - 1);
}

void drawCrosshair() {
  int cx = cols / 2;
  int cy = rows / 2;
  mx.setPoint(cy, cx, true);
  mx.setPoint(cy, cx > 0 ? cx - 1 : 0, true);
  mx.setPoint(cy > 0 ? cy - 1 : 0, cx, true);
}

void drawBubble(int px, int py) {
  for (int dx = 0; dx <= 1 && px + dx < cols; dx++) {
    for (int dy = 0; dy <= 1 && py + dy < rows; dy++) {
      mx.setPoint(py + dy, px + dx, true);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, MATRIX_INTENSITY);
  mx.clear();

  Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found - check wiring/address (AD0->GND = 0x68).");
    while (1) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrate();
}

void loop() {
  float roll, pitch;
  getTiltAngles(roll, pitch);

  roll  -= rollOffset;
  pitch -= pitchOffset;

  smoothRoll  = smoothRoll  + SMOOTHING_ALPHA * (roll  - smoothRoll);
  smoothPitch = smoothPitch + SMOOTHING_ALPHA * (pitch - smoothPitch);

  int px = angleToPixel(smoothRoll, cols);
  int py = angleToPixel(smoothPitch, rows);

  mx.clear();
  drawCrosshair();
  drawBubble(px, py);

  Serial.printf("roll=%.1f pitch=%.1f -> (%d,%d)\n", smoothRoll, smoothPitch, px, py);

  delay(REFRESH_MS);
}
