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
    return curr_dt.hour == 0
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
    curr_date = datetime.datetime.now()
    time = curr_date.time()
    status = False

    if curr_time >= datetime.time(hh,mm):
        mqttc.publish('light/status', 'true')
        status = True

    if curr_time >= (datetime.datetime.combine(curr+datetime.time(hh,mm) + datetime.timedelta(minutes=1)):
        mqttc.publish('light/status', 'false')
        status = True

    print("Time:[{}:{}:{}] | At [{}:{}] -- Light triggering status: {}",curr_time.hour, curr_time.minute, curr_time.second,hh,mm, status)


def actual():
    if checkMidnight(test_date):
        if not checkUpdated(test_date, last_updated):
            print("RELEARNING MODEL !")
            model_learn()
            last_updated = test_date

    shouldOn = predict(test_date)

    print("[{:02d}:{:02d}:{:02d}] The light is already {}".format(datetime.datetime.now().hour, datetime.datetime.now().minute, datetime.datetime.now().second, "ON" if shouldOn else "OFF"))
    if shouldOn != curr_light_status:
        curr_light_status = shouldOn
        mqttc.publish("light/trigger",str(shouldOn))
        print("[{:02d}:{:02d}] The light switched to {}".format(test_date.hour, test_date.minute, "ON" if shouldOn else "OFF"))
    else:
        print("[{:02d}:{:02d}] The light is already {}".format(test_date.hour, test_date.minute, "ON" if shouldOn else "OFF"))


mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

#mqttc.username_pw_set("homeassistant", "eu0Vabee3iegaeRee4Voo9ozohtorahZeighoosai7soovae2ohcu8quahTi4iej")
#client.on_connect = on_connect
#mqttc.connect("192.168.1.1",1883, 60)

client.on_connect = on_connect
mqttc.connect('localhost',1883,60)


curr_light_status = checkLightStatus()
model_learn()
faker = Faker()

#test_date = faker.date_time_between(start_date='-2d')
test_date = datetime.datetime.now()
last_updated = datetime.datetime.now()
index = 1

while True:
    demo_method(20,30)
    time.sleep(1)



