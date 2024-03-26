import serial
import time

# Configure serial port
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

def send_message(message):
    ser.write(message.encode())

send_message("Hello from CSC2106 Team 12!\n")
