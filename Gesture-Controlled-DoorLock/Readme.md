# Gesture Controlled Door — ESP32-C6 + Servo + OLED

![Gesture Controlled Door](https://github.com/user-attachments/assets/0deee18c-b2fc-4caa-86a3-c27a6a5ab94f)

A browser-based gesture-controlled door system using an **ESP32-C6, SG90 servo, and SSD1306 OLED display**. A webcam detects hand gestures using **MediaPipe directly in the browser**. An **open palm** opens the door, while a **fist** closes it. The command is sent over Wi-Fi to the ESP32-C6 using HTTP.

## Features

* ✋ Open palm → Door opens
* ✊ Fist → Door closes
* 📷 Browser-based webcam gesture detection
* 🧠 MediaPipe Hand Landmarker
* 📡 Wi-Fi communication using HTTP
* ⚙️ ESP32-C6 controls the servo
* 🖥️ OLED displays the current door status
* 🌐 No Python or additional software required for gesture detection

---

## Files

| File                       | Runs On                | Purpose                                            |
| -------------------------- | ---------------------- | -------------------------------------------------- |
| `esp32c6_gesture_door.ino` | ESP32-C6 / Arduino IDE | Wi-Fi web server, servo control and OLED display   |
| `gesture_detector.html`    | Web Browser            | Webcam access, gesture detection and HTTP commands |

---

## Components Required

* ESP32-C6 development board
* SG90 or similar servo motor
* SSD1306 OLED display — 128×64, I2C
* Jumper wires
* Breadboard
* External 5V power supply for servo *(recommended)*
* Laptop/PC with webcam
* Chrome or Edge browser
* Wi-Fi network

---

## Hardware Connections

| Component        | Pin / Connection        |
| ---------------- | ----------------------- |
| **Servo Signal** | ESP32-C6 GPIO 2         |
| **Servo VCC**    | 5V / External 5V supply |
| **Servo GND**    | GND                     |
| **OLED SDA**     | ESP32-C6 GPIO 6         |
| **OLED SCL**     | ESP32-C6 GPIO 7         |
| **OLED VCC**     | 3.3V                    |
| **OLED GND**     | GND                     |

> **Note:** Pin numbers are defined at the beginning of `esp32c6_gesture_door.ino`. They can be changed if your ESP32-C6 board uses different GPIO connections.

> **Servo Power:** A separate 5V supply is recommended for the servo. If an external supply is used, make sure its **GND is connected to the ESP32-C6 GND**.

---

# Setup

## 1. Flash the ESP32-C6

Open `esp32c6_gesture_door.ino` in **Arduino IDE**.

Install the following libraries through:

**Sketch → Include Library → Manage Libraries**

* `ESP32Servo`
* `Adafruit SSD1306`
* `Adafruit GFX Library`

Also install the **ESP32 Arduino board package** by Espressif if it is not already installed.

Select:

**Tools → Board → ESP32 Arduino → ESP32C6 Dev Module**

### Configure Wi-Fi

Open the `.ino` file and enter your Wi-Fi credentials:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

Upload the code to the ESP32-C6.

Open **Serial Monitor** at **115200 baud**.

After connecting to Wi-Fi, the ESP32-C6 will display its IP address, for example:

```text
Connected! IP address: 10.83.156.53
```

**Note down this IP address.** It will be required in the browser.

### If Serial Monitor is blank

Go to:

**Tools → USB CDC On Boot → Enabled**

Then re-upload the code.

---

# 2. Run the Browser Gesture Detector

The gesture detector runs directly in the browser, so **Python, pip, or any additional installation is not required**.

A local web server is needed so that the browser can access the webcam.

### Using VS Code

1. Open the project folder in VS Code.
2. Install the **Live Server** extension by **Ritwick Dey**.
3. Right-click `gesture_detector.html`.
4. Select **Open with Live Server**.

The page will open at an address similar to:

```text
http://127.0.0.1:5500/gesture_detector.html
```

Allow camera access when prompted.

Enter the **ESP32-C6 IP address** shown in the Serial Monitor.

For example:

```text
10.83.156.53
```

Click **Test Connection**.

A successful connection should show something similar to:

```text
OK: {"door":"closed"}
```

Click **Start Camera** and perform the gestures:

| Gesture     | Action     |
| ----------- | ---------- |
| ✋ Open Palm | Open Door  |
| ✊ Fist      | Close Door |

> **Important:** Do not simply double-click the HTML file. Opening it using `file://` may prevent the browser from accessing the webcam. Use **Live Server** or another local web server.

---

# How It Works

The system consists of two main parts:

### Browser

The browser uses **MediaPipe Hand Landmarker** to process the webcam feed.

```text
Webcam
   ↓
MediaPipe
   ↓
Hand Gesture Detection
   ↓
HTTP Request
   ↓
ESP32-C6
```

The gesture detector identifies the user's hand and sends an HTTP request when the detected gesture changes.

### ESP32-C6

The ESP32-C6 runs a small HTTP server with the following endpoints:

| Endpoint                | Function                                      |
| ----------------------- | --------------------------------------------- |
| `GET /door?state=open`  | Moves servo to 90° and displays "Door Opened" |
| `GET /door?state=close` | Moves servo to 0° and displays "Door Closed"  |
| `GET /status`           | Returns the current door state as JSON        |

The ESP32-C6 also includes a **CORS header** so that the browser-based application can communicate with it from a different origin.

A **1.5-second cooldown** is used by the browser application to prevent repeated commands from being sent continuously.

---

# Troubleshooting

### Test Connection fails

* Make sure the PC and ESP32-C6 are connected to the **same Wi-Fi network**.
* Check the IP address in Serial Monitor.
* Make sure the latest `.ino` code containing the CORS header has been uploaded.

### Camera does not start

* Use **Live Server** instead of opening the HTML file directly.
* Allow camera permission in the browser.
* Close applications such as Zoom or Teams that may already be using the webcam.

### OLED does not display anything

Check:

* SDA → GPIO 6
* SCL → GPIO 7
* VCC → 3.3V
* GND → GND

The default I2C address is:

```text
0x3C
```

If the display does not work, try:

```text
0x3D
```

### Servo jitters or ESP32-C6 resets

The servo may be drawing more current than the ESP32-C6 power supply can provide.

Use a **separate regulated 5V supply** for the servo and connect the external supply's **GND to ESP32-C6 GND**.

### Serial Monitor is blank

Enable:

**Tools → USB CDC On Boot → Enabled**

Then upload the firmware again.

### ESP32-C6 IP address has changed

The router may assign a different IP address through DHCP.

Check the **Serial Monitor** for the latest IP address and enter that address in the browser application.

---

## System Overview

```text
              ┌──────────────────┐
              │   Laptop / PC     │
              │     Webcam        │
              └────────┬─────────┘
                       │
                       ▼
              ┌──────────────────┐
              │    MediaPipe     │
              │ Gesture Detection│
              └────────┬─────────┘
                       │ HTTP / Wi-Fi
                       ▼
              ┌──────────────────┐
              │    ESP32-C6      │
              │   HTTP Server    │
              └───────┬───┬──────┘
                      │   │
             ┌────────┘   └─────────┐
             ▼                       ▼
        ┌──────────┐           ┌──────────┐
        │   Servo  │           │   OLED   │
        │ Door Lock│           │  Status  │
        └──────────┘           └──────────┘
```
