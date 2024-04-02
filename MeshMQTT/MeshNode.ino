//************************************************************
// this is a simple example that uses the easyMesh library
//
// 1. blinks led once for every node on the mesh
// 2. blink cycle repeats every BLINK_PERIOD
// 3. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 4. prints anything it receives to Serial.print
//
//
//************************************************************
#include <painlessMesh.h>
#include "M5StickCPlus.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else
#define LED G10
#endif

#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  100  // milliseconds LED is on for

#define   MESH_SSID       "csc2106meshy"
#define   MESH_PASSWORD   "meshpotatoes"
#define   MESH_PORT       5555

const uint16_t kIrLed = 33;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

bool ACState = false;

uint16_t tv_rawData[67] = {4526, 4476,  576, 1660,  574, 1662,  568, 1670,  
                        574, 544,  576, 542,  568, 548,  572, 548,  576, 
                        542,  578, 1660,  572, 1666,  568, 1668,  574, 
                        542,  576, 540,  570, 548,  572, 546,  574, 546,  
                        576, 542,  570, 1666,  574, 544,  578, 540,  570, 
                        550,  570, 546,  574, 544,  576, 542,  570, 1666,  
                        576, 542,  568, 1670,  572, 1664,  568, 1668,  576, 1662,  
                        568, 1668,  574, 1660,  572};  // SAMSUNG TV

uint16_t led_on_rawData[71] = {9548, 4450,  622, 548,  624, 546,  626, 546,  616, 556,  616, 556,  628, 548,  624, 550,  622, 554,  618, 1662,  622, 1656,  618, 1662,  620, 1664,  620, 1666,  628, 1652,  618, 1660,  624, 1660,  622, 1658,  624, 550,  624, 554,  628, 546,  626, 550,  620, 554,  630, 1660,  622, 560,  624, 556,  624, 1658,  626, 1658,  626, 1656,  626, 1658,  616, 1664,  620, 554,  616, 1664,  622, 38404,  9504, 2216,  622};

uint16_t led_off_rawData[71] = {9468, 4442,  620, 548,  612, 554,  616, 550,  622, 546,  616, 552,  622, 548,  614, 554,  618, 552,  620, 1652,  622, 1652,  622, 1652,  620, 1650,  612, 1660,  614, 1660,  614, 1658,  616, 1658,  616, 552,  618, 548,  616, 550,  620, 544,  616, 552,  622, 544,  616, 1656,  618, 552,  620, 1650,  612, 1658,  614, 1656,  618, 1654,  620, 1650,  622, 1648,  614, 554,  618, 1654,  618, 38218,  9436, 2210,  618};

// uint16_t fan_rawData[103] = {3556, 3530,  894, 2652,  888, 2654,  894, 878, 894, 
//                         2650,  888, 2654, 894, 2650, 890, 880,  888, 882,  896, 876,  
//                         892, 880,  888, 882,  888, 2654, 896, 878,  892, 878,  890, 2652, 
//                         896, 874,  894, 878,  892, 878,  890, 2652,  898, 2646,  892, 2650,  
//                         898, 2646,  892, 2650,  888, 884,  896, 39498,  3556, 3530,  892, 2650,  
//                         890, 2654,  896, 876,  892, 2652,  888, 2652,  898, 2646,  890, 908,  864, 
//                         908,  860, 912,  860, 912,  868, 904,  864, 2650,  888, 912,  868, 902,  868, 
//                         2648,  888, 912,  856, 916,  864, 908,  864, 2650,  888, 2654,  894, 2652,  
//                         886, 2656,  896, 2650,  888, 882,  886};  // KOK WALL FAN

// uint16_t AC_ON_rawData[583] = {3406, 1688,  444, 1256,  450, 1248,  450, 404,  450, 396,  448, 400,  
//                         440, 1260,  450, 400,  442, 408,  446, 1254,  442, 1254,  444, 404,  448, 
//                         1252,  446, 402,  452, 398,  446, 1256,  450, 1248,  450, 400,  444, 1254,  
//                         444, 1254,  442, 408,  446, 404,  448, 1250,  446, 402,  444, 406,  448, 
//                         1254,  450, 396,  450, 400,  440, 410,  444, 402,  442, 412,  440, 402,  
//                         450, 400,  442, 408,  446, 404,  448, 400,  444, 402,  452, 398,  444, 406,  
//                         446, 400,  442, 410,  444, 402,  440, 410,  444, 404,  448, 404,  450, 396,  
//                         446, 1256,  442, 404,  448, 400,  444, 406,  446, 402,  442, 408,  446, 1254,  
//                         444, 1254,  442, 410,  442, 406,  446, 400,  446, 1254,  442, 1256,  442, 1256,  
//                         450, 402,  440, 410,  444, 400,  442, 406,  446, 404,  450, 400,  444, 1256,  448, 
//                         1248,  448, 400,  444, 1256,  450, 1248,  450, 404,  452, 394,  448, 1254,  444, 1252,  
//                         446, 404,  448, 1250,  448, 1250,  448, 404,  448, 1252,  444, 400,  444, 406,  446, 404, 
//                         448, 400,  444, 404,  448, 400,  444, 404,  446, 404,  450, 404,  440, 406,  448, 400,  
//                         444, 404,  448, 400,  442, 412,  442, 406,  448, 400,  444, 406,  448, 400,  444, 406,  
//                         448, 400,  442, 410,  444, 406,  448, 400,  444, 406,  446, 402,  442, 410,  442, 404,  
//                         450, 400,  442, 408,  446, 400,  444, 410,  442, 404,  450, 402,  442, 406,  446, 404,  
//                         440, 406,  446, 404,  442, 406,  444, 406,  448, 404,  440, 408,  444, 402,  440, 406,  
//                         446, 402,  450, 400,  444, 408,  444, 402,  440, 410,  444, 404,  450, 398,  444, 408,  
//                         444, 402,  442, 406,  446, 404,  440, 408,  442, 406,  448, 402,  444, 1258,  448, 398,  
//                         444, 1254,  444, 408,  446, 402,  442, 1260,  446, 1252,  444, 1256,  440, 12728,  3406, 
//                         1692,  448, 1252,  446, 1252,  446, 404,  448, 402,  442, 408,  444, 1254,  444, 402,  450, 
//                         400,  442, 1256,  442, 1258,  448, 400,  444, 1256,  440, 408,  446, 404,  450, 1248,  448, 
//                         1250,  448, 404,  448, 1252,  444, 1254,  444, 404,  440, 410,  442, 1260,  450, 396,  446, 
//                         400,  442, 1258,  448, 402,  440, 406,  448, 400,  442, 406,  446, 404,  450, 402,  442, 404,  
//                         448, 404,  450, 400,  444, 408,  444, 400,  444, 408,  444, 404,  448, 398,  448, 400,  440, 408,  
//                         446, 406,  446, 402,  442, 404,  448, 402,  442, 1258,  448, 400,  442, 406,  446, 402,  444, 408,  
//                         446, 400,  450, 1250,  448, 1254,  444, 408,  446, 398,  446, 402,  452, 1246,  448, 1254,  446, 1254,  
//                         450, 398,  446, 404,  448, 400,  444, 404,  446, 402,  442, 410,  444, 1254,  444, 1252,  446, 402,  450, 
//                         1250,  446, 1254,  444, 404,  448, 402,  442, 1258,  450, 1248,  450, 400,  442, 1258,  450, 1248,  450, 400,  
//                         440, 1258,  450, 398,  446, 402,  450, 404,  438, 408,  446, 404,  452, 392,  446, 404,  450, 402,  442, 406,  
//                         448, 398,  442, 410,  444, 402,  440, 412,  442, 406,  448, 400,  442, 408,  446, 402,  440, 410,  444, 410,  
//                         444, 402,  450, 400,  444, 402,  450, 400,  444, 408,  444, 402,  442, 408,  448, 402,  450, 396,  448, 402,  
//                         450, 400,  444, 406,  446, 404,  450, 398,  446, 406,  470, 376,  476, 378,  474, 370,  474, 376,  478, 374,  
//                         478, 368,  476, 374,  470, 378,  476, 374,  478, 370,  472, 374,  478, 372,  470, 378,  476, 374,  478, 370,  
//                         474, 378,  474, 374,  478, 370,  474, 376,  468, 380,  474, 380,  472, 376,  478, 370,  474, 1224,  474, 374,  
//                         478, 1220,  478, 374,  478, 370,  472, 1224,  472, 1226,  472, 1228,  478};  // MITSUBISHI_AC_ON

// uint16_t AC_OFF_rawData[583] = {3402, 1688,  442, 1256,  440, 1256,  454, 396,  446, 402,  450, 400,  444, 1254,  442, 410,  444, 400,  452, 1248,  450, 
//                         1254,  442, 402,  448, 1252,  448, 400,  442, 406,  448, 1252,  446, 1252,  444, 404,  448, 1250,  446, 1252,  446, 404,  
//                         450, 404,  450, 1246,  450, 396,  446, 404,  440, 1258,  452, 398,  444, 404,  448, 402,  452, 396,  446, 404,  450, 400,  
//                         444, 406,  448, 400,  444, 406,  448, 404,  450, 396,  446, 404,  448, 398,  446, 404,  448, 404,  440, 406,  448, 400,  
//                         442, 408,  444, 404,  448, 400,  444, 404,  450, 398,  444, 410,  444, 404,  450, 400,  444, 404,  450, 1252,  448, 1252,  
//                         444, 404,  450, 402,  442, 404,  450, 1252,  444, 1256,  442, 1260,  448, 394,  448, 402,  440, 410,  444, 404,  448, 400,  
//                         444, 404,  448, 1252,  448, 1252,  442, 404,  448, 1250,  448, 1252,  446, 402,  450, 400,  442, 1258,  450, 1248,  450, 398,  
//                         444, 1256,  452, 1246,  450, 400,  442, 1254,  444, 406,  448, 400,  444, 410,  440, 406,  448, 398,  444, 404,  450, 398,  444, 
//                         412,  442, 402,  442, 408,  444, 404,  450, 398,  444, 404,  448, 400,  444, 408,  444, 406,  448, 398,  446, 404,  448, 400,  
//                         444, 410,  444, 400,  442, 406,  446, 404,  450, 400,  444, 404,  448, 400,  442, 406,  448, 404,  440, 408,  444, 404,  452, 396,  
//                         444, 404,  450, 398,  442, 408,  446, 404,  452, 396,  446, 404,  450, 398,  444, 408,  444, 404,  450, 398,  444, 402,  450, 398,  
//                         446, 404,  448, 402,  442, 408,  446, 402,  450, 398,  446, 406,  448, 398,  444, 410,  444, 400,  440, 410,  444, 404,  450, 398,  
//                         446, 410,  444, 400,  442, 1256,  440, 408,  446, 1252,  444, 406,  446, 402,  450, 400,  444, 1254,  444, 1258,  452, 12722,  3400, 
//                         1722,  418, 1280,  416, 1282,  416, 406,  446, 404,  450, 396,  446, 1256,  440, 404,  450, 400,  444, 1256,  450, 1278,  418, 400,  
//                         444, 1256,  440, 410,  444, 406,  448, 1250,  446, 1282,  416, 402,  452, 1252,  444, 1280,  418, 400,  444, 406,  448, 1252,  444, 406,  
//                         448, 400,  442, 1256,  442, 408,  442, 404,  450, 398,  444, 404,  448, 402,  442, 408,  444, 402,  450, 398,  444, 404,  448, 400,  444, 404,  
//                         446, 404,  440, 410,  444, 404,  448, 402,  442, 406,  446, 400,  442, 412,  442, 406,  446, 400,  446, 402,  450, 398,  444, 406,  448, 404,  
//                         450, 402,  442, 406,  446, 1250,  446, 1284,  414, 404,  448, 400,  446, 406,  446, 1250,  446, 1280,  418, 1282,  416, 404,  448, 400,  442, 406,  
//                         448, 404,  452, 402,  440, 404,  448, 1252,  446, 1282,  414, 404,  450, 1250,  446, 1278,  418, 404,  450, 396,  446, 1254,  444, 1286,  422, 396,  
//                         446, 1252,  444, 1286,  422, 402,  442, 1256,  440, 406,  450, 398,  444, 404,  448, 400,  444, 406,  446, 404,  448, 400,  442, 404,  450, 400,  444, 
//                         404,  448, 406,  446, 398,  446, 404,  450, 398,  442, 406,  450, 402,  450, 396,  446, 402,  440, 408,  444, 404,  450, 400,  442, 406,  448, 400,  444, 
//                         408,  444, 402,  450, 400,  444, 404,  450, 398,  444, 408,  444, 406,  448, 398,  446, 402,  452, 398,  442, 406,  450, 398,  442, 408,  446, 404,  452, 398,  
//                         444, 404,  448, 400,  442, 406,  450, 404,  450, 396,  446, 402,  452, 400,  442, 404,  448, 404,  450, 396,  448, 402,  450, 398,  446, 406,  448, 398,  444, 408,  
//                         446, 402,  440, 408,  448, 402,  450, 400,  442, 1258,  500, 346,  448, 1252,  498, 350,  452, 396,  444, 404,  448, 1252,  498, 1228,  470};  // MITSUBISHI_AC_OFF


// Prototypes
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

void sendMessage() ; // Prototype
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;

void setup() {
  Serial.begin(115200);
  irsend.begin();
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);

  pinMode(LED, OUTPUT);

  #if ESP8266
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
    Serial.begin(115200, SERIAL_8N1);
  #endif  // ESP8266

  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      // If on, switch off, else switch on
      if (onFlag)
        onFlag = false;
      else
        onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);

      if (blinkNoNodes.isLastIteration()) {
        // Finished blinking. Reset task for next run 
        // blink number of nodes (including this node) times
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        // Calculate delay based on current mesh time and BLINK_PERIOD
        // This results in blinks between nodes being synced
        blinkNoNodes.enableDelayed(BLINK_PERIOD - 
            (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();

  randomSeed(analogRead(G10));
}

void loop() {
  mesh.update();
  digitalWrite(LED, !onFlag);
}

void sendMessage() {
  // String msg = "Hello from ID: ";
  // msg += mesh.getNodeId();
  // msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  // mesh.sendBroadcast(msg);

  // if (calc_delay) {
  //   SimpleList<uint32_t>::iterator node = nodes.begin();
  //   while (node != nodes.end()) {
  //     mesh.startDelayMeas(*node);
  //     node++;
  //   }
  //   calc_delay = false;
  // }

  // Serial.printf("Sending message: %s\n", msg.c_str());
  
  // taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String &msg) {
  static int printCount = 0;
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if (printCount > 5) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    printCount = 0;
  }
  // M5.Lcd.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  printCount++;

  // Trim the message and convert to lower case for reliable comparison
  msg.trim(); // Remove any leading/trailing whitespace
  String msgLower = msg;
  msgLower.toLowerCase(); // Convert to lower case

  if (msgLower == "ontv") {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Turning on TV...");
    Serial.printf("RECEIVED: %s", msgLower);
    irsend.sendRaw(tv_rawData, 67, 38);
  } else if (msgLower == "offtv") {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Turning off TV...");
    Serial.printf("RECEIVED: %s", msgLower);
    irsend.sendRaw(tv_rawData, 67, 38);
    // Put off IR blaster code here
  } else if (msgLower == "onled") {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Turning ON LED...");
    Serial.printf("RECEIVED: %s", msgLower);
    irsend.sendRaw(led_on_rawData, 71, 38);
  } else if (msgLower == "offled") {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Turning OFF LED...");
    Serial.printf("RECEIVED: %s", msgLower);
    irsend.sendRaw(led_off_rawData, 71, 38);
  }
}


void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
  Serial.print("This node ID: ");
  Serial.println(mesh.getNodeId());

}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
