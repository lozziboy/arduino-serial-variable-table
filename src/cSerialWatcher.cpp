/*
	cSerialWatcher.cpp

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
    > Char:             8 bits (1 byte), from -128 to 127
    > Array of Chars:   8 bits (1 byte) each slot of the array
    > Flashstrings:     8 bit (1 byte) is a pointer of a string stored in flash (program) memory instead of SRAM
    > Text/Empty Space: 8 bit (1 byte) stored in a pointer of a string stored in flash (program) memory instead of SRAM
	
*/

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Arduino.h"
#include "cSerialWatcher.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
cSerialWatcher::cSerialWatcher() { //constructor
  #if SERIALWATCHER_TX_MAX_BOOL>0
    indexbool = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_BYTE>0
    indexbyte = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_INT>0
    indexint = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_UINT>0
    indexuint = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_LONG>0
    indexlong = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_ULONG>0
    indexulong = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_FLOAT>0
    indexfloat = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_CHAR>0
    indexchar = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_ACHAR>0
    indexachar = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_FLASHSTRING>0
    indexflashstring = 0;
  #endif
  #if SERIALWATCHER_TX_MAX_TEXT>0
    indextext = 0;
  #endif
  indexglobal = 0;
  Error_A = B00000000;
  Error_B = B00000000;
  Error_C = B00000000;
  Error_D = B00000000;
  TxIdx = 0;
  TxIdxSub = 0;
  // clear data buffer for the next rx
  RxBuffer[0] = '\0';
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_BOOL>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_BYTE>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_INT>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_UINT>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_LONG>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_ULONG>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_FLOAT>0
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
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_CHAR>0
void cSerialWatcher::Map(const __FlashStringHelper *Name, char &Value, bool readonly) { //char
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check char index
  if (indexchar >=  SERIALWATCHER_TX_MAX_CHAR) {
    Error_B = Error_B | SERIALWATCHER_EB_6; //Exceeded Char
    return;
  }
  //check if first map has been already done
  if (indexglobal != 255) {
    //first map
    //check for valid name
    if (Name != nullptr) {
      if (IsTextValid(Name)) {
        ArrayCharName[indexchar] = Name;
      }
      else {
        ArrayCharName[indexchar] = F("invalid");
        Error_B = Error_B | SERIALWATCHER_EB_7; //Invalid Name Char
      }
    }
    else {
        ArrayCharName[indexachar] = F("invalid");
        Error_B = Error_B | SERIALWATCHER_EB_7; //Invalid Name Char
    }
    //check for valid value
    ArrayCharValue[indexchar] = &Value; //to store the pointer in a variable & is still needed to get the address
    if (not IsCharValid(*ArrayCharValue[indexchar])) {
      Error_C = Error_C | SERIALWATCHER_EC_0; //Invalid Value Char 
    }
    ArrayCharGlobalSeq[indexchar] = indexglobal;
    if (readonly) {
      ArrayCharGlobalSeq[indexchar] = ArrayCharGlobalSeq[indexchar] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
    }
    indexchar += 1;
    indexglobal += 1;
  }
  //successive maps -> update only Value and char index
  else {
    if (not IsCharValid(*ArrayCharValue[indexchar])) {
      Error_C = Error_C | SERIALWATCHER_EC_0; //Invalid Value Char 
    }
    indexchar += 1;
  }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_ACHAR>0
void cSerialWatcher::Map(const __FlashStringHelper *Name, char *Value, bool readonly) { //array of char - pointers to the first element of the array char
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check array of char index
  if (indexachar >=  SERIALWATCHER_TX_MAX_ACHAR) {
    Error_C = Error_C | SERIALWATCHER_EC_1; //Exceeded Array of Char
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
        Error_C = Error_C | SERIALWATCHER_EC_2; //Invalid Name Array of Char
      }
    }
    else {
        ArrayACharName[indexachar] = F("invalid");
        Error_C = Error_C | SERIALWATCHER_EC_2; //Invalid Name Array of Char
    }
    //check for valid value
    ArrayACharValue[indexachar] = Value;
    if (not IsTextValid(ArrayACharValue[indexachar])) {
      Error_C = Error_C | SERIALWATCHER_EC_3; //Invalid Value Array of Char 
    }
    ArrayACharGlobalSeq[indexachar] = indexglobal;
    if (readonly) {
      ArrayACharGlobalSeq[indexachar] = ArrayACharGlobalSeq[indexachar] | SERIALWATCHER_GLOBALSEQ_READONLY; // SET
    }
    indexachar += 1;
    indexglobal += 1;
  }
  //successive maps -> update only Value and achar index
  else {
    if (not IsTextValid(ArrayACharValue[indexachar])) {
      Error_C = Error_C | SERIALWATCHER_EC_3; //Invalid Value Array of Char 
    }
    indexachar += 1;
  }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_FLASHSTRING>0
void cSerialWatcher::Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value) { //flashstring
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check flashstring index
  if (indexflashstring >=  SERIALWATCHER_TX_MAX_FLASHSTRING) {
    Error_C = Error_C | SERIALWATCHER_EC_4; //Exceeded FlashString
    return;
  }
  //check for valid name
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      ArrayFlashStringName[indexflashstring] = Name;
    }
    else {
      ArrayFlashStringName[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_5; //Invalid Name FlashString
    }
  }
  else {
      ArrayFlashStringName[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_5; //Invalid Name FlashString
  }
  //check for valid pointer (value)
  if (Value != nullptr) {
    //check for valid value
    if (IsTextValid(Value)) {
      ArrayFlashStringValue[indexflashstring] = Value;
    }
    else {
      ArrayFlashStringValue[indexflashstring] = F("invalid");
      Error_C = Error_C | SERIALWATCHER_EC_6; //Invalid Value FlashString
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
  //successive map -> update only flashstring index
  else {
    indexflashstring += 1;
  }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_TEXT>0
void cSerialWatcher::Text(const __FlashStringHelper *Value) { //write a carved value
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check texts index
  if (indextext >=  SERIALWATCHER_TX_MAX_TEXT) {
    Error_C = Error_C | SERIALWATCHER_EC_7; //Exceeded Texts
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
      Error_D = Error_D | SERIALWATCHER_ED_0; //Invalid Value Text
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
  //successive map -> update only text index
  else {
    indextext += 1;
  }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SERIALWATCHER_TX_MAX_TEXT>0
void cSerialWatcher::EmptySpace() { //add an empty space
  //update only after every transmission
  if (TxIdx != 0) {
    return;
  }
  //check texts index
  if (indextext >=  SERIALWATCHER_TX_MAX_TEXT) { //check array of texts
      Error_C = Error_C | SERIALWATCHER_EC_7; //Exceeded Texts
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
  //successive map -> update only text index
  else {
    indextext += 1; 
  }
}
#endif
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
    #if SERIALWATCHER_TX_MAX_CHAR>0
      indexchar = 0;
    #endif
    #if SERIALWATCHER_TX_MAX_ACHAR>0
      indexachar = 0;
    #endif
    #if SERIALWATCHER_TX_MAX_FLASHSTRING>0
      indexflashstring = 0;
    #endif
    #if SERIALWATCHER_TX_MAX_TEXT>0
      indextext = 0;
    #endif
    Error_C = Error_C &~ SERIALWATCHER_EC_0; //Clear Invalid Value Char
    Error_C = Error_C &~ SERIALWATCHER_EC_3; //Clear Invalid Value Array of Char
    Error_C = Error_C &~ SERIALWATCHER_EC_6; //Clear Invalid Value FlashString
    Error_D = Error_D &~ SERIALWATCHER_ED_0; //Clear Invalid Value Text
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsCharValid(char in_char) { //true: value is valid | false: value is invalid
  if ( in_char != *SERIALWATCHER_TX_FIELD_SEPARATOR and //*in_char -> char at the address pointed by *in_char
       in_char != *SERIALWATCHER_TX_CODE_SEPARATOR and
       in_char != *SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE) {
    return true;
  }
  else {
    return false;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(char *in_achar) { //true: value is valid | false: value is invalid
  if ( not StringSearchSubString(in_achar, SERIALWATCHER_TX_FIELD_SEPARATOR) and
       not StringSearchSubString(in_achar, SERIALWATCHER_TX_CODE_SEPARATOR) and
       not StringSearchSubString(in_achar, SERIALWATCHER_TX_CODE_STREAM_EMPTYSPACE)) {
    return true;
  }
  else {
    return false;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(const __FlashStringHelper *in_flash) { //true: text is valid | false: text is invalid
  if ( not StringSearchSubString(in_flash, SERIALWATCHER_TX_FIELD_SEPARATOR) and 
       not StringSearchSubString(in_flash, SERIALWATCHER_TX_CODE_SEPARATOR) and 
       not StringSearchSubString(in_flash, SERIALWATCHER_TX_CODE_SEPARATOR) ) { 
    return true;
  }
  else {
    return false;
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
    #if SERIALWATCHER_TX_MAX_BOOL>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream byte
  case 3:
    #if SERIALWATCHER_TX_MAX_BYTE>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream int
  case 4:
    #if SERIALWATCHER_TX_MAX_INT>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream unsigned int
  case 5:
    #if SERIALWATCHER_TX_MAX_UINT>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream long
  case 6:
    #if SERIALWATCHER_TX_MAX_LONG>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream unsigned long
  case 7:
    #if SERIALWATCHER_TX_MAX_ULONG>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream float
  case 8:
    #if SERIALWATCHER_TX_MAX_FLOAT>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream char
  case 9:
    #if SERIALWATCHER_TX_MAX_CHAR>0
    if ((TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE) < indexchar) {
      e = TxIdxSub + SERIALWATCHER_TX_ELEMENTS_PER_CYCLE;
    }
    else {
      e = indexchar;
    }
    for (byte i=TxIdxSub; i<e; i++) {
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_GLOBAL));
      Serial.print(ArrayCharGlobalSeq[i] & SERIALWATCHER_GLOBALSEQ_VALUE); //global sequence place
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_CHAR));
      Serial.print(i); //index
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(ArrayCharName[i]); //name
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(*ArrayCharValue[i]); //value (* gets the value at the pointed address)
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(IsReadOnly(ArrayCharGlobalSeq[i])); //readonly information
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexchar) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
    #else
    TxIdx += 1;
    break;
    #endif
  //stream array of chars 
  case 10:
    #if SERIALWATCHER_TX_MAX_ACHAR>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream flashstring
  case 11:
    #if SERIALWATCHER_TX_MAX_FLASHSTRING>0
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
      Serial.print(true); //readonly information -> always true for flashstrings
      Serial.print(F(SERIALWATCHER_TX_FIELD_SEPARATOR)); //field separator
      Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); //code separator
      TxIdxSub += 1;
    }
    if (TxIdxSub >= indexflashstring) {
      TxIdxSub = 0;
      TxIdx += 1;
    }
    break;
    #else
    TxIdx += 1;
    break;
    #endif
  //stream texts
  case 12:
    #if SERIALWATCHER_TX_MAX_TEXT>0
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
    #else
    TxIdx += 1;
    break;
    #endif
  //stream errors
  case 13:
    #if SERIALWATCHER_TX_MAX_BOOL>0
    if ((Error_A & SERIALWATCHER_EA_0) == SERIALWATCHER_EA_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BOOL)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Bool
    if ((Error_A & SERIALWATCHER_EA_1) == SERIALWATCHER_EA_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BOOL)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Bool
    #endif
    #if SERIALWATCHER_TX_MAX_BYTE>0
    if ((Error_A & SERIALWATCHER_EA_2) == SERIALWATCHER_EA_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_BYTE)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Byte
    if ((Error_A & SERIALWATCHER_EA_3) == SERIALWATCHER_EA_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_BYTE)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Byte
    #endif
    #if SERIALWATCHER_TX_MAX_INT>0
    if ((Error_A & SERIALWATCHER_EA_4) == SERIALWATCHER_EA_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_INT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Int
    if ((Error_A & SERIALWATCHER_EA_5) == SERIALWATCHER_EA_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_INT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Int
    #endif
    #if SERIALWATCHER_TX_MAX_UINT>0
    if ((Error_A & SERIALWATCHER_EA_6) == SERIALWATCHER_EA_6) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_UINT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Unsigned Int
    if ((Error_A & SERIALWATCHER_EA_7) == SERIALWATCHER_EA_7) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_UINT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Unsigned Int
    #endif
    #if SERIALWATCHER_TX_MAX_LONG>0
    if ((Error_B & SERIALWATCHER_EB_0) == SERIALWATCHER_EB_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_LONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Long
    if ((Error_B & SERIALWATCHER_EB_1) == SERIALWATCHER_EB_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_LONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Long
    #endif
    #if SERIALWATCHER_TX_MAX_ULONG>0
    if ((Error_B & SERIALWATCHER_EB_2) == SERIALWATCHER_EB_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ULONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Unsigned Long
    if ((Error_B & SERIALWATCHER_EB_3) == SERIALWATCHER_EB_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ULONG)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Unsigned Long
    #endif
    #if SERIALWATCHER_TX_MAX_FLOAT>0 
    if ((Error_B & SERIALWATCHER_EB_4) == SERIALWATCHER_EB_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLOAT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Float
    if ((Error_B & SERIALWATCHER_EB_5) == SERIALWATCHER_EB_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLOAT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Float
    #endif
    #if SERIALWATCHER_TX_MAX_CHAR>0 
    if ((Error_B & SERIALWATCHER_EB_6) == SERIALWATCHER_EB_6) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_CHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Char
    if ((Error_B & SERIALWATCHER_EB_7) == SERIALWATCHER_EB_7) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_CHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Char
    if ((Error_C & SERIALWATCHER_EC_0) == SERIALWATCHER_EC_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_CHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value Char
    #endif
    #if SERIALWATCHER_TX_MAX_ACHAR>0 
    if ((Error_C & SERIALWATCHER_EC_1) == SERIALWATCHER_EC_1) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Array of Char
    if ((Error_C & SERIALWATCHER_EC_2) == SERIALWATCHER_EC_2) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name Array of Char
    if ((Error_C & SERIALWATCHER_EC_3) == SERIALWATCHER_EC_3) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_ACHAR)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value Array of Char
    #endif
    #if SERIALWATCHER_TX_MAX_FLASHSTRING>0 
    if ((Error_C & SERIALWATCHER_EC_4) == SERIALWATCHER_EC_4) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded FlashString
    if ((Error_C & SERIALWATCHER_EC_5) == SERIALWATCHER_EC_5) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_NAME_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Name FlashString
    if ((Error_C & SERIALWATCHER_EC_6) == SERIALWATCHER_EC_6) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_FLASHSTRING)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value FlashString
    #endif
    #if SERIALWATCHER_TX_MAX_TEXT>0
    if ((Error_C & SERIALWATCHER_EC_7) == SERIALWATCHER_EC_7) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_EXCEEDED_TEXT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Exceeded Text
    if ((Error_D & SERIALWATCHER_ED_0) == SERIALWATCHER_ED_0) { Serial.print(F(SERIALWATCHER_TX_CODE_STREAM_ERROR_VALUE_TEXT)); Serial.print(F(SERIALWATCHER_TX_CODE_SEPARATOR)); } //Invalid Value Text
    #endif
    TxIdx += 1;
    break;
  //stream end
  case 14:
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
  if (Serial.available() > 0) { //only when receive data
    char c = Serial.read(); //read the incoming char
    int rxidx = StringPositionSingleChar(RxBuffer, SERIALWATCHER_RX_BUFFER_END_IDX); //read end of data buffer idx
    if (rxidx == -1) { //rx not in progress
        if (c == SERIALWATCHER_RX_CODE_STREAM_START) {
          RxBuffer[0] = SERIALWATCHER_RX_BUFFER_END_IDX; //put at the first slot Rx_Buffer End index
        }
    }
    else { //rx in progress
      if (c != SERIALWATCHER_RX_CODE_STREAM_END) {
        if (rxidx < (SERIALWATCHER_RX_ARRAY_LENGTH -1)) { 
          RxBuffer[rxidx] = c; //collect character
          RxBuffer[rxidx+1] = SERIALWATCHER_RX_BUFFER_END_IDX; //shift Rx_Buffer End index
        }
        else {
          //error: rx buffer lenght overflow
          #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
            Serial.println();
            Serial.print(F("Error! Rx Buffer Lenght Overflow"));
            delay(5000);
          #endif
          //: clear data buffer for the next rx
          RxBuffer[0] = '\0';
          return;
        }
      } 
      else { //stream end
        //terminate the string
        RxBuffer[rxidx] = '\0';
        //: diagnostics
        #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
          Serial.println();
          Serial.println();
          Serial.print(F("RxBuffer "));
          Serial.print(RxBuffer);
        #endif
        //: Parse Data: split the data into its parts
        char qu[2];
        qu[0] = SERIALWATCHER_RX_FIELD_SEPARATOR;
        qu[1] = '\0';
        // DataType - get the first part
        char DataType = strtok(RxBuffer, qu)[0]; //strtok(): breaks the string into a series of substrings; it returns a pointer to the beginning of each substring. If delimiter is not found returns a null pointer
        // ArrayPlace - get the second part
        byte ArrayPlace = atoi(strtok(NULL, qu)); //strtok(NULL,..) continues where the previous call left off
        // Value - get the third part
        char *Value = strtok(NULL, qu); //strtok(NULL,..) continues where the previous call left off
        //: diagnostics
        #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
          Serial.println();
          Serial.print(F("DataType "));
          Serial.println(DataType);
          Serial.print(F("ArrayPlace "));
          Serial.println(ArrayPlace);
          Serial.print(F("Value "));
          Serial.println(Value);
          Serial.println();
        #endif
        if (Value != NULL) {
          //: write variables ----> put data inside variables using pointers
          #if SERIALWATCHER_TX_MAX_BOOL>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_BOOL) {
              *ArrayBoolValue[ArrayPlace] = (Value[0] == '1');
            }
          #endif
          #if SERIALWATCHER_TX_MAX_BYTE>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_BYTE) {
              *ArrayByteValue[ArrayPlace] = atoi(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_INT>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_INT) {
              *ArrayIntValue[ArrayPlace] = atoi(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_UINT>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_UINT) {
              *ArrayUIntValue[ArrayPlace] = atoi(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_LONG>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_LONG) {
              *ArrayLongValue[ArrayPlace] = atol(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_ULONG>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_ULONG) {
              *ArrayULongValue[ArrayPlace] = atol(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_FLOAT>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_FLOAT) {
              *ArrayFloatValue[ArrayPlace] = atof(Value);
            }
          #endif
          #if SERIALWATCHER_TX_MAX_CHAR>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_CHAR) {
              *ArrayCharValue[ArrayPlace] = Value[0];
            }
          #endif
          #if SERIALWATCHER_TX_MAX_ACHAR>0
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_ACHAR) {
              strcpy(ArrayACharValue[ArrayPlace], Value); //the destination string should be large enough to receive the copy!
            }
          #endif
        }
        else {
          #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
            Serial.println();
            Serial.print(F("Error! Value is NULL pointer"));
          #endif
        }
        #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
          delay(5000);
        #endif
        //: clear data buffer for the next rx
        RxBuffer[0] = '\0';
      }
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::StringWrite(char *dst, const __FlashStringHelper *src) {
  //the destination string should be large enough to receive the copy
  strcpy_P(dst, (const char*) src);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int cSerialWatcher::StringPositionSingleChar(char *s_where, char c_char) {
  char *pch = strchr(s_where, c_char);
  if (pch == NULL) {
    return -1;
  }
  else {
    return pch-s_where;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//return True if string s_what is found inside string s_where
bool cSerialWatcher::StringSearchSubString(char *s_where, char *s_what) {
  if (strstr(s_where, s_what) == NULL) { 
    return false; 
  }
  else { 
    return true; 
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//return True if string s_what is found inside s_where
bool cSerialWatcher::StringSearchSubString(const __FlashStringHelper *s_where, char *s_what) {
  size_t siz = strlen_P((const char*) s_where); //returns the number of characters in src (that's a pointer to an array of char in program space)
  siz += 1; //+1 for the null terminator
  siz = constrain(siz, 0, 256);
  char dst[siz];
  StringWrite(dst, s_where);
  if (strstr(dst, s_what) == NULL) { 
    return false; 
  }
  else { 
    return true; 
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
