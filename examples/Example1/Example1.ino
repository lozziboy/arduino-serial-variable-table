
// include cSerialWatcher class inside the project
#include "cSerialWatcher.h"

// create SerialWatcher instance
cSerialWatcher SerialWatcher;

// create global variables
int u,v,w;

void setup() {
  // init serial communication
  Serial.begin(115200);
  // init variables
  u = 15;
  v = 26;
  w = 13;
}

void loop() {
  
  // map variables
  SerialWatcher.Map(F("u"), u);
  SerialWatcher.Map(F("v"), v);
  SerialWatcher.Map(F("w"), w);
  
  // update watch stream
  SerialWatcher.Update();
  
}
