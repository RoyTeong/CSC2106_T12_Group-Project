import paho.mqtt.client as mqtt
import csv
import datetime
import time

topics = {
    'request': 'light/status',
    'response': 'light/trigger'
}

curr_light_status = False

def update_csv(light_status):
    with open("home_assistant_log.csv", "a", newline='') as file:
        writer = csv.writer(file)
        writer.writerow([light_status, datetime.datetime.now()])
        file.close()

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    client.subscribe(topics['request'])

def on_message(client, userdata, msg):
    recieved = msg.payload.decode('utf-8')
    print(msg.topic+" "+str(recieved))
    update_csv(recieved)



client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message
client.on_connect = on_connect
client.connect('localhost',1883,60)
client.subscribe(topics['request'])
client.loop_forever()
