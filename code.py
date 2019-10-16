# The cpx will execute the contents of this file

# see https://learn.adafruit.com/circuitpython-made-easy-on-circuit-playground-express/neopixels
# for additional cpx functionality

from adafruit_circuitplayground.express import cpx
import time

cpx.red_led = True

while True:
    cpx.pixels[4] = (0, 25, 5)
    cpx.pixels[5] = (0, 25, 5)
    time.sleep(1)

    cpx.pixels[3] = (0, 25, 5)
    cpx.pixels[6] = (0, 25, 5)
    time.sleep(1)

    # cpx.pixels[4] = (247, 255, 0)
    # cpx.pixels[5] = (247, 255, 0)
    # cpx.pixels[3] = (247, 255, 0)
    # cpx.pixels[6] = (247, 255, 0)
    cpx.pixels[2] = (24, 25, 0)
    cpx.pixels[7] = (24, 25, 0)
    time.sleep(1)

    # cpx.pixels[4] = (247, 255, 0)
    # cpx.pixels[5] = (247, 255, 0)
    # cpx.pixels[3] = (247, 255, 0)
    # cpx.pixels[6] = (247, 255, 0)
    cpx.pixels[2] = (24, 25, 0)
    cpx.pixels[7] = (24, 25, 0)
    cpx.pixels[1] = (24, 25, 0)
    cpx.pixels[8] = (24, 25, 0)
    time.sleep(1)

    for i in range(6):
        if (i % 2) == 0:
            cpx.pixels[0] = (50, 0, 0)
            cpx.pixels[9] = (50, 0, 0)
        else:
            cpx.pixels[0] = (0, 0, 0)
            cpx.pixels[9] = (0, 0, 0)
        time.sleep(0.75)

    # for i in range(10):
    #     cpx.pixels[(i - 9) % 10] = (0, 0, 0)
    #     cpx.pixels[(i - 6) % 10] = (20, 20, 20)
    #     cpx.pixels[(i - 3) % 10] = (0, 0, 20)
    #     cpx.pixels[i] = (20, 0, 0)
    #     cpx.pixels[(i + 1) % 10] = (20, 0, 0)
    #     time.sleep(0.1)

    for i in range(100):
        if (i % 2) == 0:
            cpx.pixels.fill((50, 0, 0))
        else:
            cpx.pixels.fill((0, 0, 0))
        time.sleep(0.075)

    cpx.pixels.fill((0, 0, 0))
