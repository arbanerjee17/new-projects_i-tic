Gesture Controlled Door — ESP32-C6 + Servo + OLED (Browser Version)

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/87e4b5a8-df55-4749-8767-2e9c0e1f4431" />

A webcam in your browser watches your hand using MediaPipe (runs entirely in-browser, no installs). Open palm → door opens. Fist → door closes. The command travels over WiFi (HTTP) to the ESP32-C6, which drives the servo (mimicking the door) and updates the OLED with "Door Opened" / "Door Closed".

Files
File	Runs where	Purpose
esp32c6_gesture_door.ino	ESP32-C6 (Arduino IDE)	WiFi web server, servo control, OLED display
gesture_detector.html	Your browser	Webcam + MediaPipe gesture detection, sends commands
Parts
ESP32-C6 dev board
SG90 (or similar) servo motor
SSD1306 OLED display (128x64, I2C)
Jumper wires, breadboard
A laptop/PC with a webcam and a browser (Chrome or Edge recommended), on the same WiFi network as the ESP32-C6
Wiring
Component	ESP32-C6 Pin
Servo signal	GPIO 2
Servo VCC	5V (external supply recommended)
Servo GND	GND
OLED SDA	GPIO 8
OLED SCL	GPIO 9
OLED VCC	3.3V
OLED GND	GND

(Pin numbers are set at the top of esp32c6_gesture_door.ino — change them there if your board's silkscreen differs.)

1. Flash the ESP32-C6
In Arduino IDE, install these libraries (Sketch → Include Library → Manage Libraries):
ESP32Servo
Adafruit SSD1306
Adafruit GFX Library
Install the ESP32 boards package (Espressif) if you haven't, then select Tools → Board → ESP32 Arduino → ESP32C6 Dev Module.
If Serial Monitor shows nothing after uploading: Tools → USB CDC On Boot → Enabled, then re-upload. The C6 uses native USB, so this setting is required to see any Serial output.
Open esp32c6_gesture_door.ino and edit:
cpp
   const char* WIFI_SSID     = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
Upload. Open the Serial Monitor (115200 baud) — once connected it prints something like:
   Connected! IP address: 10.83.156.53

Note that IP address down — you'll need it in the browser page.

2. Run the browser gesture detector

No Python, no pip, nothing to install on your PC — just a local web server so the browser allows camera access.

In VS Code, install the Live Server extension (Extensions icon → search "Live Server" by Ritwick Dey → Install).
Right-click gesture_detector.html in the Explorer panel → Open with Live Server. It opens in your browser at something like http://127.0.0.1:5500/gesture_detector.html.
Allow camera access when the browser prompts you.
In the page's "ESP32-C6 IP address" box, enter the IP from Serial Monitor (e.g. 10.83.156.53).
Click Test connection first — you should see something like OK: {"door":"closed"} in the log box.
Click Start Camera, then show an open palm to open the door or make a fist to close it.

Double-clicking the HTML file directly (file:// in the address bar) often fails to get camera permission — always use Live Server (or any local web server) instead.

How it works
ESP32-C6 runs a tiny HTTP server with two endpoints:
GET /door?state=open → servo moves to 90°, OLED shows "Door Opened"
GET /door?state=close → servo moves to 0°, OLED shows "Door Closed"
GET /status → returns current door state as JSON
Responses include an Access-Control-Allow-Origin: * header so the browser page (a different origin) is allowed to call them (CORS).
The HTML page loads Google's MediaPipe Hand Landmarker model from a CDN and runs it on each webcam frame, entirely client-side.
It counts extended fingers from the hand landmarks each frame, and — with a 1.5s cooldown to avoid spamming — calls fetch() on the matching ESP32 endpoint when the gesture changes.
Troubleshooting
"Test connection" fails / fetch error in log: confirm your PC and the ESP32 are on the same WiFi network/subnet. Also make sure you re-flashed the .ino version that includes the CORS header — an older upload without it will get silently blocked by the browser.
Camera won't start: make sure you're using Live Server (not opening the file directly), and that you clicked "Allow" on the browser's permission prompt. Check no other app (Zoom, Teams, etc.) is already using the webcam.
OLED shows nothing: double-check SDA/SCL wiring and the I2C address (0x3C is used by default in the sketch; try 0x3D if your module needs it).
Servo jitters or resets the board: the 3.3V/5V pin on the ESP32-C6 usually can't supply enough current — power the servo from a separate 5V source and share GND with the board.
Serial Monitor is blank: enable Tools → USB CDC On Boot as described in step 1.3 above, then re-upload.
ESP32 IP changed since last time: your router may reassign DHCP leases. Re-check Serial Monitor for the current IP, or set a static IP / DHCP reservation for the board.
