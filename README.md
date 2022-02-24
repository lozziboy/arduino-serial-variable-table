# arduino-serial-variable-table
**a fast lightweight real time library to monitor and modify arduino variables via serial usb  
this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266**
      
___
* How to use it:

 1. include cSerialWatcher source library on top of the main.ino  
     `#include "cSerialWatcher.h"`
      
 2. create a SerialWatcher global instance  
     `cSerialWatcher SerialWatcher;`

 3. init Serial communication inside the setup routine  
     `Serial.begin(115200);`
     
 4. each variable to monitor/edit can be mapped to the GUI through the following instruction, readonly is optional and set to false by default. do that in the main loop  
	   `SerialWatcher.Map(F("Name_That_You_Prefer"), Global_Variable_Name);`
       
 5. call the update instruction to refresh them all at each loop scan (only once, in the main loop)  
	   `SerialWatcher.Update();`

 6. upload the software to the shield and then run SerialWatcherApp on your computer to monitor/edit the mapped global variables  
    https://github.com/lozziboy/arduino-serial-watcher-app    
    important note: to release the serial port pause the SerialWatcherApp using the "Pause button" every time that there is the needs to perform a new download to the board 
        
      
___
* Additional info
	   
   * readonly parameter is optional (default value is false)
	   
   * is possible to add empty spaces between lines through the following instruction  
        `SerialWatcher.EmptySpace();`

   * is possible to add custom texts through the following instruction  
        `SerialWatcher.Text(F("My custom text"));`

   * SerialWatcher class memory occupation can be exdended/reduced based on your needs!
      Just increase/decrease the number of monitorning slots based on which data-type and how many variables needs to be monitored.  
      Do that above the library include instruction.  
      `#define SERIALWATCHER_TX_MAX_BOOL 5`  
      `#define SERIALWATCHER_TX_MAX_BYTE 5`  
      `#define SERIALWATCHER_TX_MAX_INT 5`  
      `#define SERIALWATCHER_TX_MAX_UINT 2`  
      `#define SERIALWATCHER_TX_MAX_LONG 5`  
      `#define SERIALWATCHER_TX_MAX_ULONG 0`  
      `#define SERIALWATCHER_TX_MAX_FLOAT 5`  
      `#define SERIALWATCHER_TX_MAX_CHAR 0`  
      `#define SERIALWATCHER_TX_MAX_ACHAR 0`  
      `#define SERIALWATCHER_TX_MAX_FLASHSTRING 5`  
      `#define SERIALWATCHER_TX_MAX_TEXT 5`  
      
___
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
Char  | 8 bit (1 byte)
Array of Chars | 8 bits (1 byte) each slot
Flashstrings | 8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
Text/Empty Space | 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM  
      
___
* Preview:  

![Example](https://github.com/lozziboy/arduino-serial-variable-table/blob/main/docs/Example1.PNG)  

