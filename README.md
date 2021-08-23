# arduino-serial-watch-table
**a fast lightweight real time library to monitor and modify arduino variables via serial usb  
this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266**


* How to use it:

  1. copy SerialWatcher library cSerialWatch.h inside your project folder and include it on top of the main.ino
      `#include "cSerialWatch.h"`

  2. create a global SerialWatch instance  
      `cSerialWatcher SerialWatcher;`

  3. Setup Serial communication inside the setup() routine  
     `Serial.begin(115200);`
     
  4. map the variables inside the main loop():
     * each variable to monitor and edit can be mapped to the GUI through the following instruction,  
       readonly is optional and set to false by default  
       `SerialWatcher.Map(F("Name_That_I_Like"), GlobalVariableName(any), Readonly(bool));`
       
      * is possible to add empty spaces between lines through the following instruction  
        `SerialWatcher.EmptySpace();`

      * is possible to add custom texts through the following instruction  
        `SerialWatcher.Text(F("My custom text"));`

      * after the variables mapping, is necessary to call this instruction to update them all at each loop scan
        `SerialWatcher.Update();`

   5. upload the software to the shield and then run SerialWatcherApp on a laptop to monitor/edit the mapped global variables

* Supported Data-Types:

Data-Type | Info
------------ | -------------
Bool | 8 bit (1 byte) two possible values, true or false
Byte | 8 bit (1 byte) unsigned number, from 0 to 255          
Int | 16 bit (2 byte), from -32,768 to 32,767
Unsigned Int | 16 bit (2 byte) unsigned number, from 0 to 65,535
Long | 32 bit (4 bytes), from -2,147,483,648 to 2,147,483,647
Unsigned Long | 32 bit (4 bytes) unsigned number, from 0 to 4,294,967,295
Float | 32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
Array of Chars | 8 bits (1 byte) each slot
Flashstrings | 8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
Text/Empty Space | 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM
