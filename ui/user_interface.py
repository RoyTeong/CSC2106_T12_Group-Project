import tkinter as tk
import subprocess

def list_directories():
    cmd = ["ls", "-la"]
    subprocess.run(cmd)

def publish_mqtt():
    cmd = ["mosquitto_pub", "-h", "localhost","-t",'light/status', '-m', 'True']
    subprocess.run(cmd)

def start_mqtt_broker():
    cmd = ["mosquitto"]
    subprocess.run(cmd)

root = tk.Tk()
root.geometry("400x400")
root.title("My Home Assistant")
root.maxsize(1000,400)

button = tk.Button(root, text="List Current Directories",command=list_directories)
button.pack()

start_mqtt = tk.Button(root, text="Publish MQTT", command=publish_mqtt)
start_mqtt.pack()

start_mqtt = tk.Button(root, text="Start Broker", command=start_mqtt_broker)
start_mqtt.pack()

root.mainloop()