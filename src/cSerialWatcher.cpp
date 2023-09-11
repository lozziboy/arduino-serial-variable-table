/*
	cSerialWatcher.h

	a fast lightweight real time library to monitor and modify arduino variables via serial usb
	this library is compatible with arduino unoR3, unoR4, Mega, and shields based on esp8266, esp32
  

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
    > int8_t:           8 bit (1 byte), from -128 to 127
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
#include "Arduino.h"
#include "cSerialWatcher.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
cSerialWatcher::cSerialWatcher(Stream &serial) { //constructor
  _serial = &serial; //to store the pointer in a variable & is still needed to get the address
  indexglobal = 0;
  _mem = B00000000;
  TxIdx = 0;
  // clear data buffer for the next rx
  RxBuffer[0] = '\0';
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::CheckIdx() {
  //first map done -> update index and exit
  if ((_mem & SERIALWATCHER_MEMO_FISTMAPDONE) == SERIALWATCHER_MEMO_FISTMAPDONE) {
    //update index only once at every transmission cycle
    if (TxIdx == 0) {
      if (indexglobal >=  SERIALWATCHER_TX_MAX_VALUES-1) {
        _mem = _mem | SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR; //SET
      }
      else {
        indexglobal += 1;
      }
    }
    return false;
  }
  //check if maximum number of transmittable variables has been exceeded
  if (indexglobal >=  SERIALWATCHER_TX_MAX_VALUES-1) {
    _mem = _mem | SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR; //SET
    return false;
  }
  return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::CheckValidName(const __FlashStringHelper *Name) {
  if (Name != nullptr) {
    if (IsTextValid(Name)) {
      return true;
    }
    else {
      ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_NAME;
    }
  }
  else {
      ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_NAME;
  }
  return false;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//bool
void cSerialWatcher::Map(const __FlashStringHelper *Name, bool &Value, bool readonly) {
  //check first map mand manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_bool;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//int8_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, int8_t &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_int8_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint8_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, uint8_t &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_uint8_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//int16_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, int16_t &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_int16_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint16_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, uint16_t &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_uint16_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
void cSerialWatcher::Map(const __FlashStringHelper *Name, int &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_int32_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//int32_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, int32_t &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_int32_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint32_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, uint32_t &Value, bool readonly) { 
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_uint32_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//int64_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, int64_t &Value, bool readonly) { 
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_int64_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint64_t
void cSerialWatcher::Map(const __FlashStringHelper *Name, uint64_t &Value, bool readonly) { 
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_uint64_t;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//float
void cSerialWatcher::Map(const __FlashStringHelper *Name, float &Value, bool readonly) {
  //check first map and manage if maximum number of transmittable variables has been exceeded
  if (!CheckIdx()) {
    return; //exit
  }
  //map the value
  ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
  //map the data type
  ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_float;
  //map the name
  if (CheckValidName(Name)) {
      ArrayNames[indexglobal] = Name;
  }
  //read only
  if (readonly) {
    ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
  }
  //
  indexglobal += 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//char
void cSerialWatcher::Map(const __FlashStringHelper *Name, char &Value, bool readonly) { 
  //only once at every transmission cycle
  if (TxIdx == 0) {
    //check if maximum number of transmittable variables has been exceeded
    if (indexglobal >=  SERIALWATCHER_TX_MAX_VALUES-1) {
      _mem = _mem | SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR; //SET
      return;
    }
    //first map done -> check for invalid value, update index and exit
    if((_mem & SERIALWATCHER_MEMO_FISTMAPDONE) == SERIALWATCHER_MEMO_FISTMAPDONE) {
      //check for valid value
      if (IsCharValid(*(char*)ArrayValues[indexglobal])) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] &~ SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //CLEAR
      }
      else
      {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //SET
      }
      //update index
      indexglobal += 1;
      //exit
      return;
    }
    //first map
    else {
      //map the value
      ArrayValues[indexglobal] = &Value; //to store the pointer in a variable & is still needed to get the address
      //map the data type
      ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_char;
      //map the name
      if (CheckValidName(Name)) {
          ArrayNames[indexglobal] = Name;
      }
      //check for valid value
      if (IsCharValid(*(char*)ArrayValues[indexglobal])) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] &~ SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //CLEAR
      }
      else
      {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //SET
      }
      //read only
      if (readonly) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
      }
      //
      indexglobal += 1;
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//array of char - pointers to the first element of the array char
void cSerialWatcher::Map(const __FlashStringHelper *Name, char *Value, bool readonly) { 
  //only once at every transmission cycle
  if (TxIdx == 0) {
    //check if maximum number of transmittable variables has been exceeded
    if (indexglobal >=  SERIALWATCHER_TX_MAX_VALUES-1) {
      _mem = _mem | SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR; //SET
      return;
    }
    //first map done -> check for invalid value, update index and exit
    if((_mem & SERIALWATCHER_MEMO_FISTMAPDONE) == SERIALWATCHER_MEMO_FISTMAPDONE) {
      //check for valid value
      if (IsTextValid((char*)ArrayValues[indexglobal])) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] &~ SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //CLEAR
      }
      else
      {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //SET
      }
      //update index
      indexglobal += 1;
      //exit
      return;
    }
    //first map
    else {
      //map the value
      ArrayValues[indexglobal] = Value;
      //map the data type
      ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_achar;
      //map the name
      if (CheckValidName(Name)) {
          ArrayNames[indexglobal] = Name;
      }
      //check for valid value
      if (IsTextValid((char*)ArrayValues[indexglobal])) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] &~ SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //CLEAR
      }
      else
      {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_VALUE; //SET
      }
      //read only
      if (readonly) {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_READONLY; // SET
      }
      //
      indexglobal += 1;
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//flashstring
void cSerialWatcher::Map(const __FlashStringHelper *Name, const __FlashStringHelper *Value) {
  //only once at every transmission cycle
  if (TxIdx == 0) {
    //check if maximum number of transmittable variables has been exceeded
    if (indexglobal >=  SERIALWATCHER_TX_MAX_VALUES-1) {
      _mem = _mem | SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR; //SET
      return;
    }
    //map the data type
    ArrayFeatures[indexglobal] = SERIALWATCHER_DATATYPE_flashstring;
    //map the name
    if (CheckValidName(Name)) {
        ArrayNames[indexglobal] = Name;
    }
    //check for valid pointer (value)
    if (Value != nullptr) {
      //check for valid value
      if (IsTextValid(Value)) {
        ArrayValues[indexglobal] = (__FlashStringHelper*) Value;
      }
      else {
        ArrayFeatures[indexglobal] = ArrayFeatures[indexglobal] | SERIALWATCHER_FEATURES_MASK_INVALID_VALUE;
      }
    }
    else {
      ArrayValues[indexglobal] = (__FlashStringHelper*) F("");
    }
    //
    indexglobal += 1;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//write a fixed text
void cSerialWatcher::Text(const __FlashStringHelper *Value) {
  this->Map(Value, F("")); //uses the flashstring map
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//add an empty space
void cSerialWatcher::EmptySpace() {
  this->Map(F(""), F("")); //uses the flashstring map
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::Update(){
  //first map done
  if ((_mem & SERIALWATCHER_MEMO_FISTMAPDONE) != SERIALWATCHER_MEMO_FISTMAPDONE) {
    _mem = _mem | SERIALWATCHER_MEMO_FISTMAPDONE; //SET
  }
  else {
    Tx();
    Rx();
  }
  //for some datatypes (Char, Array of Char, FlashString) is necessary to execute the map function periodically
  //that's because when there is a change on the value, the following things should be done again
  //- Char: check for invalid value
  //- Array of Char: check for invalid value
  //- FlashString: update the value (because the pointed address in flash memory could change), check for invalid value
  //to achive that, indexglobal should be cleared after every transmission cycle (that happens when TxIdx == 0)
  if (TxIdx == 0) { //at every transmission cycle
    indexglobal = 0;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsCharValid(char in_char) { //true: value is valid | false: value is invalid
  if ( in_char != *SERIALWATCHER_TX_FIELD_SEPARATOR and //*in_char -> char at the address pointed by *in_char
       in_char != *SERIALWATCHER_TX_ELEMENT_SEPARATOR) {
    return true;
  }
  else {
    return false;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(char *in_achar) { //true: value is valid | false: value is invalid
  if ( not this->StringSearchSubString(in_achar, SERIALWATCHER_TX_FIELD_SEPARATOR) and
       not this->StringSearchSubString(in_achar, SERIALWATCHER_TX_ELEMENT_SEPARATOR)) {
    return true;
  }
  else {
    return false;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool cSerialWatcher::IsTextValid(const __FlashStringHelper *in_flash) { //true: text is valid | false: text is invalid
  if ( not this->StringSearchSubString(in_flash, SERIALWATCHER_TX_FIELD_SEPARATOR) and 
       not this->StringSearchSubString(in_flash, SERIALWATCHER_TX_ELEMENT_SEPARATOR)) { 
    return true;
  }
  else {
    return false;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Transmit data to PC with start and end markers -
// this function transmits data to pc in this format <DataType:ArrayPlace:Value>, where for example
//
//   @#s*;
//   1'var1name'0'0;
//   2'var2name'1'64;
//   3'var3name'86'66;
//   4'var4name'51.5'6;
//   5'var5name'txto'8;
//   @#e*;
//
//   should be interpreted as
//   @#s*        the stream start
//   ----
//   1           the slot in the ArrayValues that's needed for when the application writes it back
//   var1name    the name
//   0           the value
//   0b00000000  datatype=bool, invalidname=false invaludvalue=false readonly=false
//   ----
//   2           the slot in the ArrayValues that's needed for when the application writes it back
//   var2name    the name
//   1           the value
//   0b01000000  datatype=bool, invalidname=false invaludvalue=false readonly=true
//   ----
//   3           the slot in the ArrayValues that's needed for when the application writes it back
//   var3name    the name
//   86           the value
//   0b01000010  datatype=int, invalidname=false invaludvalue=false readonly=true
//   ----
//   4           the slot in the ArrayValues that's needed for when the application writes it back
//   var4name    the name
//   51.5        the value
//   0b00000110  datatype=float, invalidname=false invaludvalue=false readonly=false
//   ----
//   5           the slot in the ArrayValues that's needed for when the application writes it back
//   var5name    the name
//   txto        the value
//   0b00001000  datatype=achar, invalidname=false invaludvalue=false readonly=false
//   ----
//   @#e*        the stream end
//
void cSerialWatcher::Tx() {
  byte i;
  switch (TxIdx) {
  //stream start
  case 0:
    _serial->print(F(SERIALWATCHER_TX_CODE_STREAM_START));
    TxIdx += 1;
    break;
  //steam data
  case 1 ... 254:
    i=TxIdx-1;
    if (i >= indexglobal) {
      TxIdx = 255;
      break;
    }
    //number sequence
    _serial->print(i);
    _serial->print(F(SERIALWATCHER_TX_FIELD_SEPARATOR));
    //name
    if ((ArrayFeatures[i] & SERIALWATCHER_FEATURES_MASK_INVALID_NAME) != SERIALWATCHER_FEATURES_MASK_INVALID_NAME) { //!InvalidName
      _serial->print(ArrayNames[i]);
    }
    _serial->print(F(SERIALWATCHER_TX_FIELD_SEPARATOR));
    //value
    if ((ArrayFeatures[i] & SERIALWATCHER_FEATURES_MASK_INVALID_VALUE) != SERIALWATCHER_FEATURES_MASK_INVALID_VALUE) { //!InvalidValue
      switch (ArrayFeatures[i] & SERIALWATCHER_FEATURES_MASK_DATATYPE) {
        case SERIALWATCHER_DATATYPE_bool:
          _serial->print(*(bool*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_int8_t:
          _serial->print(*(int8_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;       
        case SERIALWATCHER_DATATYPE_uint8_t:
          _serial->print(*(uint8_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_int16_t:
          _serial->print(*(int16_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_uint16_t:
          _serial->print(*(uint16_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_int32_t:
          _serial->print(*(int32_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_uint32_t:
          _serial->print(*(uint32_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_int64_t:
		  #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
            this->print_long_long(*_serial, *(int64_t*)ArrayValues[i]);
		  #else
            _serial->print(*(int64_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
		  #endif
          break;
        case SERIALWATCHER_DATATYPE_uint64_t:
		  #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
            this->print_unsigned_long_long(*_serial, *(uint64_t*)ArrayValues[i]);
		  #else
            _serial->print(*(uint64_t*)ArrayValues[i]); //value - the first * gets the value at the pointed address
		  #endif
          break;
        case SERIALWATCHER_DATATYPE_float:
          _serial->print(*(float*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_char:
          _serial->print(*(char*)ArrayValues[i]); //value - the first * gets the value at the pointed address
          break;
        case SERIALWATCHER_DATATYPE_achar:
          _serial->print((char*)ArrayValues[i]); //value
          break;
        case SERIALWATCHER_DATATYPE_flashstring:
          _serial->print((const __FlashStringHelper*)ArrayValues[i]); //value
          break;
      }
    }
    _serial->print(F(SERIALWATCHER_TX_FIELD_SEPARATOR));
    //features (includes datatype)
    _serial->print(ArrayFeatures[i]);
    _serial->print(F(SERIALWATCHER_TX_ELEMENT_SEPARATOR));
    TxIdx += 1;
    break;
  //stream end
  case 255:
    //maximum number of transmittable variables has been exceeded
    if ((_mem & SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR) == SERIALWATCHER_MEMO_EXCEEDED_MAX_TXVAR)
    {
      _serial->print(F(SERIALWATCHER_TX_CODE_EXCEEDED_MAX_TXVAR)); 
      _serial->print(F(SERIALWATCHER_TX_ELEMENT_SEPARATOR));
    } 
    _serial->print(F(SERIALWATCHER_TX_CODE_STREAM_END));
    TxIdx = 0;
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
  if (_serial->available() > 0) { //only when receive data
    char c = _serial->read(); //read the incoming char
    int rxidx = this->StringPositionSingleChar(RxBuffer, SERIALWATCHER_RX_BUFFER_END_IDX); //read end of data buffer idx
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
            _serial->println();
            _serial->print(F("Error! Rx Buffer Lenght Overflow"));
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
          _serial->println();
          _serial->println();
          _serial->print(F("RxBuffer "));
          _serial->print(RxBuffer);
        #endif
        //: Parse Data: split the data into its parts using strtok()
        // strtok(char *string, const char *delim): breaks the string into a series of substrings. It returns a pointer to the beginning of each substring. If delimiter is not found returns a null pointer
        // strtok(NULL,..) continues where the previous call left off
        // strtok() modifies the initial string to be parsed, the string is subsequently unsafe and cannot be used in its original form
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
          _serial->println();
          _serial->print(F("DataType "));
          _serial->println(DataType);
          _serial->print(F("ArrayPlace "));
          _serial->println(ArrayPlace);
          _serial->print(F("Value "));
          _serial->println(Value);
          _serial->println();
        #endif
        if (Value != NULL) {
          //: write variables ----> put data inside variables using pointers
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_bool) {
              *(bool*)ArrayValues[ArrayPlace] = (Value[0] == '1');
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_int8_t) {
              *(int8_t*)ArrayValues[ArrayPlace] = atoi(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_uint8_t) {
              *(uint8_t*)ArrayValues[ArrayPlace] = atoi(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_int16_t) {
              *(int16_t*)ArrayValues[ArrayPlace] = atoi(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_uint16_t) {
              *(uint16_t*)ArrayValues[ArrayPlace] = atoi(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_int32_t) {
              *(int32_t*)ArrayValues[ArrayPlace] = atol(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_uint32_t) {
              *(uint32_t*)ArrayValues[ArrayPlace] = atol(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_int64_t) {
			#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
				*(int64_t*)ArrayValues[ArrayPlace] = this->atoll(Value);
			#else
				*(int64_t*)ArrayValues[ArrayPlace] = strtoll(Value, NULL, 10);
			#endif
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_uint64_t) {
			#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
				*(uint64_t*)ArrayValues[ArrayPlace] = this->atoull(Value);
			#else
				*(uint64_t*)ArrayValues[ArrayPlace] = strtoull(Value, NULL, 10);
			#endif
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_float) {
              *(float*)ArrayValues[ArrayPlace] = atof(Value);
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_char) {
              *(char*)ArrayValues[ArrayPlace] = Value[0];
            }
            if (DataType == SERIALWATCHER_RX_CODE_STREAM_achar) {
              strcpy((char*)ArrayValues[ArrayPlace], Value); //the destination string should be large enough to receive the copy!
            }
        }
        else {
          #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
            _serial->println();
            _serial->print(F("Error! Value is NULL pointer"));
          #endif
        }
        #if SERIALWATCHER_RX_ADV_DIAGNOSTICS //to enable advanced diagnostics edit #define at the beginning of this file
          delay(5000); //add a big delay to let the user evaluate the printed out results 
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
bool cSerialWatcher::StringSearchSubString(char *s_where, const char *s_what) {
  if (strstr(s_where, s_what) == NULL) { 
    return false; 
  }
  else { 
    return true; 
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//return True if string s_what is found inside s_where
bool cSerialWatcher::StringSearchSubString(const __FlashStringHelper *s_where, const char *s_what) {
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
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::print_unsigned_long_long(Stream &_serial, uint64_t val) {
    if (val < 10) {
         _serial.print((uint16_t) val); //find digits and print them using 
    } else {
        uint64_t subproblem = val / 10;
        int16_t remainder = val % 10;
        print_unsigned_long_long(_serial, subproblem);
        _serial.print(remainder);
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void cSerialWatcher::print_long_long(Stream &_serial, int64_t val) {
    int64_t positive = val;
    if (val < 0) {
        _serial.print("-");
        positive *= -1;
    }
    print_unsigned_long_long(_serial, positive);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
long long cSerialWatcher::atoll(char *src) {
  bool sign_must_be_inverted = false;
  byte m = 0;
  if(src[0]=='-') {
    sign_must_be_inverted = true;
    m = 1;
  }
  long long ret = 0;
  char digit[2];
  digit[1] = '\0';
  byte n = 0;
  while(src[n+m] != '\0') {    
    digit[0] = src[n+m];
    if(n!=0) {
      ret *= 10;
    }  
    ret += atoi(digit);
    n++;
  }
  if (sign_must_be_inverted) {
    return ret*-1;
  }
  else {
    return ret;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned long long cSerialWatcher::atoull(char *src) {
  unsigned long long ret = 0;
  char digit[2];
  digit[1] = '\0';
  byte n = 0;
  while(src[n] != '\0') {    
    digit[0] = src[n];
    if(n!=0) {
      ret *= 10;
    }  
    ret += atoi(digit);
    n++;
  }
  return ret;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
