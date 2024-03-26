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

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.connect('localhost',1883, 60)


curr_light_status = checkLightStatus()
model_learn()
faker = Faker()

test_date = faker.date_time_between(start_date='-2d')
last_updated = datetime.datetime.now()
index = 1

while True:
    if checkMidnight(test_date):
        if not checkUpdated(test_date, last_updated):
            print("RELEARNING MODEL !")
            model_learn()
            last_updated = test_date

    shouldOn = predict(test_date)
    test_date = test_date + timedelta(minutes=30)

    if shouldOn != curr_light_status:
        curr_light_status = shouldOn
        print("[{:02d}:{:02d}] The light switched to {}".format(test_date.hour, test_date.minute, "ON" if shouldOn else "OFF"))
    else:
        print("[{:02d}:{:02d}] The light is already {}".format(test_date.hour, test_date.minute, "ON" if shouldOn else "OFF"))

    time.sleep(0.2)



