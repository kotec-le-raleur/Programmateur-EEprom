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

#define _DEBUG_SERIAL_  1

#if _DEBUG_SERIAL_ == 0
  #warning "DEBUG is = USB"
  #define PR(x)    Serial.print(x)
  #define PX(x)    Serial.print(x,HEX)
  #define PRLN(x)  Serial.println(x)
  #define DX(x,y)  Serial.print(x);Serial.println(y,HEX)
  #define DP(x,y)  Serial.print(x);Serial.println(y)
#endif

#if _DEBUG_SERIAL_ == 1
  #warning "DEBUG is = TX1"
  #define PR(x)    Serial1.print(x)
  #define PX(x)    Serial1.print(x,HEX)
  #define PRLN(x)  Serial1.println(x)
  #define DX(x,y)  Serial1.print(x);Serial1.println(y,HEX)
  #define DP(x,y)  Serial1.print(x);Serial1.println(y)
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
le Fichier C:\Z80_DEV\Dev_GCC\Test_RH\Z80Monitor.hex est trouve : analyse en cours
cmd=!4:0:F3:C3:00:01
cmd=!3:56:C3:38:00
cmd=!3:102:C3:66:00
cmd=!24:256:31:FF:87:CD:B1:01:CD:6C:01:CD:84:01:76:0C:FF:21:0D:01:CD:52:03:C9:C3:00
cmd=!24:280:00:0D:0A:5A:4D:43:38:30:20:43:6F:6D:70:75:74:65:72:09:09:32:30:31:35:20


*/