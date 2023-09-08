/*
	cSerialWatcher.h

	a fast lightweight real time library to monitor and modify arduino variables via serial usb
	this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266
  

  Instructions

	1) include SerialWatcher source library
	   #include "cSerialWatcher.h"
	   
	2) create a SerialWatcher instance
		cSerialWatcher SerialWatcher(Serial);

  3) init Serial communication inside the setup() routine
	   Serial.begin(115200);

	4) map the variables to watch in the GUI inside the main loop()
     SerialWatcher.Map(F("Name_That_You_Prefer"), GlobalVariableDeclarationName(Variable_Name), Readonly(bool));

  5) call the update instruction to refresh them all at each loop scan (only once, in the loop)
     SerialWatcher.Update();

  6) upload the software to the shield and then run SerialWatcherApp on your computer to monitor/edit the mapped global variables
		 https://github.com/lozziboy/arduino-serial-watcher-app


  Additional info

  > readonly parameter is optional (default value is false)

  > the following instuction add empty spaces between lines 
      SerialWatcher.EmptySpace();

  > the following instuction add custome texts
      SerialWatcher.Text(F("My custom text"));

  > SerialWatcher class memory occupation can be reduced/exdended based on your needs! 
	  Just increase/decrease the number of monitorning slots based how many variables to be monitored
      #define SERIALWATCHER_TX_MAX_VALUES 5

  Usage example

      SerialWatcher.Map(F("Name"), var, false);         //  | F("name") | variable | readonly |
      SerialWatcher.EmptySpace();
      SerialWatcher.Text(F("Write here your text"));
  

	Supported Data-Types 
	> bool:             8 bit (1 byte) two possible values, true or false
    > int8_t:           8 bit (1 byte) unsigned number, from -128 to 127
    > uint8_t:          8 bit (1 byte) unsigned number, from 0 to 255
    > int16_t:          16 bit (2 byte), from -32'768 to 32'767
    > uint16_t:         16 bit (2 byte) unsigned number, from 0 to 65,535
    > int32_t:          32 bit (4 bytes), from -2'147'483'648 to 2'147'483'647
    > uint32_t:         32 bit (4 bytes) unsigned number from 0 to 4'294'967'295
    > int64_t:          64 bit (8 bytes), from -9'223'372'036'854'775'808 to 9'223'372'036'854'775'807
    > uint64_t:         64 bit (8 bytes) unsigned number, from 0 to 18'446'744'073'709'551'615   
    > float:            32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
    > char:             8 bits (1 byte), from -128 to 127
    > array of chars:   8 bits (1 byte) each slot of the array
    > flashstrings:     8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
    > text/empty space: 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM

*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef cSerialWatcher_h
#define cSerialWatcher_h

//Library configuration (base): max number of transmittable types
#ifndef SERIALWATCHER_TX_MAX_VALUES
  #define SERIALWATCHER_TX_MAX_VALUES 5  // array 0 .. TX_MAX_VALUES - 1
#endif

//Library configuration (base): activate advanced diagnostics - pay attention this will add a long delay during the library Rx phase
#define SERIALWATCHER_RX_ADV_DIAGNOSTICS false //"true" show rx advanced diagnostics -> leave to "false" before to release the project

//Library configuration (advanced)
#define SERIALWATCHER_RX_ARRAY_LENGTH 26 //lenght of array containing rx data = MAX_EDIT_ACHAR + '<' + 'b' + ':' + '1' + '2' + ':' + '>' + '\0'

// Tx codes
#define SERIALWATCHER_TX_CODE_STREAM_START "<*" // identifies the transmission stream Start
#define SERIALWATCHER_TX_CODE_STREAM_END "*>" // identifies the transmission stream End
#define SERIALWATCHER_TX_CODE_EXCEEDED_MAX_TXVAR "*1*" //exceeded max number of transmittable data (error)
#define SERIALWATCHER_TX_FIELD_SEPARATOR "\""  // identifies the transmission of a field separator -> this cannot be used for the map function
#define SERIALWATCHER_TX_ELEMENT_SEPARATOR ";"  // identifies the transmission of a code separator (new data) -> this cannot be used for the map function

// Tx Datatypes encoding
#define SERIALWATCHER_DATATYPE_bool 0
#define SERIALWATCHER_DATATYPE_int8_t 1
#define SERIALWATCHER_DATATYPE_uint8_t 2
#define SERIALWATCHER_DATATYPE_int16_t 3
#define SERIALWATCHER_DATATYPE_uint16_t 4
#define SERIALWATCHER_DATATYPE_int32_t 5
#define SERIALWATCHER_DATATYPE_uint32_t 6
#define SERIALWATCHER_DATATYPE_int64_t 7
#define SERIALWATCHER_DATATYPE_uint64_t 8
#define SERIALWATCHER_DATATYPE_float 9
#define SERIALWATCHER_DATATYPE_char 10
#define SERIALWATCHER_DATATYPE_achar 11
#define SERIALWATCHER_DATATYPE_flashstring 12
#define SERIALWATCHER_DATATYPE_FREE0 13 //free (spare - available for future implementations)
#define SERIALWATCHER_DATATYPE_FREE1 14 //free (spare - available for future implementations)
#define SERIALWATCHER_DATATYPE_FREE2 15 //free (spare - available for future implementations)

// Tx Array Features - BitMask
#define SERIALWATCHER_FEATURES_MASK_DATATYPE 0x0F // Var Features encoded: b0 b1 b2 b3 tells datatype
#define SERIALWATCHER_FEATURES_MASK_INVALID_NAME 0x10 // Var Features encoded: b4 tells if the field has an invalid name (0=ok, | 1=invalid)
#define SERIALWATCHER_FEATURES_MASK_INVALID_VALUE 0x20 // Var Features encoded: b5 tells if the field has an invalid value (0=ok, | 1=invalid)
#define SERIALWATCHER_FEATURES_MASK_READONLY 0x40 // Var Features encoded: b6 tells if the field is readonly (0=read/write | 1=readonly)
#define SERIALWATCHER_FEATURES_MASK_FREE 0x80 // Var Features encoded: b7 is free (spare - available for future implementations)

// Rx codes
#define SERIALWATCHER_RX_BUFFER_END_IDX ';'    // identifies the rx data buffer array index (position of the last character)
#define SERIALWATCHER_RX_CODE_STREAM_START '<' // identifies the receipt of the stream Start (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_END '>'   // identifies the receipt of the stream End (max one character)
#define SERIALWATCHER_RX_FIELD_SEPARATOR '"'   // identifies the receipt of the stream field separator (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_bool 'b'   // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_int8_t 'q' // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_uint8_t 'd'  // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_int16_t 'i'   // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_uint16_t 'n'  // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_int32_t 'l'  // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_uint32_t 'u' // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_int64_t 'r' // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_uint64_t 'm' // (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_float 'f' // " (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_char 'c'  // " (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_achar 's' // " (max one character)
 
#define SERIALWATCHER_MEMO_FISTMAPDONE         B00000001 //when TRUE means that first map has been done
#define SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR  B00000010 //when TRUE means that the maximum number of transmittable variables has been exceeded

class cSerialWatcher {
  
  // public methods
  public:
    cSerialWatcher(Stream &serial); //constructor
    //& = pass by reference method, when call the function is not necessary to convert input to a pointer
    void Map(const __FlashStringHelper *Name, bool &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, int8_t &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, uint8_t &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, int16_t &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, uint16_t &Value, bool readonly = false);
    #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    void Map(const __FlashStringHelper *Name, int &Value, bool readonly = false); //special case to manage arduino R4 (int are 32bits)
    #endif
    void Map(const __FlashStringHelper *Name, int32_t &Value, bool readonly = false); 
    void Map(const __FlashStringHelper *Name, uint32_t &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, int64_t &Value, bool readonly = false); 
    void Map(const __FlashStringHelper *Name, uint64_t &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, float &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, char &Value, bool readonly = false);
    void Map(const __FlashStringHelper *Name, char *Value, bool readonly = false);  //array of chars: char Value[]
    void Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value); //flashstring
    void Text(const __FlashStringHelper *Value);   //write a fixed value
    void EmptySpace();
    void Update();
    
  // private attributes
  private:

    //reference to serial port object
    Stream* _serial;                

    //error memories
    byte _mem;

    //global sequence index
    byte indexglobal;

    //name, value and datatype of each pointed variable
    const __FlashStringHelper *ArrayNames[SERIALWATCHER_TX_MAX_VALUES];

    //array of pointers to void
    void *ArrayValues[SERIALWATCHER_TX_MAX_VALUES]; 

    // array of encoded features | **encoded
    // b0 b1 b2 b3 b4 tells datatype
    // b5 tells if the field has an invalid name (0=ok, | 1=invalid)
    // b6 tells if the field has an invalid value (0=ok, | 1=invalid)
    // b7 tells if the field is readonly (0=read/write | 1=readonly)
    byte ArrayFeatures[SERIALWATCHER_TX_MAX_VALUES];

    //tx memories
    byte TxIdx;

    //rx memories
    char RxBuffer[SERIALWATCHER_RX_ARRAY_LENGTH]; //char array containing serialwatcher rx data ('\0' means completely empty)
    
  //private methods
  private:
    bool CheckIdx(); //true=ok
    bool CheckValidName(const __FlashStringHelper *Name); //true=valid
    bool IsCharValid(char in_char); //error value (not allowed character detected in the value) -> true: value is valid | false: value is invalid
    bool IsTextValid(char *in_achar); //error value (not allowed character detected in the value) -> true: value is valid | false: value is invalid
    bool IsTextValid(const __FlashStringHelper *in_flash); //error name (user inserted character not allowed) -> true: names is valid | false: name is invalid
    void Tx();
    void Rx();
	void StringWrite(char *dst, const __FlashStringHelper *src);    
    int StringPositionSingleChar(char *s_where, char c_char);
    bool StringSearchSubString(char *s_where, const char *s_what);
    bool StringSearchSubString(const __FlashStringHelper *s_where, const char *s_what);
    #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
	void print_unsigned_long_long(Stream &_serial, unsigned long long val);
    void print_long_long(Stream &_serial, long long val);
    long long atoll(char *src);
    unsigned long long atoull(char *src);
	#endif
 
};

#endif
