/*
	cSerialWatcher.h

	a fast lightweight real time library to monitor and modify arduino variables via serial usb
	this library is compatible with arduino unoR3 (atmega328p) and all the shields based on esp8266


  Instructions

	1) copy SerialWatcher source library inside your project folder and include it on top of the main.ino
	   #include "cSerialWatcher.h"

  2) init Serial communication inside the setup() routine
	   Serial.begin(115200);

	3) map the variables to watch in the GUI inside the main loop()
     SerialWatcher.Map(F("Name_That_You_Prefer"), GlobalVariableName(Variable_Name), Readonly(bool));

  4) call the update instruction to refresh them all at each loop scan (only once, in the loop)
     SerialWatcher.Update();

  5) upload the software to the shield and then run SerialWatcherApp on your computer to monitor/edit the mapped global variables
		 https://github.com/lozziboy/arduino-serial-watcher-app


  Additional info

  > readonly parameter is optional (default value is false)

  > the following instuction add empty spaces between lines 
      SerialWatcher.EmptySpace();

  > the following instuction add custome texts
      SerialWatcher.Text(F("My custom text"));

  > SerialWatcher class memory occupation can be reduced/exdended based on your needs! 
	  Just  increase/decrease the number of monitorning slots based on which data-type and how many variables to be monitored
      #define SERIALWATCHER_TX_MAX_BOOL 5
      #define SERIALWATCHER_TX_MAX_BYTE 5
      #define SERIALWATCHER_TX_MAX_INT 5
      #define SERIALWATCHER_TX_MAX_UINT 2
      #define SERIALWATCHER_TX_MAX_LONG 5
      #define SERIALWATCHER_TX_MAX_ULONG 0
      #define SERIALWATCHER_TX_MAX_CHAR 0
      #define SERIALWATCHER_TX_MAX_FLOAT 5
      #define SERIALWATCHER_TX_MAX_ACHAR 0
      #define SERIALWATCHER_TX_MAX_FLASHSTRING 5
      #define SERIALWATCHER_TX_MAX_TEXT 5
	  

  Usage example

      SerialWatcher.Map(F("Name"), var, false);         //  | F("name") | variable | readonly |
      SerialWatcher.EmptySpace();
      SerialWatcher.Text(F("Write here your text"));
  

	Supported Data-Types 
	  > Bool:             8 bit (1 byte) two possible values, true or false
    > Byte:             8 bit (1 byte) unsigned number, from 0 to 255
    > Int:              16 bit (2 byte), from -32,768 to 32,767
    > Unsigned Int:     16 bit (2 byte) unsigned number, from 0 to 65,535
    > Long:             32 bit (4 bytes), from -2,147,483,648 to 2,147,483,647
    > Unsigned Long:    32 bit (4 bytes) unsigned number, from 0 to 4,294,967,295
    > Float:            32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
    > Char:             8 bits (1 byte)
    > Array of Chars:   8 bits (1 byte) each slot of the array
    > Flashstrings:     8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
    > Text/Empty Space: 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM
	
*/

#ifndef cSerialWatcher_h
#define cSerialWatcher_h

//Library configuration (base): max number of transmittable types
#ifndef SERIALWATCHER_TX_MAX_BOOL
  #define SERIALWATCHER_TX_MAX_BOOL 5        // bool: array 0 .. TX_MAX_BOOL - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_BYTE
  #define SERIALWATCHER_TX_MAX_BYTE 5         // byte: array 0 .. TX_MAX_BYTE - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_INT
  #define SERIALWATCHER_TX_MAX_INT 5          // int: array 0 .. TX_MAX_INT - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_UINT
  #define SERIALWATCHER_TX_MAX_UINT 5         // unsigned int: array 0 .. TX_MAX_UINT - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_LONG
  #define SERIALWATCHER_TX_MAX_LONG 5         // long: array 0 .. TX_MAX_LONG - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_ULONG
  #define SERIALWATCHER_TX_MAX_ULONG 5        // unsigned long: array 0 .. TX_MAX_ULONG - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_FLOAT
  #define SERIALWATCHER_TX_MAX_FLOAT 5        // float: array  0 .. TX_MAX_FLOAT - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_CHAR
  #define SERIALWATCHER_TX_MAX_CHAR 5         // array of chars: array 0 .. TX_MAX_ACHAR - 1
#endif  
#ifndef SERIALWATCHER_TX_MAX_ACHAR
  #define SERIALWATCHER_TX_MAX_ACHAR 5        // chars: array 0 .. TX_MAX_CHAR - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_FLASHSTRING
  #define SERIALWATCHER_TX_MAX_FLASHSTRING 5  // flashstring: array 0 .. TX_MAX_FLASHSTRING - 1
#endif
#ifndef SERIALWATCHER_TX_MAX_TEXT
  #define SERIALWATCHER_TX_MAX_TEXT 5        // fixed texts: array 0 .. TX_MAX_TEXT - 1
#endif

//Library configuration (advanced)
#define SERIALWATCHER_TX_ELEMENTS_PER_CYCLE 1 //how many elements to send in one loop() scan -> minumum value is 1
#define SERIALWATCHER_RX_ARRAY_LENGTH 26 //lenght of array containing rx data = MAX_EDIT_ACHAR + '<' + 'b' + ':' + '1' + '2' + ':' + '>' + '\0'
#define SERIALWATCHER_RX_ADV_DIAGNOSTICS false //"true" show rx advanced diagnostics -> leave to "false" before to release the project

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
#define SERIALWATCHER_EB_6 B01000000 //Exceeded Char
#define SERIALWATCHER_EB_7 B10000000 //Invalid Name Char
#define SERIALWATCHER_EC_0 B00000001 //Invalid Value Char
#define SERIALWATCHER_EC_1 B00000010 //Exceeded Array of Char
#define SERIALWATCHER_EC_2 B00000100 //Invalid Name Array of Char
#define SERIALWATCHER_EC_3 B00001000 //Invalid Value Array of Char
#define SERIALWATCHER_EC_4 B00010000 //Exceeded FlashString
#define SERIALWATCHER_EC_5 B00100000 //Invalid Name FlashString
#define SERIALWATCHER_EC_6 B01000000 //Invalid Value FlashString
#define SERIALWATCHER_EC_7 B10000000 //Exceeded Text
#define SERIALWATCHER_ED_0 B00000001 //Invalid Value Text

// Tx codes
#define SERIALWATCHER_TX_CODE_STREAM_START "@#s*"   // identifies the transmission stream Start
#define SERIALWATCHER_TX_CODE_STREAM_END "@#e*"     // identifies the transmission stream End
#define SERIALWATCHER_TX_FIELD_SEPARATOR ";"  // identifies the transmission of a field separator -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_SEPARATOR "'"   // identifies the transmission of a code separator (new data) -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE "$" // identifies the transmission of an Empty Space -> this cannot be used for the map function
#define SERIALWATCHER_TX_CODE_STREAM_GLOBAL "gg"    // identifies the transmission stream of the Global position index
#define SERIALWATCHER_TX_CODE_STREAM_BOOL "aa"         // identifies the transmission of a Bool
#define SERIALWATCHER_TX_CODE_STREAM_BYTE "ab"         // " Byte
#define SERIALWATCHER_TX_CODE_STREAM_INT "ac"          // " Int
#define SERIALWATCHER_TX_CODE_STREAM_UINT "ad"         // " Unsigned Int
#define SERIALWATCHER_TX_CODE_STREAM_LONG "ae"         // " Long
#define SERIALWATCHER_TX_CODE_STREAM_ULONG "af"        // " Unsigned Long
#define SERIALWATCHER_TX_CODE_STREAM_FLOAT "ag"        // " Float
#define SERIALWATCHER_TX_CODE_STREAM_CHAR "ah"        // " Float
#define SERIALWATCHER_TX_CODE_STREAM_ACHAR "ai"        // " Array of chars
#define SERIALWATCHER_TX_CODE_STREAM_FLASHSTRING "al"  // " FlashStrings
#define SERIALWATCHER_TX_CODE_STREAM_TEXT "am"  // " Texts
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BOOL "ba"         // identifies the fact that the has been used an invalid name on the map function
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BYTE "bb"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_INT "bc"          // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_UINT "bd"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_LONG "be"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ULONG "bf"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLOAT "bg"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_CHAR "bh"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ACHAR "bi"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLASHSTRING "bl"  // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_CHAR "ch"        // identifies the fact that the has been used an invalid value on the map function
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_ACHAR "ci"        // ""
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_FLASHSTRING "cl"  // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_TEXT "cm"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BOOL "da"        // identifies the fact that the maximum transmittable limit has been reached
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BYTE "db"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_INT "dc"         // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_UINT "dd"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_LONG "de"        // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ULONG "df"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLOAT "dg"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_CHAR "dh"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ACHAR "di"       // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLASHSTRING "dl" // "
#define SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_TEXT "dm" // "

// Rx codes
#define SERIALWATCHER_RX_BUFFER_END_IDX '"'  // identifies the rx data buffer array index (position of the last character)
#define SERIALWATCHER_RX_FIELD_SEPARATOR ':' //identifies the receipt of the stream field separator (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_START '<' //identifies the receipt of the stream Start (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_END '>'   //identifies the receipt of the stream End (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_BOOL 'b'  // " Bool (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_BYTE 'd'  // " Byte (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_INT 'i'   // " Int (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_UINT 'n'  // " Unsigned Int (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_LONG 'l'  // " Long (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_ULONG 'u' // " Unsigned Long (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_FLOAT 'f' // " Float (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_CHAR 'c' // " Char (max one character)
#define SERIALWATCHER_RX_CODE_STREAM_ACHAR 's' // " Array of Char (max one character)

//Array Global Sequence encoding
#define SERIALWATCHER_GLOBALSEQ_READONLY 0x8000 // Globalsequence encoded: first bit at left side tells if the field is readonly (0=read/write | 1=readonly)
#define SERIALWATCHER_GLOBALSEQ_VALUE 0x7FFF    // Globalsequence encoded: other bits tells global sequence value

class cSerialWatcher {
  
  // public methods
  public:
    cSerialWatcher(); //constructor
    //& = pass by reference method, when call the function is not necessary to convert input to a pointer
    #if SERIALWATCHER_TX_MAX_BOOL>0
      void Map(const __FlashStringHelper *Name, bool &Value, bool readonly = false); //bool
    #endif
    #if SERIALWATCHER_TX_MAX_BYTE>0
      void Map(const __FlashStringHelper *Name, byte &Value, bool readonly = false); //byte
    #endif
    #if SERIALWATCHER_TX_MAX_INT>0
      void Map(const __FlashStringHelper *Name, int &Value, bool readonly = false);  //int
    #endif
    #if SERIALWATCHER_TX_MAX_UINT>0
      void Map(const __FlashStringHelper *Name, unsigned int &Value, bool readonly = false);  //unsigned int
    #endif
    #if SERIALWATCHER_TX_MAX_LONG>0
      void Map(const __FlashStringHelper *Name, long &Value, bool readonly = false); //long
    #endif
    #if SERIALWATCHER_TX_MAX_ULONG>0
      void Map(const __FlashStringHelper *Name, unsigned long &Value, bool readonly = false); //unsigned long
    #endif
    #if SERIALWATCHER_TX_MAX_FLOAT>0
      void Map(const __FlashStringHelper *Name, float &Value, bool readonly = false);  //float
    #endif
    #if SERIALWATCHER_TX_MAX_CHAR>0
      void Map(const __FlashStringHelper *Name, char &Value, bool readonly = false);   //char
    #endif
    #if SERIALWATCHER_TX_MAX_ACHAR>0
      void Map(const __FlashStringHelper *Name, char *Value, bool readonly = false);  //achar   char Value[]
    #endif
    #if SERIALWATCHER_TX_MAX_FLASHSTRING>0
      void Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value);   //flashstring
    #endif
    #if SERIALWATCHER_TX_MAX_TEXT>0
      void Text(const __FlashStringHelper *Value);   //write a carved value
    #endif
    void EmptySpace();
    void Update();
    
  // private attributes
  private: 
    byte Error_A; //error memory A
    byte Error_B; //error memory B
    byte Error_C; //error memory C
    byte Error_D; //error memory D
    //global
    byte indexglobal; //global sequence index | when is 255 means that first map has been done

    //bool
    #if SERIALWATCHER_TX_MAX_BOOL>0
      byte indexbool;
      const __FlashStringHelper *ArrayBoolName[SERIALWATCHER_TX_MAX_BOOL]; //array of pointers to __FlashStringHelper
      bool *ArrayBoolValue[SERIALWATCHER_TX_MAX_BOOL]; //array of pointers to bool
      int ArrayBoolGlobalSeq[SERIALWATCHER_TX_MAX_BOOL]; // array of the global sequence places for bool | **encoded - last bit at left side tells if the field is readonly
    #endif
  
    //byte
    #if SERIALWATCHER_TX_MAX_BYTE>0
      byte indexbyte;
      const __FlashStringHelper *ArrayByteName[SERIALWATCHER_TX_MAX_BYTE]; //array of pointers to __FlashStringHelper
      byte *ArrayByteValue[SERIALWATCHER_TX_MAX_BYTE]; //array of pointers to byte
      int ArrayByteGlobalSeq[SERIALWATCHER_TX_MAX_BYTE]; //array of the global sequence places for byte | **encoded - last bit at left side tells if the field is readonly
    #endif

    //int
    #if SERIALWATCHER_TX_MAX_INT>0
      byte indexint;
      const __FlashStringHelper *ArrayIntName[SERIALWATCHER_TX_MAX_INT]; //array of pointers to __FlashStringHelper
      int *ArrayIntValue[SERIALWATCHER_TX_MAX_INT]; //array of pointers to int
      int ArrayIntGlobalSeq[SERIALWATCHER_TX_MAX_INT]; //array of the global sequence places for int | **encoded - last bit at left side tells if the field is readonly
    #endif

    //unsigned int
    #if SERIALWATCHER_TX_MAX_UINT>0
      byte indexuint;
      const __FlashStringHelper *ArrayUIntName[SERIALWATCHER_TX_MAX_UINT]; //array of pointers to __FlashStringHelper
      unsigned int *ArrayUIntValue[SERIALWATCHER_TX_MAX_UINT]; //array of pointers to unsigned int
      int ArrayUIntGlobalSeq[SERIALWATCHER_TX_MAX_UINT]; //array of the global sequence places for unsigned int | **encoded - last bit at left side tells if the field is readonly
    #endif

    //long
    #if SERIALWATCHER_TX_MAX_LONG>0
      byte indexlong;
      const __FlashStringHelper *ArrayLongName[SERIALWATCHER_TX_MAX_LONG]; //array of pointers to __FlashStringHelper
      long *ArrayLongValue[SERIALWATCHER_TX_MAX_LONG]; //array of pointers to long
      int ArrayLongGlobalSeq[SERIALWATCHER_TX_MAX_LONG]; //array of the global sequence places for long | **encoded - last bit at left side tells if the field is readonly
    #endif

    //unsigned long
    #if SERIALWATCHER_TX_MAX_ULONG>0
      byte indexulong;
      const __FlashStringHelper *ArrayULongName[SERIALWATCHER_TX_MAX_ULONG]; //array of pointers to __FlashStringHelper
      unsigned long *ArrayULongValue[SERIALWATCHER_TX_MAX_ULONG]; //array of pointers to unsigned long
      int ArrayULongGlobalSeq[SERIALWATCHER_TX_MAX_ULONG]; //array of the global sequence places for unsigned long | **encoded - last bit at left side tells if the field is readonly
    #endif

    //float
    #if SERIALWATCHER_TX_MAX_FLOAT>0
      byte indexfloat;
      const __FlashStringHelper *ArrayFloatName[SERIALWATCHER_TX_MAX_FLOAT]; //array of pointers to __FlashStringHelper
      float *ArrayFloatValue[SERIALWATCHER_TX_MAX_FLOAT]; //array of pointers to int
      int ArrayFloatGlobalSeq[SERIALWATCHER_TX_MAX_FLOAT]; //array of the global sequence places for float | **encoded - last bit at left side tells if the field is readonly
    #endif

    //char
    #if SERIALWATCHER_TX_MAX_CHAR>0
      byte indexchar;
      const __FlashStringHelper *ArrayCharName[SERIALWATCHER_TX_MAX_CHAR]; //array of pointers to __FlashStringHelper
      char *ArrayCharValue[SERIALWATCHER_TX_MAX_CHAR]; //array of pointers to char
      int ArrayCharGlobalSeq[SERIALWATCHER_TX_MAX_CHAR]; //array of the global sequence places for char | **encoded - last bit at left side tells if the field is readonly
    #endif

    //array of char
    #if SERIALWATCHER_TX_MAX_ACHAR>0
      byte indexachar;
      const __FlashStringHelper *ArrayACharName[SERIALWATCHER_TX_MAX_ACHAR]; //array of pointers to __FlashStringHelper
      char *ArrayACharValue[SERIALWATCHER_TX_MAX_ACHAR]; //array of pointers to the first element of the array char
      int ArrayACharGlobalSeq[SERIALWATCHER_TX_MAX_ACHAR]; //array of the global sequence places for array of char | **encoded - last bit at left side tells if the field is readonly
    #endif

    //flashstring
    #if SERIALWATCHER_TX_MAX_FLASHSTRING>0
      byte indexflashstring;
      const __FlashStringHelper *ArrayFlashStringName[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of pointers to __FlashStringHelper
      const __FlashStringHelper *ArrayFlashStringValue[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of pointers to __FlashStringHelper
      int ArrayFlashStringGlobalSeq[SERIALWATCHER_TX_MAX_FLASHSTRING]; //array of the global sequence places for flashstring
    #endif

    //texts (carved values | empty spaces)
    #if SERIALWATCHER_TX_MAX_TEXT>0
      byte indextext;
      const __FlashStringHelper *ArrayTextValue[SERIALWATCHER_TX_MAX_TEXT]; //array of pointers to __FlashStringHelper
      int ArrayTextGlobalSeq[SERIALWATCHER_TX_MAX_TEXT]; //array of the global sequence places for texts
    #endif

    //tx memories
    byte TxIdx;
    byte TxIdxSub;
    //rx memories
    char RxBuffer[SERIALWATCHER_RX_ARRAY_LENGTH]; //char array containing serialwatcher rx data ('\0' means completely empty)
    
  //private methods
  private:
    bool IsCharValid(char in_char); //error value (not allowed character detected in the value) -> true: value is valid | false: value is invalid
    bool IsTextValid(char *in_achar); //error value (not allowed character detected in the value) -> true: value is valid | false: value is invalid
    bool IsTextValid(const __FlashStringHelper *in_flash); //error name (user inserted character not allowed) -> true: names is valid | false: name is invalid
    bool IsReadOnly(int GlobalSeqValue);
    void Tx();
    void Rx();
    void StringWrite(char *dst, const __FlashStringHelper *src);     
    int StringPositionSingleChar(char *s_where, char c_char);
    bool StringSearchSubString(char *s_where, char *s_what);
    bool StringSearchSubString(const __FlashStringHelper *s_where, char *s_what);
    
      
};

#endif
