import serial
from time import sleep

ser = serial.Serial("/dev/ttyS0", 9600)
while True:
    received_data = ser.readline() # could also used ser.read(6)
    print(received_data.decode()[:-1])
    sleep(1)
    ser.write(received_data)
