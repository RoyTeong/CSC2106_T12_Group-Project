// Allow over air update
// #define OTA_ENABLE true
#include "BaseOTA.h"

#include <Arduino.h>
#include <M5StickCPlus.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <algorithm>

// ==================== start of TUNEABLE PARAMETERS ====================
// An IR detector/demodulator is connected to GPIO pin 14
// e.g. D5 on a NodeMCU board.
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 33;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 33;
#endif  // ARDUINO_ESP32C3_DEV

// The Serial connection baud rate.
// i.e. Status message will be sent to the PC at this baud rate.
// Try to avoid slow speeds like 9600, as you will miss messages and
// cause other problems. 115200 (or faster) is recommended.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/decoder, let us use a larger
// than normal buffer so we can handle Air Conditioner remote codes.
const uint16_t kCaptureBufferSize = 1024;

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
// This parameter is an interesting trade-off. The longer the timeout, the more
// complex a message it can capture. e.g. Some device protocols will send
// multiple message packets in quick succession, like Air Conditioner remotes.
// Air Coniditioner protocols often have a considerable gap (20-40+ms) between
// packets.
// The downside of a large timeout value is a lot of less complex protocols
// send multiple messages when the remote's button is held down. The gap between
// them is often also around 20+ms. This can result in the raw data be 2-3+
// times larger than needed as it has captured 2-3+ messages in a single
// capture. Setting a low timeout value can resolve this.
// So, choosing the best kTimeout value for your use particular case is
// quite nuanced. Good luck and happy hunting.
// NOTE: Don't exceed kMaxTimeoutMs. Typically 130ms.
#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else   // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif  // DECODE_AC

const uint16_t kMinUnknownSize = 12;

// How much percentage lee way do we give to incoming signals in order to match
// it?
// e.g. +/- 25% (default) to an expected value of 500 would mean matching a
//      value between 375 & 625 inclusive.
// Note: Default is 25(%). Going to a value >= 50(%) will cause some protocols
//       to no longer match correctly. In normal situations you probably do not
//       need to adjust this value. Typically that's when the library detects
//       your remote's message some of the time, but not all of the time.
const uint8_t kTolerancePercentage = kTolerance;  // kTolerance is normally 25%

uint16_t TV_rawData[67] = {4526, 4476,  576, 1660,  574, 1662,  568, 1670,  
                        574, 544,  576, 542,  568, 548,  572, 548,  576, 
                        542,  578, 1660,  572, 1666,  568, 1668,  574, 
                        542,  576, 540,  570, 548,  572, 546,  574, 546,  
                        576, 542,  570, 1666,  574, 544,  578, 540,  570, 
                        550,  570, 546,  574, 544,  576, 542,  570, 1666,  
                        576, 542,  568, 1670,  572, 1664,  568, 1668,  576, 1662,  
                        568, 1668,  574, 1660,  572};  // SAMSUNG TV

uint16_t AC_ON_rawData[583] = {3406, 1688,  444, 1256,  450, 1248,  450, 404,  450, 396,  448, 400,  
                        440, 1260,  450, 400,  442, 408,  446, 1254,  442, 1254,  444, 404,  448, 
                        1252,  446, 402,  452, 398,  446, 1256,  450, 1248,  450, 400,  444, 1254,  
                        444, 1254,  442, 408,  446, 404,  448, 1250,  446, 402,  444, 406,  448, 
                        1254,  450, 396,  450, 400,  440, 410,  444, 402,  442, 412,  440, 402,  
                        450, 400,  442, 408,  446, 404,  448, 400,  444, 402,  452, 398,  444, 406,  
                        446, 400,  442, 410,  444, 402,  440, 410,  444, 404,  448, 404,  450, 396,  
                        446, 1256,  442, 404,  448, 400,  444, 406,  446, 402,  442, 408,  446, 1254,  
                        444, 1254,  442, 410,  442, 406,  446, 400,  446, 1254,  442, 1256,  442, 1256,  
                        450, 402,  440, 410,  444, 400,  442, 406,  446, 404,  450, 400,  444, 1256,  448, 
                        1248,  448, 400,  444, 1256,  450, 1248,  450, 404,  452, 394,  448, 1254,  444, 1252,  
                        446, 404,  448, 1250,  448, 1250,  448, 404,  448, 1252,  444, 400,  444, 406,  446, 404, 
                        448, 400,  444, 404,  448, 400,  444, 404,  446, 404,  450, 404,  440, 406,  448, 400,  
                        444, 404,  448, 400,  442, 412,  442, 406,  448, 400,  444, 406,  448, 400,  444, 406,  
                        448, 400,  442, 410,  444, 406,  448, 400,  444, 406,  446, 402,  442, 410,  442, 404,  
                        450, 400,  442, 408,  446, 400,  444, 410,  442, 404,  450, 402,  442, 406,  446, 404,  
                        440, 406,  446, 404,  442, 406,  444, 406,  448, 404,  440, 408,  444, 402,  440, 406,  
                        446, 402,  450, 400,  444, 408,  444, 402,  440, 410,  444, 404,  450, 398,  444, 408,  
                        444, 402,  442, 406,  446, 404,  440, 408,  442, 406,  448, 402,  444, 1258,  448, 398,  
                        444, 1254,  444, 408,  446, 402,  442, 1260,  446, 1252,  444, 1256,  440, 12728,  3406, 
                        1692,  448, 1252,  446, 1252,  446, 404,  448, 402,  442, 408,  444, 1254,  444, 402,  450, 
                        400,  442, 1256,  442, 1258,  448, 400,  444, 1256,  440, 408,  446, 404,  450, 1248,  448, 
                        1250,  448, 404,  448, 1252,  444, 1254,  444, 404,  440, 410,  442, 1260,  450, 396,  446, 
                        400,  442, 1258,  448, 402,  440, 406,  448, 400,  442, 406,  446, 404,  450, 402,  442, 404,  
                        448, 404,  450, 400,  444, 408,  444, 400,  444, 408,  444, 404,  448, 398,  448, 400,  440, 408,  
                        446, 406,  446, 402,  442, 404,  448, 402,  442, 1258,  448, 400,  442, 406,  446, 402,  444, 408,  
                        446, 400,  450, 1250,  448, 1254,  444, 408,  446, 398,  446, 402,  452, 1246,  448, 1254,  446, 1254,  
                        450, 398,  446, 404,  448, 400,  444, 404,  446, 402,  442, 410,  444, 1254,  444, 1252,  446, 402,  450, 
                        1250,  446, 1254,  444, 404,  448, 402,  442, 1258,  450, 1248,  450, 400,  442, 1258,  450, 1248,  450, 400,  
                        440, 1258,  450, 398,  446, 402,  450, 404,  438, 408,  446, 404,  452, 392,  446, 404,  450, 402,  442, 406,  
                        448, 398,  442, 410,  444, 402,  440, 412,  442, 406,  448, 400,  442, 408,  446, 402,  440, 410,  444, 410,  
                        444, 402,  450, 400,  444, 402,  450, 400,  444, 408,  444, 402,  442, 408,  448, 402,  450, 396,  448, 402,  
                        450, 400,  444, 406,  446, 404,  450, 398,  446, 406,  470, 376,  476, 378,  474, 370,  474, 376,  478, 374,  
                        478, 368,  476, 374,  470, 378,  476, 374,  478, 370,  472, 374,  478, 372,  470, 378,  476, 374,  478, 370,  
                        474, 378,  474, 374,  478, 370,  474, 376,  468, 380,  474, 380,  472, 376,  478, 370,  474, 1224,  474, 374,  
                        478, 1220,  478, 374,  478, 370,  472, 1224,  472, 1226,  472, 1228,  478};  // MITSUBISHI_AC_ON

uint16_t AC_OFF_rawData[583] = {3402, 1688,  442, 1256,  440, 1256,  454, 396,  446, 402,  450, 400,  444, 1254,  442, 410,  444, 400,  452, 1248,  450, 
                        1254,  442, 402,  448, 1252,  448, 400,  442, 406,  448, 1252,  446, 1252,  444, 404,  448, 1250,  446, 1252,  446, 404,  
                        450, 404,  450, 1246,  450, 396,  446, 404,  440, 1258,  452, 398,  444, 404,  448, 402,  452, 396,  446, 404,  450, 400,  
                        444, 406,  448, 400,  444, 406,  448, 404,  450, 396,  446, 404,  448, 398,  446, 404,  448, 404,  440, 406,  448, 400,  
                        442, 408,  444, 404,  448, 400,  444, 404,  450, 398,  444, 410,  444, 404,  450, 400,  444, 404,  450, 1252,  448, 1252,  
                        444, 404,  450, 402,  442, 404,  450, 1252,  444, 1256,  442, 1260,  448, 394,  448, 402,  440, 410,  444, 404,  448, 400,  
                        444, 404,  448, 1252,  448, 1252,  442, 404,  448, 1250,  448, 1252,  446, 402,  450, 400,  442, 1258,  450, 1248,  450, 398,  
                        444, 1256,  452, 1246,  450, 400,  442, 1254,  444, 406,  448, 400,  444, 410,  440, 406,  448, 398,  444, 404,  450, 398,  444, 
                        412,  442, 402,  442, 408,  444, 404,  450, 398,  444, 404,  448, 400,  444, 408,  444, 406,  448, 398,  446, 404,  448, 400,  
                        444, 410,  444, 400,  442, 406,  446, 404,  450, 400,  444, 404,  448, 400,  442, 406,  448, 404,  440, 408,  444, 404,  452, 396,  
                        444, 404,  450, 398,  442, 408,  446, 404,  452, 396,  446, 404,  450, 398,  444, 408,  444, 404,  450, 398,  444, 402,  450, 398,  
                        446, 404,  448, 402,  442, 408,  446, 402,  450, 398,  446, 406,  448, 398,  444, 410,  444, 400,  440, 410,  444, 404,  450, 398,  
                        446, 410,  444, 400,  442, 1256,  440, 408,  446, 1252,  444, 406,  446, 402,  450, 400,  444, 1254,  444, 1258,  452, 12722,  3400, 
                        1722,  418, 1280,  416, 1282,  416, 406,  446, 404,  450, 396,  446, 1256,  440, 404,  450, 400,  444, 1256,  450, 1278,  418, 400,  
                        444, 1256,  440, 410,  444, 406,  448, 1250,  446, 1282,  416, 402,  452, 1252,  444, 1280,  418, 400,  444, 406,  448, 1252,  444, 406,  
                        448, 400,  442, 1256,  442, 408,  442, 404,  450, 398,  444, 404,  448, 402,  442, 408,  444, 402,  450, 398,  444, 404,  448, 400,  444, 404,  
                        446, 404,  440, 410,  444, 404,  448, 402,  442, 406,  446, 400,  442, 412,  442, 406,  446, 400,  446, 402,  450, 398,  444, 406,  448, 404,  
                        450, 402,  442, 406,  446, 1250,  446, 1284,  414, 404,  448, 400,  446, 406,  446, 1250,  446, 1280,  418, 1282,  416, 404,  448, 400,  442, 406,  
                        448, 404,  452, 402,  440, 404,  448, 1252,  446, 1282,  414, 404,  450, 1250,  446, 1278,  418, 404,  450, 396,  446, 1254,  444, 1286,  422, 396,  
                        446, 1252,  444, 1286,  422, 402,  442, 1256,  440, 406,  450, 398,  444, 404,  448, 400,  444, 406,  446, 404,  448, 400,  442, 404,  450, 400,  444, 
                        404,  448, 406,  446, 398,  446, 404,  450, 398,  442, 406,  450, 402,  450, 396,  446, 402,  440, 408,  444, 404,  450, 400,  442, 406,  448, 400,  444, 
                        408,  444, 402,  450, 400,  444, 404,  450, 398,  444, 408,  444, 406,  448, 398,  446, 402,  452, 398,  442, 406,  450, 398,  442, 408,  446, 404,  452, 398,  
                        444, 404,  448, 400,  442, 406,  450, 404,  450, 396,  446, 402,  452, 400,  442, 404,  448, 404,  450, 396,  448, 402,  450, 398,  446, 406,  448, 398,  444, 408,  
                        446, 402,  440, 408,  448, 402,  450, 400,  442, 1258,  500, 346,  448, 1252,  498, 350,  452, 396,  444, 404,  448, 1252,  498, 1228,  470};  // MITSUBISHI_AC_OFF

// Legacy (No longer supported!)
//
// Change to `true` if you miss/need the old "Raw Timing[]" display.
#define LEGACY_TIMING_INFO false
// ==================== end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;  // Somewhere to store the results

// This section of code runs only once at start-up.
void setup() {
  OTAwifi();  // start default wifi (previously saved on the ESP) for OTA
  M5.begin();

  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf("Receiving Start", 0);
#if defined(ESP8266)
  Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(kBaudRate, SERIAL_8N1);
#endif  // ESP8266
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  // Perform a low level sanity checks that the compiler performs bit field
  // packing as we expect and Endianness is as we expect.
  assert(irutils::lowLevelSanityCheck() == 0);

  Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
  OTAinit();  // setup OTA handlers and show IP
#if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif  // DECODE_HASH
  irrecv.setTolerance(kTolerancePercentage);  // Override the default tolerance.
  irrecv.enableIRIn();  // Start the receiver
}

// The repeating section of the code
void loop() {
  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
    // Check if we got an IR message that was to big for our capture buffer.
    if (results.overflow)
      Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
    // Display the library version the message was captured with.
    Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_STR "\n");
    // Display the tolerance percentage if it has been change from the default.
    if (kTolerancePercentage != kTolerance)
      Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));

    // Compare the received raw data with each expected raw data
    bool isTVMatch = std::equal(std::begin(results.rawbuf), std::end(results.rawbuf), std::begin(TV_rawData));
    bool isAC_ON_Match = std::equal(std::begin(results.rawbuf), std::end(results.rawbuf), std::begin(AC_ON_rawData));
    bool isAC_OFF_Match = std::equal(std::begin(results.rawbuf), std::end(results.rawbuf), std::begin(AC_OFF_rawData));
    
    if (isTVMatch) {
        M5.Lcd.printf("Received TV signal", 0);
    } else if (isAC_ON_Match) {
        M5.Lcd.printf("Received AIRCON ON signal", 0);
    } else if (isAC_OFF_Match) {
        M5.Lcd.printf("Received AIRCON OFF signal", 0);
    } 

    // Display any extra A/C info if we have it.
    String description = IRAcUtils::resultAcToString(&results);
    if (description.length()) Serial.println(D_STR_MESGDESC ": " + description);
    yield();  // Feed the WDT as the text output can take a while to print.
#if LEGACY_TIMING_INFO
    // Output legacy RAW timing info of the result.
    Serial.println(resultToTimingInfo(&results));
    yield();  // Feed the WDT (again)
#endif  // LEGACY_TIMING_INFO
    // Output the results as source code
    Serial.println(resultToSourceCode(&results));
    Serial.println();    // Blank line between entries
    yield();             // Feed the WDT (again)
  }
  OTAloopHandler();
}
