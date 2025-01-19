import serial

ser = serial.Serial("/dev/cu.usbserial-14320", 9600)
ser.write("Hello\n".encode())
received_data = ser.readline()
print(received_data.decode()[:-1])
