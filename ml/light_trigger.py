from http import client
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
import pandas as pd
import datetime
import time
import paho.mqtt.client as mqtt
from datetime import timedelta
from faker import Faker

model = DecisionTreeClassifier()


def model_learn():
    df = pd.read_csv("home_assistant_log.csv")

    #Converting timestamp to Hours, Minutes and Day.
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df['hour'] = df['timestamp'].dt.hour
    df['minute'] = df['timestamp'].dt.minute
    df['day'] = df['timestamp'].dt.day

    X = df.drop(columns=['timestamp','lightOn']).to_numpy()
    y = df['lightOn'].to_numpy()

    X_train, X_test, y_train, y_test = train_test_split(X,y, test_size=0.2,random_state=12)
    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)
    print("Accuracy is {} -- Split = 0.2".format(accuracy_score(y_test, y_pred)))

def predict(curr_dt):
    #curr_dt = datetime.datetime.now()
    curr_hour = curr_dt.hour
    curr_day = curr_dt.day
    curr_minute = curr_dt.minute
    predict =  model.predict([ [curr_hour, curr_minute, curr_day] ])[0]
    return predict

def checkMidnight(curr_dt):
    #curr_dt = datetime.datetime.now()
    return curr_dt.hour == 0 and curr_dt.minute == 0 and curr_dt.second == 0
    #return curr_dt.hour == 0 and curr_dt.minute == 0 and curr_dt.second == 0

def checkLightStatus():
    df = pd.read_csv("home_assistant_log.csv")
    last_status = df.iloc[-1]['lightOn']
    return last_status

def checkUpdated(date, date2):
    return date.day == date2.day and date.month == date2.month and date.year == date2.year

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")


def demo_method(hh, mm):
    toRunFor = 20 #Seconds
    curr_date = datetime.datetime.now()
    time = curr_date.time()

    status = ""

    if time.hour == hh and time.minute == mm and time.second == 0:
        mqttc.publish('light/status', 'true')
        status = "-- Triggering light/status -- True"



    tts = datetime.datetime.combine(curr_date, datetime.time(hh,mm)) + datetime.timedelta(seconds=toRunFor)
    if time.hour == tts.hour and time.minute == tts.minute and time.second == tts.second:
        mqttc.publish('light/status', 'false')
        status = "-- Triggering light/status -- False"

    print("Time:[{:02d}:{:02d}:{:02d}] {}".format(time.hour, time.minute, time.second, status))
    status = ""



def actual():
    now = datetime.datetime.now()
    curr_light_status = checkLightStatus()
    if checkMidnight(now):
        print("Midnight -- Relearning model")
        model_learn()

    for i in range(1,5):
        shouldOn = predict([i,now])
        status = ""

        if curr_light_status != shouldOn:
            curr_light_status = shouldOn
            mqttc.publish("light/status",str(shouldOn).lower())
            status = " -- light/status/{} setting to {}".format(i,str(shouldOn).lower())

        print("[{:02d}:{:02d}:{:02d}] {}".format(now.hour, now.minute, now.second, status))
        status = ""




mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

mqttc.username_pw_set("homeassistant", "eu0Vabee3iegaeRee4Voo9ozohtorahZeighoosai7soovae2ohcu8quahTi4iej")
client.on_connect = on_connect
#mqttc.connect("192.168.1.1",1883, 60)
mqttc.connect("192.168.50.176", 1883, 60)

#client.on_connect = on_connect
#mqttc.connect('localhost',1883,60)


model_learn()
faker = Faker()

while True:
    demo_method(00,44)
    #actual()
    time.sleep(1)



