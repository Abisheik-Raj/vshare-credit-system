#include "HX711.h"

#define DOUT  4
#define SCK   5

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT, SCK);
  scale.set_scale();     // No scaling yetc:\Users\Abisheik Raj\OneDrive\Desktop\Message Box\message
  scale.tare();          // Reset to 0
  Serial.println("Place a known weight after 5 seconds...");
  delay(5000);
}

void loop() {
  Serial.print("Raw reading: ");
  Serial.println(scale.get_value(10));  // Average of 10 readings
  delay(1000);
}
