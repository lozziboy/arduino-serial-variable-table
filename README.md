# arduino-serial-variable-table
**a fast lightweight real time library to monitor and modify arduino variables via serial usb  
this library is compatible with arduino unoR3, Mega, unoR4 and shields based on esp32, esp8266**
      
___
* Preview:  

![Example](https://github.com/lozziboy/arduino-serial-variable-table/blob/main/docs/image.PNG)  

___
* How to use it:

 1. include cSerialWatcher source library on top of the main.ino  
     `#include "cSerialWatcher.h"`
      
 2. create a SerialWatcher global instance  
     `cSerialWatcher SerialWatcher(Serial);`

 3. init Serial communication inside the setup routine  
     `Serial.begin(115200);`
     
 4. each variable to monitor/edit can be mapped to the GUI through the following instruction, readonly is optional and set to false by default. do that in the main loop  
	   `SerialWatcher.Map(F("Name_That_You_Prefer"), Global_Variable_Name);`
       
 5. call the update instruction to refresh them all at each loop scan (only once call in the main loop)  
	   `SerialWatcher.Update();`

 6. upload the software to the shield and then run SerialWatcherApp on your computer to monitor/edit the mapped global variables  
    https://github.com/lozziboy/arduino-serial-watcher-app    
    important note: to release the serial port pause the SerialWatcherApp using the "Pause button" every time that there is the needs to perform a new download to the board
    
Variable table is constantly updated in little parts at each loop().
Is strongly suggested to do not use blocking istructions in order to run the loop() as fast as possible.
Some blocking istructions to be avoided in the main loop() are
while() statemens that needs many time
delay() statemens that freeze the program for many time
For a good user experience the loop() time should be not higher than 50ms
      
___
* Additional info
	   
   * readonly parameter is optional (default value is false)
	   
   * is possible to add empty spaces between lines through the following instruction  
        `SerialWatcher.EmptySpace();`

   * is possible to add custom texts through the following instruction  
        `SerialWatcher.Text(F("My custom text"));`

   * SerialWatcher class memory occupation can be exdended/reduced based on your needs!
      Just edit the cSerialWatcher.h file to increase the number of monitorning slots based on how many variables monitor.  
      In Windows envirmonment the library position is C:\Users\(your_login_user)\Documents\Arduino\libraries\cSerialWatcher.  
      `#define SERIALWATCHER_TX_MAX_VALUES 5`  
      
___
* Supported Data-Types:

Data-Type | Info
------------ | -------------
bool | 8 bit (1 byte) two possible values, true or false
int8_t | 8 bit (1 byte), from -128 to 127
uint8_t | 8 bit (1 byte) unsigned number, from 0 to 255          
int16_t | 16 bit (2 byte), from -32,768 to 32,767
uint16_t | 16 bit (2 byte) unsigned number, from 0 to 65,535
int32_t | 32 bit (4 bytes), from -2,147,483,648 to 2,147,483,647
uint32_t | 32 bit (4 bytes) unsigned number, from 0 to 4,294,967,295
int64_t | 32 bit (4 bytes), from -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
uint64_t | 32 bit (4 bytes) unsigned number, from 0 to 18,446,744,073,709,551,615
Float | 32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
Char  | 8 bit (1 byte), , from -128 to 127
Array of Chars | 8 bits (1 byte) each slot
Flashstrings | 8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
Text/Empty Space | 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM  
      

