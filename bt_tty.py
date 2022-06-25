from time import sleep
import serial
import sys
import atexit

ser = serial.Serial('/dev/cu.ESSlave-DevB')  # open serial port

def onclose():
    ser.close()

atexit.register(onclose)

msg = 'Hello'
while True:
    ser.write([ord(char) for char in msg])
    sleep(1)
    #s = ser.read(48)
    #sys.stdout.buffer.write(s)
    #sys.stdout.flush()
