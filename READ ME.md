# Operation-Noodles
 Operation Noodles is a Bluetooth-controlled ESP32 project that lets you turn an LED on/off, make it blink, and display numbers (0-9) on a 7-segment display—all with simple commands. A little experiment in IoT and command parsing.


+++Key Features:

1. takes text command from an app like serial Bluetooth monitor
2. recognize the command and does the what it says
3. here's the commands that can be recognized, and how they are recognized:
	1. tun on LED: if the command includes the words "on" and "LED"
	2. tun off LED: if the command includes the words "off" and "LED"
	3. blink LED {times} {period}: if the command includes the words "blink" and "LED". the variables ({times} for how many times the led should blink) and ({period} for how Much delay there should be between each blink) [e.g. blink LED 5 2000 ]
	4. Show number: checks if the command includes number, (only number, bcz the previous commands also may have number but they have "and" statements) it shows that number on the 7 segment.
	5. Shut down: this command turns off both led and 7 segment even if they're in the middle of executing.

+++Added features after debugging:
	
the LED pins are connected to D2 and GND

and 7 seg pins are : {26, 27, 13, 12, 14, 25, 33}; // GPIO pins for segments a to g

we also use millis() for the blink operation instead of delay() so we can interrupt the blinking operation with another command.
bcz if we use delay we have to use a while() loop and while() loops will not stop until the break condition is met. but when using millis() we can use a different method which doesn't have that issue.

the program is not case sensitive, neither it is sensitive to white space. 

+++requirements 
ESP32 library 