//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// CLASS SERIAL WATCHER - version 4.0.0
// Class to watch and modify global variables using a pc connected in serial
// 
//  How to use it:
//
//  > The first thing to do is to create a global SerialWatch instance 
//      cSerialWatcher SerialWatcher;
//
//  > Then, inside the main loop():
//     . each variable to monitor and edit can be mapped to the GUI through
//        SerialWatcher.Map(F("Name_That_I_Like"), GlobalVariableName(any), Readonly(bool)); //Readonly is optional and set to false by default
//
//     . is possible to add empty spaces between the monitored variables through the following instruction
//        SerialWatcher.EmptySpace();
//
//     . is possible to add custom texts through the following instruction
//        SerialWatcher.Text(F("My custom text"));
//
//     . after the variables mapping, is necessary to call this instruction to update all the mapped variables data
//        SerialWatcher.Update();
//    
//  > Upload the software to the mcu and then run SerialWatcherApp on a laptop to monitor and edit the mapped global variables
//
//  Supported Data-Types 
//  > Bool:             8 bit (1 byte) two possible values, true or false
//  > Byte:             8 bit (1 byte) unsigned number, from 0 to 255
//  > Int:              16 bit (2 byte), from -32,768 to 32,767
//  > Unsigned Int:     16 bit (2 byte) unsigned number, from 0 to 65,535
//  > Long:             32 bit (4 bytes), from -2,147,483,648 to 2,147,483,647
//  > Unsigned Long:    32 bit (4 bytes) unsigned number, from 0 to 4,294,967,295
//  > Float:            32 bits (4 bytes) floating-point numbers, from -3.4028235E+38 to 3.4028235E+38
//  > Array of Chars:   8 bits (1 byte) each slot
//  > Flashstrings:     8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
//  > Text/Empty Space: 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//Library configuration
#define SERIALWATCHER_CLASS_DIAGNOSTICS false //write "true" for advanced diagnostics -> leave it to "false" before to release the project
#define SERIALWATCHER_TX_ELEMENTS_PER_CYCLE 1 //how many elements to send in one loop() scan -> minumum value is 1
#define SERIALWATCHER_TX_MAX_BOOL 30   //number of transmittable bool: array 0 .. TX_MAX_BOOL - 1
#define SERIALWATCHER_TX_MAX_BYTE 10   //max number of transmittable byte: array 0 .. TX_MAX_BYTE - 1
#define SERIALWATCHER_TX_MAX_INT 15    //max number of transmittable int: array 0 .. TX_MAX_INT - 1
#define SERIALWATCHER_TX_MAX_UINT 1    //max number of transmittable unsigned int: array 0 .. TX_MAX_UINT - 1
#define SERIALWATCHER_TX_MAX_LONG 5    //max number of transmittable long: array 0 .. TX_MAX_LONG - 1
#define SERIALWATCHER_TX_MAX_ULONG 1  //max number of transmittable unsigned long: array 0 .. TX_MAX_ULONG - 1
#define SERIALWATCHER_TX_MAX_FLOAT 20  //max number of transmittable float: array  0 .. TX_MAX_FLOAT - 1
#define SERIALWATCHER_TX_MAX_ACHAR 5   //maxum number of transmittable array of chars: array 0 .. TX_MAX_ACHAR - 1
#define SERIALWATCHER_TX_MAX_FLASHSTRING 1 //maxum number of transmittable flashstring: array 0 .. TX_MAX_FLASHSTRING - 1
#define SERIALWATCHER_TX_MAX_TEXT 25 //maxum number of transmittable fixed texts: array 0 .. TX_MAX_TEXT - 1

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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
cSerialWatcher::cSerialWatcher() { //constructor
  indexbool = 0;
  indexbyte = 0;
  indexint = 0;
  indexuint = 0;
  indexlong = 0;
  indexulong = 0;
  indexfloat = 0;
  indexachar = 0;
  indexflashstring = 0;
  indextext = 0;
  indexglobal = 0;
  Error_A = B00000000;
  Error_B = B00000000;
  Error_C = B00000000;
  TxIdx = 0;
  TxIdxSub = 0;
  RxInProgress = false;
  RxIdx = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, bool &Value, bool readonly) { //bool
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check bool index
  if (indexbool >=  SERIALWATCHER_TX_MAX_BOOL) {
    Error_A = Error_A | SERIALWATCHER_EA_0; //Exceeded Bool
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayBoolName[indexbool] = Name;
    }
    else {
      ArrayBoolName[indexbool] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_1; //Invalid Name Bool
    }
  }
  else {
      ArrayBoolName[indexbool] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_1; //Invalid Name Bool
  }
  //map the bool
  ArrayBoolValue[indexbool] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayBoolGlobalSeq[indexbool] = indexglobal;
  if (readonly) {
    ArrayBoolGlobalSeq[indexbool] = ArrayBoolGlobalSeq[indexbool] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexbool += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, byte &Value, bool readonly) { //byte
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check byte index
  if (indexbyte >=  SERIALWATCHER_TX_MAX_BYTE) {
    Error_A = Error_A | SERIALWATCHER_EA_2; //Exceeded Byte
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayByteName[indexbyte] = Name;
    }
    else {
      ArrayByteName[indexbyte] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_3; //Invalid Name Byte
    }
  }
  else {
    ArrayByteName[indexbyte] = F("invalid");
    Error_A = Error_A | SERIALWATCHER_EA_3; //Invalid Name Byte
  }
  //map the byte
  ArrayByteValue[indexbyte] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayByteGlobalSeq[indexbyte] = indexglobal;
  if (readonly) {
    ArrayByteGlobalSeq[indexbyte] = ArrayByteGlobalSeq[indexbyte] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexbyte += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, int &Value, bool readonly) { //int
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check int index
  if (indexint >=  SERIALWATCHER_TX_MAX_INT) {
    Error_A = Error_A | SERIALWATCHER_EA_4; //Exceeded Int
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayIntName[indexint] = Name;
    }
    else {
      ArrayIntName[indexint] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_5; //Invalid Name Int
    }
  }
  else {
      ArrayIntName[indexint] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_5; //Invalid Name Int
  }
  //map the int
  ArrayIntValue[indexint] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayIntGlobalSeq[indexint] = indexglobal;
  if (readonly) {
    ArrayIntGlobalSeq[indexint] = ArrayIntGlobalSeq[indexint] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexint += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, unsigned int &Value, bool readonly) { //unsigned int
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check unsigned int index
  if (indexuint >=  SERIALWATCHER_TX_MAX_UINT) {
    Error_A = Error_A | SERIALWATCHER_EA_6; //Exceeded Unsigned Int
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayUIntName[indexuint] = Name;
    }
    else {
      ArrayUIntName[indexuint] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_7; //Invalid Name Unsigned Int
    }
  }
  else {
      ArrayUIntName[indexuint] = F("invalid");
      Error_A = Error_A | SERIALWATCHER_EA_7; //Invalid Name Unsigned Int
  }
  //map the unsigned int
  ArrayUIntValue[indexuint] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayUIntGlobalSeq[indexuint] = indexglobal;
  if (readonly) {
    ArrayUIntGlobalSeq[indexuint] = ArrayUIntGlobalSeq[indexuint] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexuint += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, long &Value, bool readonly) { //long
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check long index
  if (indexlong >=  SERIALWATCHER_TX_MAX_LONG) {
    Error_B = Error_B | SERIALWATCHER_EB_0; //Exceeded Long
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayLongName[indexlong] = Name;
    }
    else {
      ArrayLongName[indexlong] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_1; //Invalid Name Long
    }
  }
  else {
      ArrayLongName[indexlong] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_1; //Invalid Name Long
  }
  //map the long
  ArrayLongValue[indexlong] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayLongGlobalSeq[indexlong] = indexglobal;
  if (readonly) {
    ArrayLongGlobalSeq[indexlong] = ArrayLongGlobalSeq[indexlong] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexlong += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, unsigned long &Value, bool readonly) { //unsigned long
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check unsigned long index
  if (indexulong >= SERIALWATCHER_TX_MAX_ULONG) {
    Error_B = Error_B | SERIALWATCHER_EB_2; //Exceeded Unsigned Long
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayULongName[indexulong] = Name;
    }
    else {
      ArrayULongName[indexulong] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_3; //Invalid Name Unsigned Long
    }
  }
  else {
      ArrayULongName[indexulong] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_3; //Invalid Name Unsigned Long
  }
  //map the unsigned long
  ArrayULongValue[indexulong] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayULongGlobalSeq[indexulong] = indexglobal;
  if (readonly) {
    ArrayULongGlobalSeq[indexulong] = ArrayULongGlobalSeq[indexulong] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexulong += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, float &Value, bool readonly) { //float
  //do this only at first map
  if (indexglobal == 255) {
    return;
  }
  //check float index
  if (indexfloat >=  SERIALWATCHER_TX_MAX_FLOAT) {
    Error_B = Error_B | SERIALWATCHER_EB_4; //Exceeded Float
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayFloatName[indexfloat] = Name;
    }
    else {
      ArrayFloatName[indexfloat] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_5; //Invalid Name Float
    }
  }
  else {
      ArrayFloatName[indexfloat] = F("invalid");
      Error_B = Error_B | SERIALWATCHER_EB_5; //Invalid Name Float
  }
  //map the float
  ArrayFloatValue[indexfloat] = &Value; //to store the pointer in a variable & is still needed to get the address
  ArrayFloatGlobalSeq[indexfloat] = indexglobal;
  if (readonly) {
    ArrayFloatGlobalSeq[indexfloat] = ArrayFloatGlobalSeq[indexfloat] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
  }
  indexfloat += 1;
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, char Value[], bool readonly) { //array of char
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check array of char index
  if (indexachar >=  SERIALWATCHER_TX_MAX_ACHAR) {
    Error_B = Error_B | SERIALWATCHER_EB_6; //Exceeded Array of Char
    return;
  }
  //check if first map has been already done
  if (indexglobal != 255) {
    //first map
    //check for valid name
    if (Name != nullptr) {
      if (IsTextValid(Name)) {
        ArrayACharName[indexachar] = Name;
      }
      else {
        ArrayACharName[indexachar] = F("invalid");
        Error_B = Error_B | SERIALWATCHER_EB_7; //Invalid Name Array of Char
      }
    }
    else {
        ArrayACharName[indexachar] = F("invalid");
        Error_B = Error_B | SERIALWATCHER_EB_7; //Invalid Name Array of Char
    }
    //check for valid value
    ArrayACharValue[indexachar] = Value;
    if (not IsTextValid(ArrayACharValue[indexachar])) {
      Error_C = Error_C | SERIALWATCHER_EC_0; //Invalid Value Array of Char 
    }
    ArrayACharGlobalSeq[indexachar] = indexglobal;
    if (readonly) {
      ArrayACharGlobalSeq[indexachar] = ArrayACharGlobalSeq[indexachar] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
    }
    indexachar += 1;
    indexglobal += 1;
  }
  else {
    //successive maps -> update only Value and achar index
    if (not IsTextValid(ArrayACharValue[indexachar])) {
      Error_C = Error_C | SERIALWATCHER_EC_0; //Invalid Value Array of Char 
    }
    indexachar += 1;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value) { //flashstring
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check flashstring index
  if (indexflashstring >=  SERIALWATCHER_TX_MAX_FLASHSTRING) {
    Error_C = Error_C | SERIALWATCHER_EC_1; //Exceeded FlashString
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayFlashStringName[indexflashstring] = Name;
    }
    else {
      ArrayFlashStringName[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_2; //Invalid Name FlashString
    }
  }
  else {
      ArrayFlashStringName[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_2; //Invalid Name FlashString
  }
  //check for valid pointer (value)
  if (Value != nullptr) {
    //check for valid value
    if (IsTextValid(Value)) {
      ArrayFlashStringValue[indexflashstring] = Value;
    }
    else {
      ArrayFlashStringValue[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_3; //Invalid Value FlashString
    }
  }
  else {
    ArrayFlashStringValue[indexflashstring] = F("");
  }
  //check if first map has been already done
  if (indexglobal != 255) {  
    //first map -> update array global sequence and increase all indexes
    ArrayFlashStringGlobalSeq[indexflashstring] = indexglobal;
    indexflashstring += 1;
    indexglobal += 1;
  }
  else {
    //successive map -> update only flashstring index
    indexflashstring += 1;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Text(const __FlashStringHelper *Value) { //write a carved value
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check texts index
  if (indextext >=  SERIALWATCHER_TX_MAX_TEXT) {
    Error_C = Error_C | SERIALWATCHER_EC_4; //Exceeded Texts
    return;
  }
  //check for valid pointer (value)
  if (Value != nullptr) {
    //check for valid value
    if (IsTextValid(Value)) {
      ArrayTextValue[indextext] = Value;
    }
    else {
      ArrayTextValue[indextext] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_5; //Invalid Value Text
    }
  }
  else{
    ArrayTextValue[indextext] = F("");
  }
  //check if first map has been already done
  if (indexglobal != 255) {
    //first map -> update array global sequence and increase all indexes
    ArrayTextGlobalSeq[indextext] = indexglobal;
    indextext += 1;
    indexglobal += 1;
  }
  else {
    //successive map -> update only text index
    indextext += 1;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::EmptySpace() { //add an empty space
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check texts index
  if (indextext >=  SERIALWATCHER_TX_MAX_TEXT) { //check array of texts
      Error_C = Error_C | SERIALWATCHER_EC_4; //Exceeded Texts
      return;
  }
  //map value
  ArrayTextValue[indextext] = F(SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE);
  //check if first map has been already done
  if (indexglobal != 255) {
    //first map -> update array global sequence and increase all indexes
    ArrayTextGlobalSeq[indextext] = indexglobal;
    indextext += 1;
    indexglobal += 1;
  }
  else {
    //successive map -> update only text index
    indextext += 1; 
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Update(){
  //first map
  if (indexglobal != 255) {
    indexglobal = 255; //first map done
  }
  else {
    Tx();
    Rx();
  }
  //things to be cleared after every transmission because refreshed by the Map function
  if (TxIdx == 0) {
    indexachar = 0;
    indexflashstring = 0;
    indextext = 0;
    Error_C = Error_C &~ SERIALWATCHER_EC_0; //Invalid Value Array of Char: CLEAR
    Error_C = Error_C &~ SERIALWATCHER_EC_3; //Invalid Value FlashString: CLEAR
    Error_C = Error_C &~ SERIALWATCHER_EC_5; //Invalid Value Text: CLEAR
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::StringSearchSingleChar(const __FlashStringHelper *s_where, const char *c_char) {  //return True if c_char is found inside s_where 
  size_t siz = strlen_P((const char*) s_where); //returns the number of characters in src (that's a pointer to an array of char in program space)
  siz += 1; //+1 for the null terminator
  siz = constrain(siz, 0, 256);
  char dst[siz];
  strncpy_P(dst, (const char*) s_where, siz);
  dst[siz-1] = '\0';
  if (strpbrk(dst, c_char) == NULL) { return false; }
  else { return true; }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::StringSearchSingleChar(char s_where[], const char *c_char) { //return True if c_char is found inside s_where
  if (strpbrk(s_where, c_char) == NULL) { return false; }
  else { return true; }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(const __FlashStringHelper *Text) {  //true: text is valid | false: text is invalid
  if ( StringSearchSingleChar(Text, SERIALWATCHER_TX_FIELD_SEPARATOR) or 
       StringSearchSingleChar(Text, SERIALWATCHER_TX_CODE_SEPARATOR) or 
       StringSearchSingleChar(Text, SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE)) { 
    return false;
  }
  else {
    return true;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(char Text[]) { //true: value is valid | false: value is invalid
  if ( StringSearchSingleChar(Text, SERIALWATCHER_TX_FIELD_SEPARATOR) or
       StringSearchSingleChar(Text, SERIALWATCHER_TX_CODE_SEPARATOR) or 
       StringSearchSingleChar(Text, SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE)) {
    return false;
  }
  else {
    return true;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsReadOnly(int GlobalSeqValue) {
  return (GlobalSeqValue & SERIALWATCHER_GLOBALSEQ_READONLY) == SERIALWATCHER_GLOBALSEQ_READONLY;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Transmit data to PC with start and end markers -
// this function transmits data to pc in this format <DataType:ArrayPlace:Value>, where for example
//  
//   @#s*;
//   g9'f5'namea'51.5'1;
//   g14's1'nameb'txt'0;
//   @#e*;
//
// should be interpreted as
//
// @#s*   is the stream start
//
// g9     is the place in the global sequence
// f5     is the data type in trasmission, the fifth value in the array of float
// namea  is the name
// 51.5   is the value
// 1      means that the parameter is readonly
//
// g15    is the place in the global sequence
// s1     is the data type in trasmission, the fifth value in the array of float
// nameb  is the name
// txt   is the value
// 0      means that the parameter is read/write
//
// @#e*   is the stream end
//
void cSerialWatcher::Tx() {
	byte e;
  switch (TxIdx) {
  case 0:  
    TxIdx += 1;
    break;  
  //stream start
  case 1:
    Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_START));
    Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
    TxIdx += 1;
    break;  
  //steam bool
  case 2:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexbool) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexbool;
    } 
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayBoolGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_BOOL));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayBoolName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayBoolValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayBoolGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexbool) {
      TxIdxSub = 0; 
      TxIdx += 1;
    }
    break;
  //stream byte
  case 3: 
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexbyte) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexbyte;
    }    
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayByteGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_BYTE));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayByteName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayByteValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayByteGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    } 
    if (TxIdxSub >= indexbyte) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream int
  case 4:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexint) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexint;
    }    
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayIntGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_INT));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayIntName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayIntValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayIntGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    } 
    if (TxIdxSub >= indexint) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream unsigned int
  case 5:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexuint) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexuint;
    }    
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayUIntGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_UINT));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayUIntName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayUIntValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayUIntGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    } 
    if (TxIdxSub >= indexuint) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream long
  case 6:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexlong) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexlong;
    }    
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayLongGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_LONG));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayLongName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayLongValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayLongGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    } 
    if (TxIdxSub >= indexlong) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream unsigned long
  case 7:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexulong) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexulong;
    }    
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayULongGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ULONG));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayULongName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayULongValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayULongGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    } 
    if (TxIdxSub >= indexulong) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream float
  case 8:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexfloat) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexfloat;
    }
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayFloatGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_FLOAT));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayFloatName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayFloatValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayFloatGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexfloat) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream array of chars 
  case 9:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexachar) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexachar;
    }
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayACharGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ACHAR));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayACharName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      if (IsTextValid(ArrayACharValue[i])) { Serial.print(ArrayACharValue[i]); } //value
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayACharGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexachar) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream flashstring
  case 10:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexflashstring) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexflashstring;
    }
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayFlashStringGlobalSeq[i]); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_FLASHSTRING));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayFlashStringName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      if (IsTextValid(ArrayFlashStringValue[i])) { Serial.print(ArrayFlashStringValue[i]); } //value
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(true); //readonly information -> always trye
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexflashstring) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream texts
  case 11:
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indextext) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indextext;
    }
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayTextGlobalSeq[i]); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_TEXT));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator -> name is nothing
      Serial.print(ArrayTextValue[i]); //value
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator -> readonly information is nothing
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indextext) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
  //stream errors
  case 12:
    if ((Error_A & SERIALWATCHER_EA_0) == SERIALWATCHER_EA_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BOOL)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Bool
    if ((Error_A & SERIALWATCHER_EA_1) == SERIALWATCHER_EA_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BOOL)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Bool
    if ((Error_A & SERIALWATCHER_EA_2) == SERIALWATCHER_EA_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BYTE)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Byte
    if ((Error_A & SERIALWATCHER_EA_3) == SERIALWATCHER_EA_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BYTE)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Byte
    if ((Error_A & SERIALWATCHER_EA_4) == SERIALWATCHER_EA_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_INT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Int
    if ((Error_A & SERIALWATCHER_EA_5) == SERIALWATCHER_EA_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_INT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Int
    if ((Error_A & SERIALWATCHER_EA_6) == SERIALWATCHER_EA_6) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_UINT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Unsigned Int
    if ((Error_A & SERIALWATCHER_EA_7) == SERIALWATCHER_EA_7) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_UINT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Unsigned Int
    if ((Error_B & SERIALWATCHER_EB_0) == SERIALWATCHER_EB_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_LONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Long
    if ((Error_B & SERIALWATCHER_EB_1) == SERIALWATCHER_EB_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_LONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Long
    if ((Error_B & SERIALWATCHER_EB_2) == SERIALWATCHER_EB_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ULONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Unsigned Long
    if ((Error_B & SERIALWATCHER_EB_3) == SERIALWATCHER_EB_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ULONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Unsigned Long
    if ((Error_B & SERIALWATCHER_EB_4) == SERIALWATCHER_EB_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLOAT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Float
    if ((Error_B & SERIALWATCHER_EB_5) == SERIALWATCHER_EB_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLOAT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Float
    if ((Error_B & SERIALWATCHER_EB_6) == SERIALWATCHER_EB_6) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Array of Char
    if ((Error_B & SERIALWATCHER_EB_7) == SERIALWATCHER_EB_7) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Array of Char
    if ((Error_C & SERIALWATCHER_EC_0) == SERIALWATCHER_EC_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value Array of Char
    if ((Error_C & SERIALWATCHER_EC_1) == SERIALWATCHER_EC_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded FlashString
    if ((Error_C & SERIALWATCHER_EC_2) == SERIALWATCHER_EC_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name FlashString
    if ((Error_C & SERIALWATCHER_EC_3) == SERIALWATCHER_EC_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value FlashString
    if ((Error_C & SERIALWATCHER_EC_4) == SERIALWATCHER_EC_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_TEXT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Text
    if ((Error_C & SERIALWATCHER_EC_5) == SERIALWATCHER_EC_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_TEXT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value Text
    TxIdx += 1;
    break;
  //stream end
  case 13:
    Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_END));
    Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
    TxIdx += 1;
    break;
  default:
    TxIdx = 0;
    break;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Receive data from PC with start and end markers combined with parsing -
// based on snipped code from https://forum.arduino.cc/index.php?topic=396450
// this function expects to receive data from pc in this format <DataType:ArrayPlace:Value>, where
// <          is the stream start
// DataType   is the data type
// ArrayPlace is the array place
// Value      is the value
// >          is the stream end
// for example <b:12:0.0> is splitted as:   b DataType | 12 ArrayPlace | 0     Value
// for example <i:12:37.0> is splitted as:  i DataType | 12 ArrayPlace | 37    Value
// for example <f:12:86.42> is splitted as: f DataType | 12 ArrayPlace | 86.42 Value
// for example <s:12:text> is splitted as:  s DataType | 12 ArrayPlace | text  Value
//
void cSerialWatcher::Rx() {
  while (Serial.available() > 0) { //only when receive data
    char rc = Serial.read(); //read the incoming char
    if (not RxInProgress and (rc == SERIALWATCHER_RX_CODE_STREAM_START)){
        RxInProgress = true;
        return;
    }
    if (RxInProgress) {
      if (rc != SERIALWATCHER_RX_CODE_STREAM_END) {
        RxChars[RxIdx] = rc;
        if (RxIdx < (sizeof(RxChars) - 1)) { 
          RxIdx++; 
        }
        return;
      } 
      else { //stream end
        RxChars[RxIdx] = '\0'; //terminate the string
        RxInProgress = false;
        RxIdx = 0;
        //: Parse Data: split the data into its parts
        char DataType;
        byte ArrayPlace;
        char *Value;
        // DataType - get the first part
        char *strtokIdx = strtok(RxChars, ":"); //strtok(): if a token is found, return data is a pointer to the beginning of the token
        DataType = strtokIdx[0]; // copy it to datatype
        // ArrayPlace - get the second part
        strtokIdx = strtok(NULL, ":"); //strtok(NULL,..) continues where the previous call left off
        ArrayPlace = atoi(strtokIdx);  // convert this part to an integer
        // Value - get the third part
        Value = strtok(NULL, ":"); //strtok(NULL,..) continues where the previous call left off
        // ShowParsedData ----> that's just for diagnostics
        #if SERIALWATCHER_CLASS_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
          Serial.print(F("DataType "));
          Serial.println(DataType);
          Serial.print(F("ArrayIdx"));
          Serial.println(ArrayPlace);
          Serial.print(F("strtokIdx "));
          Serial.println(strtokIdx);
          Serial.print(F("Value "));
          Serial.println(Value);
          Serial.print(F("Converted Value "));
          Serial.println(atof(Value));
          delay(10000);
        #endif
        //: write variables ----> put data inside variables using pointers
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_BOOL) {
           *ArrayBoolValue[ArrayPlace] = (Value[0] == '1');
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_BYTE) {
           *ArrayByteValue[ArrayPlace] = atoi(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_INT) {
           *ArrayIntValue[ArrayPlace] = atoi(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_UINT) {
           *ArrayUIntValue[ArrayPlace] = atoi(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_LONG) {
           *ArrayLongValue[ArrayPlace] = atol(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_ULONG) {
           *ArrayULongValue[ArrayPlace] = atol(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_FLOAT) {
           *ArrayFloatValue[ArrayPlace] = atof(Value);
        }
        if (DataType == SERIALWATCHER_RX_CODE_STREAM_ACHAR) {
           strcpy(ArrayACharValue[ArrayPlace], Value);
        }
      }
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
