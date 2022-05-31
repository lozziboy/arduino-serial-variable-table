
/*  
a lightweight real time library to monitor and modify arduino variables via serial usb
this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266
https://github.com/lozziboy/arduino-serial-variable-table
*/

// include cSerialWatcher class
#include "cSerialWatcher.h"

// create SerialWatcher instance
cSerialWatcher SerialWatcher;

// create global variables
int u = 15;
bool v = true;
char w[] = "my array char";

void setup() {
  Serial.begin(115200); // init serial communication
}

void loop() {
  
  // map variables
  SerialWatcher.Map(F("u"), u);
  SerialWatcher.Map(F("v"), v);
  SerialWatcher.Map(F("w"), w);
  
  // update watch stream
  SerialWatcher.Update();
}
