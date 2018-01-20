import time
import serial
import struct

ser = serial.Serial( #下面这些参数根据情况修改
	port='COM21',
	baudrate=115200,
 
)

while (True):
	while ser.inWaiting() > 0:
		data = ser.read(6)
		if data :
			#ch=(struct.unpack('4h',data[1:]));
			#print ([i/16384*20 for i in ch])
			print(struct.unpack('3h',data))
		
		