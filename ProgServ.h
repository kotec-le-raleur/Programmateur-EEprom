#ifndef __Prog_header__
#define __Prog_header__
/*
  Connections
  ARDUINO         EEPROM
  pin 51 = |WE      27
  pin 52 = |OE      22
  pin 53 = |CE      20

  22~29 (PORT A) = IO0~IO7
  37~30 (PORT C) = A0~A7
  49~45 (Part of PORT L) = A8~A14
  
*/

#define E_WE 51
#define E_OE 52
#define E_CE 53

#define _DEBUG_SERIAL_  0

#if _DEBUG_SERIAL_ == 0
  #pragma message "DEBUG is = USB"
  #define PR(x)    Serial.print(x)
  #define PX(x)    Serial.print(x,HEX)
  #define PRLN(x)  Serial.println(x)
  #define DX(x,y)  Serial.print(x);Serial.println(y,HEX)
  #define DP(x,y)  Serial.print(x);Serial.println(y)
#endif

#if _DEBUG_SERIAL_ == 1
  #pragma message "DEBUG is = TX1"
  #define PR(x)    Serial1.print(x)
  #define PX(x)    Serial1.print(x,HEX)
  #define PRLN(x)  Serial1.println(x)
  #define DX(x,y)  Serial1.print(x);Serial1.println(y,HEX)
  #define DP(x,y)  Serial1.print(x);Serial1.println(y)
#endif

#if _DEBUG_SERIAL_ == 2
  #pragma message "DEBUG is = TX2"
  #define PR(x)    Serial2.print(x)
  #define PX(x)    Serial2.print(x,HEX)
  #define PRLN(x)  Serial2.println(x)
  #define DX(x,y)  Serial2.print(x);Serial2.println(y,HEX)
  #define DP(x,y)  Serial2.print(x);Serial2.println(y)
#endif


#define LED_PIN    LED_BUILTIN       // standard sur les cartes :=PC13

#define SET_CE_LOW    digitalWrite(E_CE, LOW )
#define SET_CE_HIGH   digitalWrite(E_CE, HIGH)
#define SET_WE_LOW    digitalWrite(E_WE, LOW )
#define SET_WE_HIGH   digitalWrite(E_WE, HIGH)
#define SET_OE_LOW    digitalWrite(E_OE, LOW )
#define SET_OE_HIGH   digitalWrite(E_OE, HIGH)

#endif    ///  de __Prog_header__

/*
****************************************************************************************
*/