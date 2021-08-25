/*
	cSerialWatcher.h

	a fast lightweight real time library to monitor and modify arduino variables via serial usb
	this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266

	> copy SerialWatcher source library inside your project folder and include it on top of the main.ino
	#include "cSerialWatcher.cpp"

	> create a global SerialWatch instance
	cSerialWatcher SerialWatcher;

	> init Serial communication inside the setup() routine
	Serial.begin(115200);

	> map the variables inside the main loop():
	- each variable to monitor/edit can be mapped to the GUI through the following instruction,
	  readonly is optional and set to false by default
	  SerialWatcher.Map(F("Name_That_I_Like"), GlobalVariableName(any), Readonly(bool));

		- is possible to add empty spaces between lines through the following instruction
			SerialWatcher.EmptySpace();

		- is possible to add custom texts through the following instruction
			SerialWatcher.Text(F("My custom text"));

		- after the variables mapping, is necessary to call the update instruction to refresh them all at each loop scan
			SerialWatcher.Update();

	> upload the software to the shield
	
	> run SerialWatcherApp on your computer to monitor/edit the mapped global variables
		https://github.com/lozziboy/arduino-serial-watcher-app

	Supported Data-Types 
	> Bool:             8 bit (1 byte) two possible values, true or false
	> Byte:             8 bit (1 byte) unsigned number, from 0 to 255
	> Int:              16 bit (2 byte), from -32,768 to 32,767
	> Unsigned Int:     16 bit (2 byte) unsigned number, from 0 to 65,535
	> Long:             32 bit (4 bytes), from -2,147,483,648 to 2,147,483,647
	> Unsigned Long:    32 bit (4 bytes) unsigned number, from 0 to 4,294,967,295
	> Float:            32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
	> Array of Chars:   8 bits (1 byte) each slot
	> Flashstrings:     8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
	> Text/Empty Space: 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM
	
	SerialWatcher class memory occupation can be reduced/exdended based on your needs! 
	Just increase/decrease the number of monitorning slots based on which data-type and how many variables to be monitored.
	
*/

#ifndef cSerialWatcher_h
#define cSerialWatcher_h

//Library configuration (base): max number of transmittable types
#define SERIALWATCHER_TX_MAX_BOOL 7        // bool: array 0 .. TX_MAX_BOOL - 1
#define SERIALWATCHER_TX_MAX_BYTE 7        // byte: array 0 .. TX_MAX_BYTE - 1
#define SERIALWATCHER_TX_MAX_INT 7         // int: array 0 .. TX_MAX_INT - 1
#define SERIALWATCHER_TX_MAX_UINT 7        // unsigned int: array 0 .. TX_MAX_UINT - 1
#define SERIALWATCHER_TX_MAX_LONG 7        // long: array 0 .. TX_MAX_LONG - 1
#define SERIALWATCHER_TX_MAX_ULONG 7       // unsigned long: array 0 .. TX_MAX_ULONG - 1
#define SERIALWATCHER_TX_MAX_FLOAT 7       // float: array  0 .. TX_MAX_FLOAT - 1
#define SERIALWATCHER_TX_MAX_ACHAR 7       // array of chars: array 0 .. TX_MAX_ACHAR - 1
#define SERIALWATCHER_TX_MAX_FLASHSTRING 7 // flashstring: array 0 .. TX_MAX_FLASHSTRING - 1
#define SERIALWATCHER_TX_MAX_TEXT 7        // fixed texts: array 0 .. TX_MAX_TEXT - 1

//Library configuration (advanced)
#define SERIALWATCHER_CLASS_DIAGNOSTICS false //write "true" for advanced diagnostics -> leave it to "false" before to release the project
#define SERIALWATCHER_TX_ELEMENTS_PER_CYCLE 1 //how many elements to send in one loop() scan -> minumum value is 1

//Error codes
#define SERIALWATCHER_EA_0 B00000001 //Exceeded Bool: error, reached maximum number of mapped values
#define SERIALWATCHER_EA_1 B00000010 //Invalid Name Bool: error, inserted invalid name on the map function
#define SERIALWATCHER_EA_2 B00000100 //Exceeded Byte
#define SERIALWATCHER_EA_3 B00001000 //Invalid Name Byte
#define SERIALWATCHER_EA_4 B00010000 //Exceeded Int
#define SERIALWATCHER_EA_5 B00100000 //Invalid Name Int
#define SERIALWATCHER_EA_6 B01000000 //Exceeded UInt
#define SERIALWATCHER_EA_7 B10000000 //Invalid Name UInt
#define SERIALWATCHER_EB_0 B00000001 //Exceeded Long
#define SERIALWATCHER_EB_1 B00000010 //Invalid Name Long
#define SERIALWATCHER_EB_2 B00000100 //Exceeded ULong
#define SERIALWATCHER_EB_3 B00001000 //Invalid Name ULong
#define SERIALWATCHER_EB_4 B00010000 //Exceeded Float
#define SERIALWATCHER_EB_5 B00100000 //Invalid Name Float
#define SERIALWATCHER_EB_6 B01000000 //Exceeded Array of Char
#define SERIALWATCHER_EB_7 B10000000 //Invalid Name Array of Char
#define SERIALWATCHER_EC_0 B00000001 //Invalid Value Array of Char
#define SERIALWATCHER_EC_1 B00000010 //Exceeded FlashString
#define SERIALWATCHER_EC_2 B00000100 //Invalid Name FlashString
#define SERIALWATCHER_EC_3 B00001000 //Invalid Value FlashString
#define SERIALWATCHER_EC_4 B00010000 //Exceeded Text
#define SERIALWATCHER_EC_5 B00100000 //Invalid Value Text

// Tx codes
#define SERIALWATCHER_TX_FIELD_SEPARATOR ";"  // identifies the transmission of a field separator -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_SEPARATOR "'"   // identifies the transmission of a code separator (new data) -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE "," // identifies the transmission of an Empty Space -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_STREAM_START "@#s*"   // identifies the transmission stream Start
#define SERIALWATCHER_TX_CODE_STREAM_END "@#e*"     // identifies the transmission stream End
#define SERIALWATCHER_TX_CODE_STREAM_GLOBAL "gg"    // identifies the transmission stream of the Global position index
#define SERIALWATCHER_TX_CODE_STREAM_BOOL "aa"         // identifies the transmission of a Bool
#define SERIALWATCHER_TX_CODE_STREAM_BYTE "ab"         // " Byte
#define SERIALWATCHER_TX_CODE_STREAM_INT "ac"          // " Int
#define SERIALWATCHER_TX_CODE_STREAM_UINT "ad"         // " Unsigned Int
#define SERIALWATCHER_TX_CODE_STREAM_LONG "ae"         // " Long
#define SERIALWATCHER_TX_CODE_STREAM_ULONG "af"        // " Unsigned Long
#define SERIALWATCHER_TX_CODE_STREAM_FLOAT "ag"        // " Float
#define SERIALWATCHER_TX_CODE_STREAM_ACHAR "ah"        // " Array of chars
#define SERIALWATCHER_TX_CODE_STREAM_FLASHSTRING "ai"  // " FlashStrings
#define SERIALWATCHER_TX_CODE_STREAM_TEXT "al"  // " Texts
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BOOL "ba"         // identifies the fact that the has been used an invalid name on the map function
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BYTE "bb"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_INT "bc"          // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_UINT "bd"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_LONG "be"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ULONG "bf"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLOAT "bg"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ACHAR "bh"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLASHSTRING "bi"  // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_ACHAR "ca"        // identifies the fact that the has been used an invalid value on the map function
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_FLASHSTRING "cb"  // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_TEXT "cc"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BOOL "da"        // identifies the fact that the maximum transmittable limit has been reached
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BYTE "db"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_INT "dc"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_UINT "de"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_LONG "df"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ULONG "dg"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLOAT "dh"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ACHAR "di"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLASHSTRING "dl" // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_TEXT "dm" // "

// Rx codes
#define SERIALWATCHER_RX_CODE_STREAM_START '<' //identifies the receipt of the stream Start (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_END '>'   // " End (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_BOOL 'b'  // " Bool (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_BYTE 'd'  // " Byte (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_INT 'i'   // " Int (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_UINT 'n'  // " Unsigned Int (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_LONG 'l'  // " Long (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_ULONG 'u' // " Unsigned Long (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_FLOAT 'f' // " Float (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_ACHAR 's' // " Array of Char (max one character)

//Array Global Sequence encoding
#define SERIALWATCHER_GLOBALSEQ_READONLY 0x8000 // Globalsequence encoded: first bit at left side tells if the field is readonly (0=read/write | 1=readonly)
#define SERIALWATCHER_GLOBALSEQ_VALUE 0x7FFF    // Globalsequence encoded: other bits tells global sequence value

class cSerialWatcher {
  
  // public methods
  public:
    cSerialWatcher(); //constructor
    //& = pass by reference method, when call the function is not necessary to convert input to a pointer
    void Map(const __FlashStringHelper *Name, bool &Value, bool readonly = false); //bool
    void Map(const __FlashStringHelper *Name, byte &Value, bool readonly = false); //byte
    void Map(const __FlashStringHelper *Name, int &Value, bool readonly = false);  //int
    void Map(const __FlashStringHelper *Name, unsigned int &Value, bool readonly = false);  //unsigned int
    void Map(const __FlashStringHelper *Name, long &Value, bool readonly = false);          //long
    void Map(const __FlashStringHelper *Name, unsigned long &Value, bool readonly = false); //unsigned long
    void Map(const __FlashStringHelper *Name, float &Value, bool readonly = false);   //float
    void Map(const __FlashStringHelper *Name, char Value[], bool readonly = false);   //achar
    void Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value);   //flashstring
    void Text(const __FlashStringHelper *Value);   //write a carved value
    void EmptySpace();
    void Update();
    
  // private attributes
  private: 
    byte Error_A; //error memory A
    byte Error_B; //error memory B
    byte Error_C; //error memory C
    //global
    byte indexglobal; //global sequence index | when is 255 means that first map has been done

    //bool
    byte indexbool;
    const __FlashStringHelper *ArrayBoolName[SERIALWATCHER_TX_MAX_BOOL]; //array of pointers to __FlashStringHelper
    bool *ArrayBoolValue[SERIALWATCHER_TX_MAX_BOOL]; //array of pointers to bool
    int ArrayBoolGlobalSeq[SERIALWATCHER_TX_MAX_BOOL]; // array of the global sequence places for bool | **encoded - last bit tells at left side if the field is readonly

    //byte
    byte indexbyte;
    const __FlashStringHelper *ArrayByteName[SERIALWATCHER_TX_MAX_BYTE]; //array of pointers to __FlashStringHelper
    byte *ArrayByteValue[SERIALWATCHER_TX_MAX_BYTE]; //array of pointers to byte
    int ArrayByteGlobalSeq[SERIALWATCHER_TX_MAX_BYTE]; //array of the global sequence places for byte | **encoded - last bit tells at left side if the field is readonly

    //int
    byte indexint;
    const __FlashStringHelper *ArrayIntName[SERIALWATCHER_TX_MAX_INT]; //array of pointers to __FlashStringHelper
    int *ArrayIntValue[SERIALWATCHER_TX_MAX_INT]; //array of pointers to int
    int ArrayIntGlobalSeq[SERIALWATCHER_TX_MAX_INT]; //array of the global sequence places for int | **encoded - last bit tells at left side if the field is readonly

    //unsigned int
    byte indexuint;
    const __FlashStringHelper *ArrayUIntName[SERIALWATCHER_TX_MAX_UINT]; //array of pointers to __FlashStringHelper
    unsigned int *ArrayUIntValue[SERIALWATCHER_TX_MAX_UINT]; //array of pointers to unsigned int
    int ArrayUIntGlobalSeq[SERIALWATCHER_TX_MAX_UINT]; //array of the global sequence places for unsigned int | **encoded - last bit tells at left side if the field is readonly
    
    //long
    byte indexlong;
    const __FlashStringHelper *ArrayLongName[SERIALWATCHER_TX_MAX_LONG]; //array of pointers to __FlashStringHelper
    long *ArrayLongValue[SERIALWATCHER_TX_MAX_LONG]; //array of pointers to long
    int ArrayLongGlobalSeq[SERIALWATCHER_TX_MAX_LONG]; //array of the global sequence places for long | **encoded - last bit tells at left side if the field is readonly

    //unsigned long
    byte indexulong;
    const __FlashStringHelper *ArrayULongName[SERIALWATCHER_TX_MAX_ULONG]; //array of pointers to __FlashStringHelper
    unsigned long *ArrayULongValue[SERIALWATCHER_TX_MAX_ULONG]; //array of pointers to unsigned long
    int ArrayULongGlobalSeq[SERIALWATCHER_TX_MAX_ULONG]; //array of the global sequence places for unsigned long | **encoded - last bit tells at left side if the field is readonly

    //float
    byte indexfloat;
    const __FlashStringHelper *ArrayFloatName[SERIALWATCHER_TX_MAX_FLOAT]; //array of pointers to __FlashStringHelper
    float *ArrayFloatValue[SERIALWATCHER_TX_MAX_FLOAT]; //array of pointers to int
    int ArrayFloatGlobalSeq[SERIALWATCHER_TX_MAX_FLOAT]; //array of the global sequence places for float | **encoded - last bit tells at left side if the field is readonly

    //array of char
    byte indexachar;
    const __FlashStringHelper *ArrayACharName[SERIALWATCHER_TX_MAX_ACHAR]; //array of pointers to __FlashStringHelper
    char *ArrayACharValue[SERIALWATCHER_TX_MAX_ACHAR]; //array of pointers to the first element of the array char
    int ArrayACharGlobalSeq[SERIALWATCHER_TX_MAX_ACHAR]; //array of the global sequence places for array of char | **encoded - last bit tells at left side if the field is readonly

    //flashstring
    byte indexflashstring;
    const __FlashStringHelper *ArrayFlashStringName[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of pointers to __FlashStringHelper
    const __FlashStringHelper *ArrayFlashStringValue[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of pointers to __FlashStringHelper
    int ArrayFlashStringGlobalSeq[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of the global sequence places for flashstring

    //texts (carved values | empty spaces)
    byte indextext;
    const __FlashStringHelper *ArrayTextValue[SERIALWATCHER_TX_MAX_TEXT]; //array of pointers to __FlashStringHelper
    int ArrayTextGlobalSeq[SERIALWATCHER_TX_MAX_TEXT]; //array of the global sequence places for texts

    //tx memories
    byte TxIdx;
    byte TxIdxSub;
    //rx memories
    char RxChars[32]; //serialwatcher rx char number
    bool RxInProgress;
    byte RxIdx;
    
  //private methods
  private:
  	bool StringSearchSingleChar(const __FlashStringHelper *s_where, const char *c_char);
	  bool StringSearchSingleChar(char s_where[], const char *c_char);
    bool IsTextValid(const __FlashStringHelper *Name); //error name (user inserted character not allowed) -> true: names is valid | false: name is invalid
    bool IsTextValid(char Value[]); //error string value (character not allowed detected) -> true: value is valid | false: value is invalid
    bool IsReadOnly(int GlobalSeqValue);
    void Tx();
    void Rx();
    
};

#endif
