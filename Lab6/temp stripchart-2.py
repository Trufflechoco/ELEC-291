import time
import serial
import serial.tools.list_ports
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sys, time, math

xsize=100

# graphing stuff
def data_gen():
    t = data_gen.t
    f = data_gen.f

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
    except:
        portlist=list(serial.tools.list_ports.comports())
        print ('Available serial ports:')
        for item in portlist:
           print (item[0])
        exit()

    while 1 :
        t+=1
        f+=1
        strin = ser.readline()
        while (len(strin) <= 10):
            strin = ser.readline()
        strin = str(strin).split(",")
        strin1=strin[1]
        strin2=strin1.split("deg")
        val = float(strin2[0])
        temperaturef = (val *9.0/5.0) + 32.0
        temperaturek= val + 273.15
        print(val, "Celsius")
        print(temperaturef, "Fahrenheit")
        print(temperaturek, "Kelvin")
 
        
        yield t, val
        
        
        

def run(data):
    # update the data
    
    if t>-1:
        xdata.append(t)
        ydata.append(y)
        if t>xsize: # Scroll to the left.
            ax.set_xlim(t-xsize, t)
        line.set_data(xdata, ydata)
        
    return line,

def run(data2):
    # update the data
    f,y2 = data2
    if f>-1:
        xdata2.append(f)
        ydata2.append(y2)
        if f>xsize: # Scroll to the left.
            ax.set_xlim(f-xsize, f)
        line2.set_data(xdata2, ydata2)
        
    return line2,

def on_close_figure(event):
    sys.exit(0)

data_gen.t = -1
data_gen.f = -1
fig = plt.figure()
fig.canvas.mpl_connect('close_event', on_close_figure)
ax = fig.add_subplot(111)
line, = ax.plot([], [], lw=2)
line2, = ax.plot([], [], lw=1)
ax.set_ylim(-10, 30)
ax.set_xlim(0, xsize)
ax.grid()
xdata, ydata = [], []
xdata2, ydata2 = [], []


# Important: Although blit=True makes graphing faster, we need blit=False to prevent
# spurious lines to appear when resizing the stripchart.
ani = animation.FuncAnimation(fig, run, data_gen, blit=False, interval=50, repeat=False)
plt.show()
