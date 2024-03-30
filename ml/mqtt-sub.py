import datetime
import csv
import time
import paho.mqtt.client as mqtt

topics =  'light/status'

curr_light_status = False

def update_csv(light_status):
    with open("home_assistant_log.csv", "a", newline='') as file:
        writer = csv.writer(file)
        writer.writerow([light_status, datetime.datetime.now()])
        file.close()

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    print("mqtt-sub.py currently subscribing to {}".format(topics))
    client.subscribe(topics)

def on_message(client, userdata, msg):
    recieved = msg.payload.decode('utf-8')
    print(msg.topic+" "+str(recieved))
    update_csv(recieved)



client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message
client.on_connect = on_connect
#client.username_pw_set("homeassistant","eu0Vabee3iegaeRee4Voo9ozohtorahZeighoosai7soovae2ohcu8quahTi4iej")
#client.connect("192.168.1.1",1883,60)
client.connect('localhost',1883,60)
client.subscribe(topics)
client.loop_forever()
