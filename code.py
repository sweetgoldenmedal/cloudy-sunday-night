# The cpx will execute the contents of this file

# see https://learn.adafruit.com/circuitpython-made-easy-on-circuit-playground-express/neopixels
# for additional cpx functionality

from adafruit_circuitplayground.express import cpx
import time

cpx.red_led = True

while True:
    for i in range(10):
        cpx.pixels[(i - 9) % 10] = (0, 0, 0)
        cpx.pixels[(i - 6) % 10] = (20, 20, 20)
        cpx.pixels[(i - 3) % 10] = (0, 0, 20)
        cpx.pixels[i] = (20, 0, 0)
        cpx.pixels[(i + 1) % 10] = (20, 0, 0)
        time.sleep(0.1)

    for i in range(5):
        if (i % 2) == 0:
            cpx.pixels.fill((50, 0, 0))
        else:
            cpx.pixels.fill((0, 0, 0))
        time.sleep(0.1)

    cpx.pixels.fill((0, 0, 0))
