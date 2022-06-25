import serial
import sys
import atexit

ser = serial.Serial('/dev/cu.ESSlave-DevB')  # open serial port

def onclose():
    ser.close()

atexit.register(onclose)

while True:
    s = ser.read(48)
    sys.stdout.buffer.write(s)
    sys.stdout.flush()
