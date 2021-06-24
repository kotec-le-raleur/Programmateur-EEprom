// contient les fonctions relatives à FLASH:write in eeprom


#include <Arduino.h>
#include "ProgServ.h"

//=================  EXTERNALS   =======================
extern void SetPoortA_outputs();
extern void SetPoortA_inputs();
extern int  readLine(char * , int );
extern unsigned int Offset;
//===============================================================================

void writeValue (int adr, byte val) ;               //avec delai de 8 ms
void writeValueEx (int adr, byte val) ;
void do_Flash() ;
void do_FlashEx() ;
void do_Unprotect() ;
void do_Unprotect_2() ;

//*******************************************
//************   do_flash   *****************
//*******************************************
/*   
:180250000AFF44202D207072696E7420243830206279746573206672C4
:180268006F6D20737065636966696564206C6F636174696F6E0D0AFF47
:1802800021F601CD5203211302CD5203212B02CD5203213F02CD5203E0
:18029800215202CD52033EFFC94D656D6F72792044756D7020436F6D43
:1802B0006D616E640D0AFF4C6F636174696F6E20746F207374617274F6
*/


void do_Flash(char * cmd)    
{
  unsigned int Addr=0, Data=0  ;
  int err=0, rectype=0, nbo=0, i=0;
  unsigned long Tp1;

  Tp1 = millis();
 
  PRLN(cmd);
 
 	err = sscanf(cmd + 1, "%2X", &nbo);
	err = sscanf(cmd + 3, "%4X", &Addr);
	err = sscanf(cmd + 7, "%2X", &rectype);
 
	 if ((nbo == 0) || (rectype != 0))
	 {
     PRLN("fin de fichier");
	 	return ;
	 }
//	DX("Nb byte=",nbo);  
//  DX("Adresse=",Addr);   
  DX(" Eeprom=",Addr+Offset); 
 // DX("   Type=",rectype);
	 for (i = 0; i < nbo; i++)
	 {
	 	err = sscanf(cmd + 9 + (2 * i), "%2X", &Data);
    writeValue (Addr+i + Offset, Data) ;        // flashage effectif
	 }
   //delay(10);  // necessaire pour finir le writeValueEx
 	// sortie quand c'est fini !
   // DP("  Temps=", millis()-Tp1);
   // DP("  Ligne: ","Flash OK");
    return;
	
}
//===============================================================================

void do_FlashEx(char * cmd)    
{
  unsigned int Addr=0, Data=0  ;
  int err=0, rectype=0, nbo=0, i=0;
  unsigned long Tp1;

  Tp1 = millis();
 
  PRLN(cmd);
 
 	err = sscanf(cmd + 1, "%2X", &nbo);
	err = sscanf(cmd + 3, "%4X", &Addr);
	err = sscanf(cmd + 7, "%2X", &rectype);
 
	 if ((nbo == 0) || (rectype != 0))
	 {
     PRLN("fin de fichier");
	 	return ;
	 }
//	DX("Nb byte=",nbo);  
//  DX("Adresse=",Addr);   
  DX(" Eeprom=",Addr+Offset); 
 // DX("   Type=",rectype);
	 for (i = 0; i < nbo; i++)
	 {
	 	err = sscanf(cmd + 9 + (2 * i), "%2X", &Data);
    writeValueEx (Addr+i + Offset, Data) ;        // flashage effectif
	 }
   delay(10);  // necessaire pour finir le writeValueEx
 	// sortie quand c'est fini !
    DP("  Temps=", millis()-Tp1);
   // DP("  Ligne: ","Flash OK");
    return;
	
}
//===============================================================================





//===============================================================================

void writeValue (int adr, byte val) {
	
 // PR("writeValue-> add="); PX(adr);  DX(" data=",val);
  SET_WE_HIGH;
  SET_CE_HIGH;				//select  
  SET_OE_HIGH;    // output disable

  // now put the address and data on the bus
 
  PORTC = lowByte(adr); // lower byte of the address (first 8 bits)
  PORTL = highByte(adr); // upper byte of the addr(only 7 bits are used, the addr is a 15 bit number for 32K)
  PORTA = val;  // this is the DATA
  delayMicroseconds(50);
  SET_CE_LOW;             //  |CE goes LOW  // Chip enable
  SET_WE_LOW;             //  |WE goes LOW  // Write enable
  delayMicroseconds(1);   //   a ajuster au scope/analyseur logique
  SET_WE_HIGH;            //  |WE goes HIGH // Write disable
  SET_CE_HIGH;            //  |CE goes HIGH // Chip disable
  SET_OE_HIGH;            //   output disable
  delay(8);               // 8 mS pour laisser finir le write BYTE 
}
//===============================================================================
// version sans delay pour écriture par page
void writeValueEx (int adr, byte val) {
	
 // PR("writeValue-> add="); PX(adr);  DX(" data=",val);
  SET_WE_HIGH;
  SET_CE_HIGH;				//select  
  SET_OE_HIGH;    // output disable

  // now put the address and data on the bus
 
  PORTC = lowByte(adr); // lower byte of the address (first 8 bits)
  PORTL = highByte(adr); // upper byte of the addr(only 7 bits are used, the addr is a 15 bit number for 32K)
  PORTA = val;  // this is the DATA
  delayMicroseconds(1);
  SET_CE_LOW;             //  |CE goes LOW  // Chip enable
  SET_WE_LOW;             //  |WE goes LOW  // Write enable
  delayMicroseconds(1);   //   a ajuster au scope/analyseur logique
  SET_WE_HIGH;            //  |WE goes HIGH // Write disable
  SET_CE_HIGH;            //  |CE goes HIGH // Chip disable
  SET_OE_HIGH;            //   output disable
}


void do_Unprotect() {
// attention: test le 19 avril: le code pour les 28c64 semble OK aussi pour les 28C256 ???
  byte re= 0;
  PRLN ("in_unprotect 28C64");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;
  writeValueEx (0x1555, 0xAA);
  writeValueEx (0x0AAA, 0x55);
  writeValueEx (0x1555, 0x80);
  writeValueEx (0x1555, 0xAA);
  writeValueEx (0x0AAA, 0x55);
  writeValueEx (0x1555, 0x20);
  writeValueEx (0x0000, 0x00);
  delay(20);
 // SetPoortA_inputs();
 // re= readValue(0);
 // DX("adr 0 =",re);
  SET_CE_LOW;
}


void do_Unprotect_2() {
// attention: test le 19 avril: le code pour les 28c64 semble OK aussi pour les 28C256 ???
  byte re= 0;
  PRLN ("in_unprotect 28C256");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;

  writeValueEx (0x5555, 0xAA);
  writeValueEx (0x2AAA, 0x55);
  writeValueEx (0x5555, 0x80);
  writeValueEx (0x5555, 0xAA);
  writeValueEx (0x2AAA, 0x55);
  writeValueEx (0x5555, 0x20);
  writeValueEx (0x0000, 0x00);
 
  delay(20);
   SetPoortA_inputs();
  // re= readValue(0);
  DX("adr 0 =",re);
  SET_CE_LOW;
}

