These udev rules are intended to provide a way to reliably and uniquely identify all arduinos connected to the controlling raspberry pi.
This file should be placed in /etc/udev/rules.d/

Thus far vendor, model and serial has been enough info to provide unique identification.

To find the necessary info about a new arduino:
Identify the device assigned to thew new arduino by reading syslog output while plugging the device in or just diff the contents of /dev with the arduino connected and disconnected.

Once you've identified the ad-hoc device that the new arduino is connected to, dump the udev info of that device
udevadm info -n /dev/ttyACMO | egrep -i 'serial|vendor|model'

Incorporate this info into the udev rule


# reload the rules:
udevadm control --reload-rules

# and then trigger the rules
udevadm trigger