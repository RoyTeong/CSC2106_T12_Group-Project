#include "NimBLEDevice.h"
#include "M5StickCPlus2.h"


M5Canvas img(&StickCP2.Display);
M5Canvas clc(&StickCP2.Display);

// The remote service we wish to connect to.
static BLEUUID serviceUUID("ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("ebe0ccc1-7a0a-4b0c-8a1a-6ff2997da3a6");

BLEClient *pClient;

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
std::string add[2]={"a4:c1:38:64:98:97","a4:c1:38:4d:8d:e3"};
 String rooms[2]={"KITCHEN","LIVING ROOM"};

  float temp[2];
  float humi[2];
  float voltage[2];
 
 
  bool ch=0;

  int posx[2]={5,5};
  int posy[2]={5,70};
  unsigned short blue=0x024E;
  unsigned short blue2=0x01EC; 
  unsigned short blue3=0x469F;
  unsigned short grays[13];
  int vol=0;
  int volE; 
  unsigned long tt=0;
  unsigned long tt2=0;
  int period=1000;
  int s=0;
  bool freeL[4]={1,1,1,1};

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for SENSOR 0 ");
  freeL[0]=0;
  temp[0] = (pData[0] | (pData[1] << 8)) * 0.01; //little endian
  humi[0] = pData[2];
  voltage[0] = (pData[3] | (pData[4] << 8)) * 0.001; //little endian
  Serial.printf("temp = %.1f C ; humidity = %.1f %% ; voltage = %.3f V\n", temp[0], humi[0], voltage[0]);
  Serial.println();
  freeL[0]=1;
}

static void notifyCallback2(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for SENSOR 1 ");
  
  freeL[1]=0;
  temp[1] = (pData[0] | (pData[1] << 8)) * 0.01; //little endian
  humi[1] = pData[2];
  voltage[1] = (pData[3] | (pData[4] << 8)) * 0.001; //little endian
  Serial.printf("temp = %.1f C ; humidity = %.1f %% ; voltage = %.3f V\n", temp[1], humi[1], voltage[1]);

  Serial.println();
  freeL[1]=1;
}

bool connectToServer(BLEAddress pAddress) {
  Serial.print("Forming a connection to ");
  Serial.println(pAddress.toString().c_str());

  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    return false;
  }
  Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  std::string value = pRemoteCharacteristic->readValue();
  Serial.print("The characteristic value was: ");
  Serial.println(value.c_str());

  if(pClient->getPeerAddress().toString().c_str()==add[0])
  pRemoteCharacteristic->registerForNotify(notifyCallback);

  if(pClient->getPeerAddress().toString().c_str()==add[1])
  pRemoteCharacteristic->registerForNotify(notifyCallback2);


  connected = true;//added
  return true;//added
}


void setup() {

 auto cfg = M5.config();
    StickCP2.begin(cfg);
    //StickCP2.Rtc.setDateTime( { { 2023, 12, 30 }, { 22, 43, 0 } } );
   
    StickCP2.Display.setBrightness(38);
    StickCP2.Display.setRotation(3);
    StickCP2.Display.fillScreen(RED);
    img.createSprite(170,180);
    clc.createSprite(60,170);
    clc.setSwapBytes(true);
  
  Serial.begin(9600);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

    /*  int co=220;
     for(int i=0;i<13;i++)
     {grays[i]=StickCP2.Display.color565(co, co, co);
     co=co-20;} */

  connectToServer(BLEAddress(add[0]));
  connectToServer(BLEAddress(add[1]));

  StickCP2.Display.fillScreen(BLACK);

} // End of setup.


const uint8_t notificationOff[] = {0x0, 0x0};
const uint8_t notificationOn[] = {0x1, 0x0};
bool onoff = true;


void draw() {
    StickCP2.Display.setBrightness(100);
    img.fillSprite(BLACK);

    // Print values for the first device only
    
  for(int i=0;i<2;i++)
  {
  
    img.setTextDatum(middle_center);
    img.setTextColor(WHITE, blue);

    img.drawString("Temp: " + String(temp[i]) + "C", posx[i] + 50, posy[i] + 20);
    img.drawString("Humidity: " + String((int)humi[i]) + "%", posx[i] + 50, posy[i] + 32);
   

    img.unloadFont();
    img.setTextDatum(0);

    img.setTextColor(blue3, blue);
    img.drawString(rooms[i], posx[i] + 4, posy[i] + 2);
    
  }
    img.unloadFont();
    img.pushSprite(0, 0);
}



void loop() {


    if(millis()>tt+period){
    vol = StickCP2.Power.getBatteryVoltage();
    volE=map(vol,3000,4180,0,5); 
    if(freeL[0]==1 && freeL[1]==1 && freeL[0]==1 && freeL[3]==1)
    draw();
    tt=millis();
    }
}