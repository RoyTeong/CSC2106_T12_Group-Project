# CSC2106_T12_Group-Project

### GROUP MEMBERS:
1. Tan Chang Yuan Jasper 2200620
2. Shaun Tay Jia Le 2200555
3. Koh Zheng Wei 2200798
4. Roy Teong Ying Jun 2200507
5. Loo Siong Yu 2201255

## System Overview
![image](https://github.com/RoyTeong/CSC2106_T12_Group-Project/assets/34854761/2165dfbe-7044-40b3-b5d3-eae7f40a593d)

# Setting up Home Assistant 
## Installing Home Assistant on Raspberry Pi
1. Follow steps listed on this website [Instructions](https://www.home-assistant.io/installation/raspberrypi/)
2. After home assistant has been successfully installed, download [homeassistant_backup.tar](https://github.com/RoyTeong/CSC2106_T12_Group-Project/blob/main/homeassistant_backup.tar)
3. Go to Settings > System > Backups 
4. Upload the backup and restore it
5. Username: "jasper" Password: "123456789iI@"
## MQTT
### Topics 
#### light/status
- This topic will be used to send commands to turn on or off the room light
- The machine learning Pi will also be getting its data from this topic 
#### device/trigger 
- This topic is used to send commands to our mesh network through the MQTT serial M5 Stick 
- e.g. "634095713:ontv"
- The data in front of the : will be the node ID and the data behind the : are the trigger commands 
## User Controller 
- Run [user_interface.py](https://github.com/RoyTeong/CSC2106_T12_Group-Project/blob/main/ui/user_interface.py)
```shell
python3 user_interface.py
```
## Mesh Setup
### MeshSerialTransmit.ino
- This will be flashed to a M5 Stick, it handles connecting to the MQTT broker and receiving messages from it 
- It will then send the message using serial UART to the "MeshNodeMain.ino"
- Update the WiFi and MQTT broker in line 6 - 15 accordingly to your setup
### MeshNodeMain.ino
- This will be flashed to a M5 Stick, it will handle receiving the serial data and sending it out to the Mesh network
### MeshNode.ino
- This will be flashed to a M5 Stick that has a IR emitter connected to it
- IR raw data will be here
