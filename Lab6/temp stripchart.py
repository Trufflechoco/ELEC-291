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
        strin = ser.readline()
        while (len(strin) <= 10):
            strin = ser.readline()
        strin = str(strin).split(",")
        strin1=strin[1]
        strin2=strin1.split("deg")
        val = float(strin2[0])
        print(val)
        yield t, val

def run(data):
    # update the data
    t,y = data
    if t>-1:
        xdata.append(t)
        ydata.append(y)
        if t>xsize: # Scroll to the left.
            ax.set_xlim(t-xsize, t)
        line.set_data(xdata, ydata)

    return line,

def on_close_figure(event):
    sys.exit(0)

data_gen.t = -1
fig = plt.figure()
fig.canvas.mpl_connect('close_event', on_close_figure)
ax = fig.add_subplot(111)
line, = ax.plot([], [], lw=2)
ax.set_ylim(-10, 30)
ax.set_xlim(0, xsize)
ax.grid()
xdata, ydata = [], []

# Important: Although blit=True makes graphing faster, we need blit=False to prevent
# spurious lines to appear when resizing the stripchart.
ani = animation.FuncAnimation(fig, run, data_gen, blit=False, interval=50, repeat=False)
plt.show()
