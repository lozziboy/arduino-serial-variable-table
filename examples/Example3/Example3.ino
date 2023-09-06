
/*  
a lightweight real time library to monitor and modify arduino variables via serial usb
this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266
https://github.com/lozziboy/arduino-serial-variable-table
*/

// save space in memory tracing only data that needs to be watched
#define SERIALWATCHER_TX_MAX_BOOL 0
#define SERIALWATCHER_TX_MAX_BYTE 1
#define SERIALWATCHER_TX_MAX_INT 2
#define SERIALWATCHER_TX_MAX_UINT 0
#define SERIALWATCHER_TX_MAX_LONG 0
#define SERIALWATCHER_TX_MAX_ULONG 0
#define SERIALWATCHER_TX_MAX_FLOAT 2
#define SERIALWATCHER_TX_MAX_CHAR 0
#define SERIALWATCHER_TX_MAX_ACHAR 1
#define SERIALWATCHER_TX_MAX_FLASHSTRING 0
#define SERIALWATCHER_TX_MAX_TEXT 0

// include cSerialWatcher class
#include "cSerialWatcher.h"

// create SerialWatcher instance
cSerialWatcher SerialWatcher(Serial);

// create global variables
int u = 15;
int v = 26;
char w[25] = "my array char";
float f = 13.6;
float g = 9.1;
byte b = 12;

void setup() {
  Serial.begin(115200); // init serial communication
}

void loop() {
  
  // map variables
  SerialWatcher.Map(F("u"), u);
  SerialWatcher.Map(F("v"), v);
  SerialWatcher.Map(F("w"), w);
  SerialWatcher.Map(F("f"), f);
  SerialWatcher.Map(F("g"), g);
  SerialWatcher.Map(F("b"), b);
  
  // update watch stream
  SerialWatcher.Update();
}
