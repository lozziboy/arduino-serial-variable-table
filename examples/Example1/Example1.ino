
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
int u = 15;
bool v = true;

void setup() {
  Serial.begin(115200); // init serial communication
}

void loop() {
  
  // map variables
  SerialWatcher.Map(F("u"), u);
  SerialWatcher.Map(F("v"), v);
  
  // update watch stream
  SerialWatcher.Update();
}
