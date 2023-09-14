
/*  
a fast lightweight real time library to monitor and modify arduino variables via serial usb
this library is compatible with arduino unoR3, unoR4, Mega, and shields based on esp8266, esp32
https://github.com/lozziboy/arduino-serial-variable-table
*/

// include cSerialWatcher class
#include "cSerialWatcher.h"

// create SerialWatcher instance
cSerialWatcher SerialWatcher(Serial);

// create global variables
char w[25] = "my array char";
int u = 15;
int v = 26;
float f = 13.6;
float g = 9.1;
byte b = 12;

void setup() {
  Serial.begin(115200); // init serial communication
}

void loop() {
  
  // map variables
  SerialWatcher.Map(F("w"), w);
  SerialWatcher.Map(F("u"), u);
  SerialWatcher.Map(F("v"), v);
  SerialWatcher.Map(F("f"), f);
  SerialWatcher.Map(F("g"), g);
  SerialWatcher.Map(F("b"), b);
  
  // update watch stream
  SerialWatcher.Update();
}
