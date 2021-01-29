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



#define D0 2
#define D1 3
#define D2 4
#define D3 5
#define D4 6
#define Shift 7
#define NMI 8
#define Port2 9

int incomingByte = 0xff;
String validKeys = "0123456789abcdef+-GA";


void setup() {
  pinMode(NMI, OUTPUT);
  digitalWrite(NMI,HIGH);   // no interupt now please

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(Shift, OUTPUT);
  pinMode(Port2, INPUT);

  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("PCtoTEC Ready");
}





void TxChar(char keyByte) {

  Serial.print("Sending: ");            // this is the character ewe get from the Arduino Serial interface
  Serial.print(char(keyByte));

  int val = validKeys.indexOf(keyByte); // find the "scan code" value to send to the TEC (this is the A0-A5 bits the 74c923 would set for the key)

  Serial.print(" Mapval: ");
  Serial.println(val);

  int D0val = LOW;
  int D1val = LOW;
  int D2val = LOW;
  int D3val = LOW;
  int D4val = LOW;

  if ((val & 1) == 1) { 
    D0val = HIGH;
    }
  if ((val & 2) == 2) { 
    D1val = HIGH;
    }
  if ((val & 4) == 4) { 
    D2val = HIGH;
    }
  if ((val & 8) == 8) { 
    D3val = HIGH;
    }
  if ((val & 16) == 16) { 
    D4val = HIGH;
    }



  while (digitalRead(Port2) == HIGH) {    // wait till TEC is scanning displays so we know its ready for a key

  }

    
  digitalWrite(D0,D0val);                 // send byte to 74ls245 latch
  digitalWrite(D1,D1val);
  digitalWrite(D2,D2val);
  digitalWrite(D3,D3val);
  digitalWrite(D4,D4val);
  digitalWrite(Shift,HIGH);               // Shift is active-low

  digitalWrite(NMI,LOW);                  // strobe interrupt - "press" the key
  delay(5);                               // short delay for TEC to perform NMI routine
  digitalWrite(NMI,HIGH);                 // unstrobe interupt - "release" the key

 
}




void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (validKeys.indexOf(char(incomingByte))!=-1) {  // ignore invalid input
      TxChar(char(incomingByte));                     // transmit key to TEC
    }  
  }
}
