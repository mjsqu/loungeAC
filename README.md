## Circuit details

The Arduino will use four main libraries:

* OneWire/Dallas - temperature sensing
* WifiClient - Connect to Wifi
* PubSubClient - Connect to a MQTT Server
* IRSend - sends IR signals to the Air Conditioner/Heatpump

The circuit requires 2 pins to be used - the circuit for the IR LED is more complicated:

* Pin connects to a resistor, banded brown 1 ,red 2, black 0, brown (x10^1), brown 1 - 1200 Ohms
* This connects to the Base (B) pin of a 2n3904 transistor
* Emitter (E) pin is connected to ground
* Collector (C) pin is connected to the LED
* Other pin of the LED is connected to a resistor with brown 1, black 0, black 0, black (x10^0), brown 1, - 100 Ohms, (swapping this for a smaller 56 Ohm resistor made the IR Led much brighter - check 2N3904 datasheet and see what we can get away with!)
* Resistor is connected to the 3.3v pin on the Arduino

The temperature sensor is connected to the driving pin from the middle pin with a resistor also connected to 3.3v (I think this is called pull-up), directly to the positive 3.3v and ground on the other two pins.

The resistor is 4.7k Ohms, yellow 4, purple 7, black 0, brown (x10^1)
