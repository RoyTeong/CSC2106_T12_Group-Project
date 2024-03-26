import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
OT1_PIN = 21
GPIO.setup(OT1_PIN, GPIO.IN)

try:
	while True:
		motion_detected = GPIO.input(OT1_PIN)

		if motion_detected:
			print("Motion detected!")
		else:
			print("No motion!")

		time.sleep(1)
except KeyboardInterrupt:
	print("Exiting...")

finally:
	GPIO.cleanup()
