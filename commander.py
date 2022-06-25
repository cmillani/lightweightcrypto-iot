import asyncio
from time import sleep
import serial
import sys
import atexit

debug_serial = serial.Serial('/dev/cu.usbmodem14101')  # open serial port
#data_serial = serial.Serial('/dev/cu.ESSlave-DevB')  # open serial port

def onclose():
    debug_serial.close()
    #data_serial.close()

atexit.register(onclose)

while True:
    if (debug_serial.in_waiting > 0):
        print(str(debug_serial.readline()))
    sleep(1)
