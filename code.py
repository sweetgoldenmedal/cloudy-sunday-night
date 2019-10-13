# The cpx will execute the contents of this file

from adafruit_circuitplayground.express import cpx
import time
#while True:
#    cpx.red_led = True
#    time.sleep(.01)
#    cpx.red_led = False
#    time.sleep(.01)

cpx.red_led= False

while True:
    for i in range(10):
        cpx.pixels[i] = [1,0,1]
        #cpx.pixels[(i-9)%10] = (0,0,0)
        #cpx.pixels[(i-6)%10] = (20,20,20)
        #cpx.pixels[(i-3)%10] = (0,0,20)
        #cpx.pixels[i] = (20,0,0)
        #cpx.pixels[(i+1)%10] = (20,0,0)
        #time.sleep(.1)
        time.sleep(1)

    for i in range(10):
        cpx.pixels[i] = [0,0,0]
        time.sleep(1)
