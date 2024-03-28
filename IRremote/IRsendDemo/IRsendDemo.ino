/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.1 January, 2019
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <M5StickCPlus.h>

const uint16_t kIrLed = 33;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


// Example of data captured by IRrecvDumpV2.ino
// uint16_t rawData[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
//                         600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
//                         600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
//                         650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
//                         650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
//                         650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
//                         650, 1650, 650, 1650, 650, 1650, 600};

uint16_t rawData[67] = {4526, 4476,  576, 1660,  574, 1662,  568, 1670,  
                        574, 544,  576, 542,  568, 548,  572, 548,  576, 
                        542,  578, 1660,  572, 1666,  568, 1668,  574, 
                        542,  576, 540,  570, 548,  572, 546,  574, 546,  
                        576, 542,  570, 1666,  574, 544,  578, 540,  570, 
                        550,  570, 546,  574, 544,  576, 542,  570, 1666,  
                        576, 542,  568, 1670,  572, 1664,  568, 1668,  576, 1662,  
                        568, 1668,  574, 1660,  572};  // SAMSUNG TV

// Example Samsung A/C state captured from IRrecvDumpV2.ino
// uint8_t samsungState[kSamsungAcStateLength] = {
//     0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
//     0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0};

void setup() {
  irsend.begin();
  M5.begin();

  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf("Sending Start", 0);

  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_LED, OUTPUT); // setting the M5_LED as output mode
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
}

void loop() {
  // Serial.println("NEC");
  // irsend.sendNEC(0x00FFE01FUL);
  // delay(2000);
  // Serial.println("Sony");
  // irsend.sendSony(0xa90, 12, 2);  // 12 bits & 2 repeats
  // delay(2000);
  if(digitalRead(M5_BUTTON_HOME) == LOW){
    M5.Lcd.print("Sending Signal");
    irsend.sendRaw(rawData, 67, 38);
    digitalWrite(M5_LED, !digitalRead(M5_LED));
    while(digitalRead(M5_BUTTON_HOME) == LOW);
  }
  // Serial.println("a Samsung A/C state from IRrecvDumpV2");
  // irsend.sendSamsungAC(samsungState);
  // delay(2000);
}
