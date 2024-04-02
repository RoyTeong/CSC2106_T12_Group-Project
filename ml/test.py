import csv
import random
import pandas as pd
import datetime
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
import datetime
from faker import Faker
import time

## Initialize and train the model
model = DecisionTreeClassifier()
fake = Faker()

usual_pattern = [
    (datetime.time(7, 0), datetime.time(17, 0), False),  # 7am - 5pm: Light Off
    (datetime.time(17, 0), datetime.time(22, 0), True),  # 5pm - 10pm: Light On (sometimes Off)
    (datetime.time(22, 0), datetime.time(2, 0), False),  # 10pm - 2am: Light Off (sometimes On)
    (datetime.time(2, 0), datetime.time(2, 15), True),  # 2am - 2:15am: Light On (mostly Off)
    (datetime.time(2, 15), datetime.time(7, 0), False)   # 2:15am - 7am: Light Off (mostly On)
]

def checkActualOutputOfLight(date):
    for start_time, end_time, light_status in usual_pattern:
        if start_time < date.time() < end_time:
            return "ON" if light_status else "OFF"

def learn_model():
    # Read the CSV file
    df = pd.read_csv("home_assistant_log.csv")

    # Define features (X) and target (y)
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    df['hour_of_day'] = df['timestamp'].dt.hour
    df["minute"] = df["timestamp"].dt.minute

    X = df.drop(columns=["timestamp", "lightOn"], axis=1).to_numpy()
    y = df["lightOn"].to_numpy()


    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=12)
    model.fit(X_train,y_train)

def predict(date):
    day = date.day
    hour = date.hour
    minute = date.minute

    y_pred = model.predict([[hour, minute]])
    return y_pred


# ============================================

learn_model()
shouldOn = predict(datetime.datetime.now())

tocheck = datetime.datetime.now()
testCheck = 1


#For MQTT
#import paho.mqtt.client as mqtt
#import json


#topic = 'home/light'

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("home/light")

## The callback for when a PUBLISH message is received from the server.
#def on_message(client, userdata, msg):
#    #print(msg.topic+" "+str(msg.payload))
#    rec = msg.payload.decode('utf-8')
#    if rec == "get_status":
#        light_status = predict(datetime.datetime.now())
#        print("Publishing light status = {}".format(light_status[0]))
        #mqttc.publish(topic, str(light_status[0]))
#    print("At {:02d}:{:02d}:{:02d}, Should be {}".format(tocheck.hour, tocheck.minute, tocheck.second ,predict(tocheck)[0]))
#    testCheck = testCheck + 1
#    if testCheck > 5:
#        tocheck = fake.date_time()
#    else:
#        tocheck = datetime.datetime.now()

#    print("At {:02d}:{:02d}:{:02d}, Should be {}".format(tocheck.hour, tocheck.minute, tocheck.second ,predict(tocheck)[0]))
#    testCheck = testCheck + 1
#    if testCheck > 5:
#        tocheck = fake.date_time()
#    else:
#        tocheck = datetime.datetime.now()

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("192.168.50.176", 1883, 60)
mqttc.subscribe("home/light")
mqttc.loop_forever()


light_status = False
while True:
    predicted = predict(tocheck)[0]
    print("At {:02d}:{:02d}:{:02d}, Should be {}".format(tocheck.hour, tocheck.minute, tocheck.second ,predicted))
    print("Sending trigger to on" if light_status != predicted else "Do nothing")
    if light_status != predicted:
        light_status = not light_status
    testCheck = testCheck + 1
    if testCheck > 5:
        tocheck = fake.date_time()
    else:
        tocheck = datetime.datetime.now()
    time.sleep(5)