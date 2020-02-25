import serial

byte_to_write = '1'
arduino_list = [1,2]
connection_list = []

for arduino_number in arduino_list:
    connection_list.append(serial.Serial('/dev/arduino-{}'.format(arduino_number), 9600))

for arduino_connection in connection_list:
    arduino_connection.write(byte_to_write.encode('utf-8'))
