# Encoded-Laser-and-Infrared-Serial-Communication

This is an arduino library which enables you to use a simple laser or LED to transmit characters between two arduinos (transmitter arduino with the laser to receiver arduino with a photodiode). Each byte of data is encoded (but not encrypted) to add some robustness to noise during the transmission and then modulated. The pin connection setup is explained in the example skethes included in the library. I originally wrote this as part of a netduino robotics project for uni, but I've since modified it to work with an arduino.

## What you need:
- 2 x [arduinos with USB cables](https://rover.ebay.com/rover/1/711-53200-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338102960&mpre=http%3A%2F%2Fwww.ebay.com%2Fitm%2FUNO-R3-MEGA328P-ATMEGA16U2-Development-board-for-Arduino-USB-Cable-TSUS%2F112271479914%3F_trksid%3Dp2045573.c100506.m3226%26_trkparms%3Daid%253D555014%2526algo%253DPL.DEFAULT%2526ao%253D1%2526asc%253D43550%2526meid%253Dacbccaeae4ac45e3b16bb8d5869da0c5%2526pid%253D100506%2526rk%253D1%2526rkt%253D1%2526)
- 2 x computers, one per arduino
- [male to male and male to female jumper leads](https://rover.ebay.com/rover/1/705-53470-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338112046&mpre=http%3A%2F%2Fwww.ebay.com.au%2Fitm%2F120Pcs-Good-Male-to-Female-Dupont-Wire-Jumper-Cable-for-Arduino-Breadboard-11cm-%2F252548000949%3Fepid%3D2097112763%26hash%3Ditem3acd08acb5%3Ag%3Ai~EAAOSwLnBX35pi%23shpCntId)
- 1 x [KY-008 laser module](https://rover.ebay.com/rover/1/705-53470-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338112046&mpre=http%3A%2F%2Fwww.ebay.com.au%2Fitm%2FKY-008-Laser-Transmitter-Module-for-Arduino-AVR-PIC-good-%2F201415041566%3Fhash%3Ditem2ee545a21e%3Ag%3AI5wAAOSwBP9UXYG3%23shpCntId) or a [visible light LED](https://rover.ebay.com/rover/1/705-53470-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338112046&mpre=http%3A%2F%2Fwww.ebay.com.au%2Fitm%2FElectronic-Parts-Pack-KIT-for-ARDUINO-Component-Switch-Button-Resistors-%2F132007578550%3Fhash%3Ditem1ebc43cbb6%3Ag%3AL-cAAOSwo4pYL1w2%23shpCntId)
- 1 x [laser sensor module, non-modulated](https://rover.ebay.com/rover/1/705-53470-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338112046&mpre=http%3A%2F%2Fwww.ebay.com.au%2Fitm%2F1x-Laser-Sensor-Module-non-modulator-Tube-Laser-Receiver-Module-best-cheap-%2F261583750482%3Fhash%3Ditem3ce79b4552%3Ag%3A11kAAOSwPe1T~r0w%23shpCntId)
- 1 x [mini breadboard](https://rover.ebay.com/rover/1/711-53200-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338102960&mpre=http%3A%2F%2Fwww.ebay.com%2Fitm%2FSYB-170-Mini-Solderless-Breadboard-Breadboard-Prototype-Board-Tie-points-LOT-%2F401247725915%3Fvar%3D%26hash%3Ditem5d6c3a695b%3Am%3AmFh1UBVkUnl0y7LDGqkWIMQ) for photo-receiver / photodiode

## How to install:
- download this library and add it to your libraries folder in the arduino folders on your computers

## How to use:
1. open the arduino application
2. open File -> Examples -> HT_LumenWire and select either the photoreceiver or transmitter examples
3. plug in the arduino and select the appropriate COM port, then upload the transmitter / receiver sketches to their respective arduinos
4. connect the components as described in the sketches
5. open the serial window (spyglass symbol in the IDE) on both computers
6. point the laser or LED to the photodiode
7. enter a character or phrase in the serial window on the computer doing the transmitting and press enter to send
8. you should receive the message on the receiver serial window automatically

## How it works:
### 1. [Hamming(7,4) Encoding](https://en.wikipedia.org/wiki/Hamming(7,4)):
- each byte of data (in ASCII: D7 D6 D5...D1 D0, e.g. D3 is 3rd digit of the byte) typed into the serial window is split into its most significant and least significant 4 bits, AKA nibbles: D7...D4 and D3...D0
- each nibble is encoded via hamming encoding, producing an unsigned 8 bit integer. The extra 4 bits contain information about the original nibble to help detect errors and recover the original nibble in case some of the data was scrambled due to noise in the transmission. The encoding / decoding algorithm has its limits on how much unscrambling it can do but it is certainly more efficient than sending multiple times to improve accuracy.
- the encoding is as follows: y = x G, where x = (D0 D1 D2 D3) and G is a [Hamming(7,4) matrix](https://user-images.githubusercontent.com/30153408/28493355-d03e608a-6f58-11e7-813d-4c5fae92f2de.png), producing y = (H0, H1, H2, D0, D1, D2, D3) and then we add an additional bit of even parity at the front y' = (P0, H0, H1, H2, D0, D1, D2, D3) where the parity bit P0 is just the XOR sum of the other bits.
- the two encoded nibbles (now 8 bits each) are concatenated, least significant byte first to product a 16-bit integer
### 2. [Manchester Modulation](https://en.wikipedia.org/wiki/Manchester_code):
- the 16-bit integer is then split into is least and most significant bytes
- each byte is modulated via manchester modulation: 2 start bits to let the receiver know it is about to receive actual data and not just stray signals, then the byte followed by 1 stop bit, producing 11 bits. Each of those 11 bits is represented with two half-bits: first half-bit is the original XOR'ed with 1 and the second is XOR'ed with 0 instead, this adds a clock pulse into the signal. This produces 22 bits for each byte.
- the pair of 22 bits are joined end on end, least significant first, producing 44 bits of data
### 3. Transmission
The 44-bit data is sent via the laser or LED, 1's are on and 0's are off
### 4. Manchester De-Modulation:
The 44-bit signal is received by the photo-detector and manchester-demodulated (reverse algorithm of modulation) to reconstruct the 16-bit encoded signal
### 5. Hamming Decoding:
The 16-bit demodulated signal is then hamming decoded to reproduce the original byte
### 6. Printing Message:
The byte is then printed on the serial window

## Extras:
- For a full list of included functions in the library, please refer to the .cpp and .h files.
- You can also set the bitrate via the .set_speed(...) function. It must be a minimum of 500 bits/sec but no more than the laser slew rate. You will need to set the speed manually into both the transmitter and receiver codes.

## Future versions:
- I plan to modify the code so that you only need to set the speed on the transmitter end and the receiver will automatically calculate the bitrate based on the interval between received bits.
- I plan to make this project work on a micro-controller with a higher clock rate than the arduino, like a raspberry pi
