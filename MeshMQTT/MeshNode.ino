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

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else
#define LED G10
#endif

#define BLINK_PERIOD 3000  // milliseconds until cycle repeat
#define BLINK_DURATION 100 // milliseconds LED is on for

#define MESH_SSID "csc2106meshy"
#define MESH_PASSWORD "meshpotatoes"
#define MESH_PORT 5555

// Prototypes
void sendMessage();
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

void sendMessage();                                                // Prototype
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage); // start with a one second interval

// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;

void setup()
{
    Serial.begin(115200);
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);

    pinMode(LED, OUTPUT);

    mesh.setDebugMsgTypes(ERROR | DEBUG); // set before init() so that you can see error messages

    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);

    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();

    blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []()
                     {
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
      } });
    userScheduler.addTask(blinkNoNodes);
    blinkNoNodes.enable();

    randomSeed(analogRead(G10));
}

void loop()
{
    mesh.update();
    digitalWrite(LED, !onFlag);
}

void sendMessage()
{
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

void receivedCallback(uint32_t from, String &msg)
{
    static int printCount = 0;
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    if (printCount > 5)
    {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0);
        printCount = 0;
    }
    M5.Lcd.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    printCount++;

    // Trim the message and convert to lower case for reliable comparison
    msg.trim(); // Remove any leading/trailing whitespace
    String msgLower = msg;
    msgLower.toLowerCase(); // Convert to lower case

    if (msgLower == "on")
    {
        M5.Lcd.printf("The message ON is received");
        Serial.printf("RECEIVED: %s", msgLower);
        // Put on IR blaster code here
    }
    else if (msgLower == "off")
    {
        M5.Lcd.printf("The message OFF is received");
        Serial.printf("RECEIVED: %s", msgLower);
        // Put off IR blaster code here
    }
}

void newConnectionCallback(uint32_t nodeId)
{
    // Reset blink task
    onFlag = false;
    blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
    blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD * 1000)) / 1000);

    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
    Serial.print("This node ID: ");
    Serial.println(mesh.getNodeId());
}

void changedConnectionCallback()
{
    Serial.printf("Changed connections\n");
    // Reset blink task
    onFlag = false;
    blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
    blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD * 1000)) / 1000);

    nodes = mesh.getNodeList();

    Serial.printf("Num nodes: %d\n", nodes.size());
    Serial.printf("Connection list:");

    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end())
    {
        Serial.printf(" %u", *node);
        node++;
    }
    Serial.println();
    calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay)
{
    Serial.printf("Delay to node %u is %d us\n", from, delay);
}