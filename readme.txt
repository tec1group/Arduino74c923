// PC to TEC Keyboard replacement v1.0
//
// Use an Arduino to emulate the 74c923 keyboard encoder.
//
// Note the Arduino serial is unbuffered and no flow control - so limit input strings to ghe max buffer length:
// MEGA, UNO - 64 bytes
// some very old boards may only be 16 bytes 
//
// Licenced as open source under the GNU GPL 3.0 -- https://opensource.org/licenses/GPL-3.0
//
// Code by Craig Hart, January 2021
//
// Note - the SHIFT key is always "Not Pressed" --- I leave this up to the user to implement if required
// shift functions ar eonly used in MON2 (?JMON) and ar emainly used for editing code. Since we are
// effectively "uploading" code with this tool, no need for shift (?)
//
// Also note - the Arduino Mega 2560 'glitches' during setup, so what I recommend is - start the Arduino,
// THEN press RESET on the TEC. After this, the system wrks fine. If you reset the arduino for any reason,
// expect 1-2 spurious keyboard inputs to be generated as the arduino configures itself.
//
// Last note - this code only emulates the 74c923 (TEC port 0) - it does not emulate RESET. But this could be
// added if needed.
//
//
// to use this code, open the Arduino's serial console, or usae a plain text terminal emaulator like PuTTY
// or even a dumb terminal, if you wish. Serial port is 9600,n,8,1 no flow control.
// 
// Valid keys are 0-9, a-f (lower case) and A (caps) for Address, G (Caps) for Go.
//
// Example: Send the string A0800Ad1+02A0270AGG to a MON-1 based TEC. The built-in intro (Issue 10, page 71, paragraph 5) runs.
//
// A0800A   - Enter address mode, set data entry address to 0800, return to data mode
// d1       - enter value d1
// +        - increment address
// 02       - enter value 02
// A0270A   - Enter address mode, set data entry address to 0270, return to data mode
// GG       - 'Go' - execute code at 0x0270
//
// Note that the 'A' is case sensitive - caps for the Address key, lower case enter the data value 'a'
//



So basically how it works, is we use a serial terminal to send keys to the Arduino. The Arduino then 'emulates' the 74c923 and fools the TEC.
The hardware buffer chip is needed as the Arduino can't tri-state it's outputs. Also, the Arduino may not be fast enough on it's own. We monitor
PORT2 of the TEC for activity - this tells us the TEC is scanning the displays, hence is ready for input. without this, the TEC could send keys
before the monitor was ready.


If you have code that doesn't scan the displays whilst looking for a key, this won't work. To fix this, remove the following two lines of code:

  while (digitalRead(Port1) == HIGH) {    // wait till TEC is scanning displays so we know its ready for a key
  }

This will fix it, but you will find that MON-1 will not work well, with inputs glitching at random. This is because the MONitor isn't always ready
for the next key when the Arduino sends it. I have tested with both MON-1B and MON2 and both work. Not tested with JMON as yet.




HARDWARE Notes: The NMI line of the Z80 (Pin 17) must be disconnected from the 4049 and driven only by the Arduino.

If you prefer to replace the 74c923 with this circuit (i.e. you want to drop this mod onto an existing TEC-1 PCB), then you can hook pin 8
of the Arduino to pin 13 of the 74c923 socket (unplug the 74c923, obviously!!!). You will then need to swap all the logic HIGH and LOW states
for each digitalWrite(NMI,xxxx); line in the code, as the 4049 inverts the logic.

In the schematic, TEC_PORT0 is pin 15 of the 74ls138 that is connected normally to pin 14 of the 74c923. You can plug into pin 14 of the
74c923 socket if you wish, to obrain this signal.

The data lines D0-D5 are pins 15-19 of the 74c923.

TEC_PORT2 is pin 13 of the same 74ls138 - which goes to pin 11 of the latch that drives the 7-segment display segments (top latch in the
TEC schematics). As this is a read-pin, leave it connected to the latch and just tap into the existing signal. In effect, the only pin of the Arduino 
that doesn't go to the 74c923 socket, is this pin.

Don't forget to also join the 0v of the Arduino with the 0v line of the TEC so both units share a commom reference voltage.

 