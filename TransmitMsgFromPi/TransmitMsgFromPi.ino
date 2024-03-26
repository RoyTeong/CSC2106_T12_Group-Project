#include <M5StickCPlus.h>

void setup() {
  M5.begin();
  Serial.begin(115200);
  Serial.setTimeout(500);
}

void loop() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print(message);
  }
}
