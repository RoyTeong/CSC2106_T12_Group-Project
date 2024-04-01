import tkinter as tk
import subprocess

def command(topic, payload):
    cmd = ["mosquitto_pub","-h","192.168.1.1","-p","1883","-t"]
    cmd.append(topic)
    cmd = cmd + ["-m",payload]
    cmd = cmd + ["-u","homeassistant","-P","eu0Vabee3iegaeRee4Voo9ozohtorahZeighoosai7soovae2ohcu8quahTi4iej"]
    print(cmd)
    subprocess.run(cmd)

def light_on():
    command("light/status","true")

def light_off():
    command("light/status","false")

def tv_on():
    command("device/toggle","634095713:onled")

def tv_off():
    command("device/toggle","634095713:offled")

def ac_on():
    command("device/toggle","4218396093:on")

def ac_off():
    command("device/toggle","4218396093:off")

root = tk.Tk()
root.geometry("400x400")
root.title("My Home Assistant")
root.maxsize(1000,400)

button_height = 3  # Adjust the height as needed

lightOnBtn = tk.Button(root, text="Light on", command=light_on, height=button_height)
lightOnBtn.pack(side="top", fill="x", expand=True)

lightOffBtn = tk.Button(root, text="Light off", command=light_off, height=button_height)
lightOffBtn.pack(side="top", fill="x", expand=True)

tvOn = tk.Button(root, text="Tv On", command=tv_on, height=button_height)
tvOn.pack(side="top", fill="x", expand=True)

tvOff = tk.Button(root, text="Tv Off", command=tv_off, height=button_height)
tvOff.pack(side="top", fill="x", expand=True)

acOn = tk.Button(root, text="Ac On", command=ac_on, height=button_height)
acOn.pack(side="top", fill="x", expand=True)

acOff = tk.Button(root, text="Ac Off", command=ac_off, height=button_height)
acOff.pack(side="top", fill="x", expand=True)


root.mainloop()
