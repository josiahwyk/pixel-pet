// I2C scanner: finds the OLED's address.
// Open Serial Monitor at 115200 baud to see the result.
#include <Wire.h>

const int PIN_SDA = 21;
const int PIN_SCL = 22;

void setup() {
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
  delay(500);
}

void loop() {
  int found = 0;
  Serial.println("Scanning...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at 0x%02X\n", addr);
      found++;
    }
  }
  if (found == 0) Serial.println("No I2C devices found. Check wiring.");
  delay(3000);
}
