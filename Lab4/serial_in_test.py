import time
import serial
import serial.tools.list_ports

# configure the serial port
try:
    ser = serial.Serial(
        port='COM5', # Change as needed
        timeout=0.05,
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.EIGHTBITS
    )
    ser.isOpen()
    print("OK")
except:
    portlist=list(serial.tools.list_ports.comports())
    print ('Available serial ports:')
    for item in portlist:
       print (item[0])
    exit()

while 1 :
    strin = ser.readline()
    print (strin.decode('utf-8'))
