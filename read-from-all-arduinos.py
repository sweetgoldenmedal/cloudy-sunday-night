import serial
import time
from datetime import datetime
import sys


sleep_time = 0.1
arduino_list = [1,2]
connection_list = []

for arduino_number in arduino_list:
    connection_list.append(serial.Serial('/dev/arduino-{}'.format(arduino_number), 9600))

while 1:
    for arduino_connection in connection_list:
        if (arduino_connection.in_waiting > 0):
            byte = arduino_connection.read().decode('utf-8')
            print("{} device: {} byte: {}".format(datetime.now(),arduino_connection.port, byte))
        time.sleep(sleep_time)

