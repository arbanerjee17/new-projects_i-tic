

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- USER CONFIG ----------
const char* WIFI_SSID     = "realmeGT7";
const char* WIFI_PASSWORD = "qwerrewq00";

#define SERVO_PIN     2
#define OLED_SDA_PIN  8
#define OLED_SCL_PIN  9
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C   // most SSD1306 boards use 0x3C (some use 0x3D)

const int DOOR_OPEN_ANGLE   = 90;
const int DOOR_CLOSED_ANGLE = 0;
// ----------------------------------

WebServer server(80);
Servo doorServo;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool doorOpen = false;

void showStatus(const String &line1, const String &line2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println(line2);
  display.display();
}

void openDoor() {
  doorServo.write(DOOR_OPEN_ANGLE);
  doorOpen = true;
  showStatus("Door", "Opened");
  Serial.println("[DOOR] Opened");
}

void closeDoor() {
  doorServo.write(DOOR_CLOSED_ANGLE);
  doorOpen = false;
  showStatus("Door", "Closed");
  Serial.println("[DOOR] Closed");
}

// GET /door?state=open   or   GET /door?state=close
void handleDoor() {
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "missing 'state' param");
    return;
  }
  String state = server.arg("state");
  if (state == "open") {
    openDoor();
    server.send(200, "text/plain", "door opened");
  } else if (state == "close") {
    closeDoor();
    server.send(200, "text/plain", "door closed");
  } else {
    server.send(400, "text/plain", "state must be 'open' or 'close'");
  }
}

void handleStatus() {
  server.send(200, "application/json",
              String("{\"door\":\"") + (doorOpen ? "open" : "closed") + "\"}");
}

void setup() {
  Serial.begin(115200);
  delay(300);

  // --- OLED init ---
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed - check wiring/address");
  }
  display.setRotation(0);
  showStatus("Booting", "Connecting WiFi...");

  // --- Servo init ---
  ESP32PWM::allocateTimer(0);
  doorServo.setPeriodHertz(50);
  doorServo.attach(SERVO_PIN, 500, 2400);
  closeDoor(); // start closed

  // --- WiFi init ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  showStatus("Ready", WiFi.localIP().toString());
  delay(1500);
  closeDoor();

  // --- Web server routes ---
  server.on("/door", handleDoor);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Use this IP in gesture_detector.py -> ESP32_IP");
}

void loop() {
  server.handleClient();
}
