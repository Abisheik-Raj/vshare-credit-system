# 🍱 VShare Smart Dustbin (IoT + Mobile + Cloud)

VShare Smart Dustbin is an IoT-enabled waste management system that incentivizes responsible disposal of food waste through a QR-based reward mechanism integrated with a mobile application.

The system ensures that users are rewarded only after verified disposal, preventing misuse and encouraging sustainable behavior.

---

## 🚀 Problem Statement

Food waste is a major issue in urban environments. Traditional disposal systems:

- Do not track contributions
- Provide no incentives
- Allow misuse of reward systems

---

## 💡 Solution

VShare introduces a **smart dustbin ecosystem** that:

- Verifies waste disposal using sensors
- Generates a unique QR code per deposit
- Rewards users via a mobile app
- Uses a cloud backend for synchronization

---

## 🏗️ System Architecture

```
+-------------------+        +---------------------+        +---------------------+
|   ESP32 Dustbin   | <----> |     Firebase DB     | <----> |   VShare Mobile App |
+-------------------+        +---------------------+        +---------------------+

Sensors → ESP32 → QR → Firebase → App Scan → Firebase → ESP32 → Lid Open
```

---

## 🔌 Hardware Components

- ESP32 (Main Controller)
- Load Cell + HX711 (Weight Detection)
- Ultrasonic Sensor (HC-SR04) (Bin Capacity Detection)
- Servo Motor (Lid Control)
- 2.4" SPI TFT Display (ILI9341 - QR Display)

---

## 🔗 Pin Configuration

| Component       | ESP32 Pin |
| --------------- | --------- |
| TFT CS          | 5         |
| TFT RST         | 4         |
| TFT DC          | 2         |
| Load Cell DOUT  | 32        |
| Load Cell SCK   | 33        |
| Ultrasonic TRIG | 25        |
| Ultrasonic ECHO | 26        |
| Servo Motor     | 27        |

---

## ⚙️ System Workflow

### 1. Trash Detection

- Load cell detects weight greater than threshold
- Ultrasonic sensor ensures bin has available space

---

### 2. QR Generation

A unique QR is generated using:

- Dustbin ID
- Timestamp
- Weight

Example:

```
dustbin_001_123456_0.75
```

---

### 3. QR Display

- QR code is rendered on the TFT display
- User scans it using the VShare mobile app

---

### 4. Firebase Sync (ESP32 → Cloud)

ESP32 updates Firebase:

```json
dustbins/dustbin_001
{
  "currentQR": "generated_qr",
  "isScanned": false
}
```

---

### 5. Mobile App Interaction

- App scans QR
- Validates QR uniqueness
- Updates Firebase:

```json
"isScanned": true
```

---

### 6. Polling Mechanism (ESP32)

ESP32 continuously checks:

```
dustbins/dustbin_001/isScanned
```

If `true`:

- Lid opens
- Trash is deposited
- Lid closes

---

## 🔐 Anti-Misuse Mechanism

- Each QR is **unique per transaction**
- Prevents duplicate scans
- Lid opens **only after scan confirmation**
- Future support for QR history tracking

---

## ☁️ Firebase Integration

### 🔹 Technology Used

- Firebase Realtime Database
- REST API using HTTPClient (ESP32)

---

### 🔹 Data Structure

```
dustbins/
   └── dustbin_001/
        ├── currentQR: "..."
        └── isScanned: false

qr_logs/ (future scope)
   └── qr_id/
        ├── used: true
        ├── weight: 0.75
        └── timestamp: ...
```

---

### 🔹 Communication Flow

| Component  | Action                |
| ---------- | --------------------- |
| ESP32      | Writes QR to Firebase |
| Mobile App | Reads QR              |
| Mobile App | Updates scan status   |
| ESP32      | Polls scan status     |

---

### 🔹 API Endpoints

**PUT (ESP32 updates QR):**

```
https://vshare-80324-default-rtdb.asia-southeast1.firebasedatabase.app/dustbins/dustbin_001.json
```

**GET (Polling scan status):**

```
https://vshare-80324-default-rtdb.asia-southeast1.firebasedatabase.app/dustbins/dustbin_001/isScanned.json
```

---

## 📱 Mobile Application (Flutter)

- QR code scanning
- Firebase integration
- Credit score update system
- User interaction with smart dustbins

---

## 🧠 Key Features

- Real-time IoT + Cloud communication
- Incentive-based waste disposal
- Sensor-driven validation
- QR-based secure interaction
- Scalable architecture

---

## 🎯 What This Project Achieves

- Reduces food wastage
- Encourages responsible disposal
- Builds a reward-based ecosystem
- Connects hardware and software seamlessly

---

## ⚠️ Limitations

- Uses polling instead of real-time listeners
- Load cell requires calibration
- Firebase security rules need enhancement

---

## 🔮 Future Enhancements

- 🔐 Firebase Authentication
- ⚡ Real-time database listeners (no polling)
- 🤖 AI-based waste classification
- 📊 Analytics dashboard
- 🏪 Partner-based reward integration

---

## ⚙️ Installation & Setup

```bash
git clone https://github.com/Abisheik-Raj/vshare-credit-system.git
```

### Steps:

1. Open `.ino` file in Arduino IDE
2. Install required libraries:
   - Adafruit ILI9341
   - Adafruit GFX
   - HX711
   - QRCode

3. Update:
   - WiFi credentials
   - Firebase URL

4. Upload to ESP32

---

## 👤 Author

**Abisheik Raj**
Integrated MTech Software Engineering Student @ VIT Chennai

---
