#!/bin/bash

usage () {
	echo "Usage: $0 <sketch path>"
	exit 1
}

if [ "$#" -lt 1 ] 
then
	usage
fi

SKETCH_PATH=$1

echo "compiling sketch: ${SKETCH_PATH}"
arduino-cli compile --fqbn arduino:avr:uno ${1} || exit 1

for ARDUINO in $(ls /dev/arduino*)
do
	echo "uploading sketch: ${SKETCH_PATH} to ${ARDUINO}"
	sudo /usr/local/bin/arduino-cli upload -p ${ARDUINO} --fqbn arduino:avr:uno ${SKETCH_PATH} || exit 1
done
