# Arduino74c923
Replacing your 74c923 keyboard encoder, with an Arduino
T
So basically how it works, is we use a serial terminal to send keys to an Arduino, which is wired (via a buffer chip), into the TEC in place of the 74c923.
The Arduino then 'emulates' the 74c923 and fools the TEC.

The hardware buffer chip is needed as the Arduino can't tri-state it's outputs. Also, the Arduino may not be fast enough on it's own. We monitor
PORT2 of the TEC for activity - this tells us the TEC is scanning the displays, hence is ready for input. without this, the TEC could send keys
before the monitor was ready.

Please sse the full readme.txt and the notes at the top of the sourcecode. A schematic is also provided.
