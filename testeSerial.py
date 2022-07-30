import serial
from time import sleep

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
ser.reset_input_buffer()

while True:
	ser.reset_input_buffer()
	c = input()
	ser.write(c.encode('utf-8'))
	ser.write('\n'.encode('utf-8'))
	print(f'T:{c}\n');

	sleep(1)
	if ser.in_waiting > 0:	
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
	sleep(1)
	