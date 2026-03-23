/****************************************************
 * VShare Smart Dustbin - ESP32 Firmware (FINAL)
 * Features:
 * - Load Cell (HX711)
 * - Ultrasonic Sensor (HC-SR04)
 * - Servo Lid Control
 * - TFT Display (ILI9341) showing QR
 * - Firebase Polling with Timeout + Reset
 ****************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Servo.h>
#include "HX711.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <QRCode.h>

/******************** PIN CONFIG ********************/
#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2

#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN 33

#define TRIG_PIN 25
#define ECHO_PIN 26

#define SERVO_PIN 27

/******************** WIFI CONFIG ********************/
const char *ssid = "YOUR_WIFI";
const char *password = "YOUR_PASSWORD";

/******************** FIREBASE CONFIG ********************/
String FIREBASE_HOST = "https://vshare-80324-default-rtdb.asia-southeast1.firebasedatabase.app/";
String DUSTBIN_ID = "dustbin_001";

String FIELD_SCANNED = "isScanned";

/******************** OBJECTS ********************/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
HX711 scale;
Servo lidServo;

/******************** VARIABLES ********************/
float calibration_factor = 2280;

/******************** FUNCTIONS ********************/

// Ultrasonic
float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0)
    return 999;

  return duration * 0.034 / 2;
}

// Load Cell
float getWeight()
{
  float w = scale.get_units(5);
  if (w < 0)
    w = 0;
  return w;
}

// QR Generation
String generateQR(float weight)
{
  return DUSTBIN_ID + "_" + String(millis()) + "_" + String(weight, 2);
}

// Display QR
void displayQR(String data)
{
  tft.fillScreen(ILI9341_BLACK);

  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, data.c_str());

  int scale = 4;

  for (uint8_t y = 0; y < qrcode.size; y++)
  {
    for (uint8_t x = 0; x < qrcode.size; x++)
    {
      if (qrcode_getModule(&qrcode, x, y))
      {
        tft.fillRect(x * scale, y * scale, scale, scale, ILI9341_WHITE);
      }
    }
  }
}

// Firebase: Update QR
void updateQRToFirebase(String qr)
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  String url = FIREBASE_HOST + "dustbins/" + DUSTBIN_ID + ".json";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"currentQR\":\"" + qr + "\",\"isScanned\":false}";
  http.PUT(payload);

  http.end();
}

// Firebase: Check scan
bool isQRScanned()
{
  if (WiFi.status() != WL_CONNECTED)
    return false;

  HTTPClient http;
  String url = FIREBASE_HOST + "dustbins/" + DUSTBIN_ID + "/" + FIELD_SCANNED + ".json";

  http.begin(url);
  int code = http.GET();

  if (code == 200)
  {
    String res = http.getString();
    http.end();
    return res == "true";
  }

  http.end();
  return false;
}

// Firebase: Reset scan
void resetScanStatus()
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  String url = FIREBASE_HOST + "dustbins/" + DUSTBIN_ID + ".json";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  http.PATCH("{\"isScanned\":false}");
  http.end();
}

// Servo
void openLid()
{
  lidServo.write(90);
}

void closeLid()
{
  lidServo.write(0);
}

/******************** SETUP ********************/
void setup()
{
  Serial.begin(115200);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // TFT
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  // Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  lidServo.attach(SERVO_PIN);
  closeLid();
}

/******************** LOOP ********************/
void loop()
{

  // Ensure WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconnecting WiFi...");
    WiFi.begin(ssid, password);
    delay(3000);
    return;
  }

  float weight = getWeight();
  float distance = getDistance();

  Serial.println("Weight: " + String(weight));
  Serial.println("Distance: " + String(distance));

  if (weight > 0.2 && distance > 10)
  {

    String qr = generateQR(weight);

    displayQR(qr);
    updateQRToFirebase(qr);

    Serial.println("QR Generated: " + qr);

    // Wait for scan (timeout 30 sec)
    unsigned long start = millis();
    bool scanned = false;

    while (millis() - start < 30000)
    {
      if (isQRScanned())
      {
        scanned = true;
        break;
      }
      Serial.println("Waiting for scan...");
      delay(2000);
    }

    if (!scanned)
    {
      Serial.println("Scan timeout. Skipping...");
      return;
    }

    Serial.println("QR Scanned!");

    openLid();
    delay(3000);
    closeLid();

    resetScanStatus();

    Serial.println("Trash deposited");

    delay(5000); // cooldown
  }

  delay(1000);
}