# Encoded-Laser-and-Infrared-Serial-Communication

This is an arduino library which enables you to use a simple laser or LED to send characters between two arduinos (transmitter arduino with the laser to receiver arduino with a photodiode). The pin connection setup is explained in the example skethes included in the library. You will need two computers, one per arduino.

How to install:
- download this library and add it to your libraries folder in the arduino folders on your computers

How to use:
1. open the arduino application
2. open File -> Examples -> HT_LumenWire and select either the photoreceiver or transmitter examples
3. plug in the arduino and select the appropriate COM port, then upload the transmitter / receiver sketches to their respective arduinos
4. connect the components as described in the sketches
5. open the serial window (spyglass symbol in the IDE) on both computers
6. point the laser or LED to the photodiode
7. enter a character or phrase in the serial window on the computer doing the transmitting and press enter to send
8. you should receive the message on the receiver serial window automatically
