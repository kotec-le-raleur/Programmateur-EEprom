/*
  mon programmateur d'eeprom 
  15 avril 2021

 
  Connections
  ARDUINO         EEPROM
  pin 51 = |WE      27
  pin 52 = |OE      22
  pin 53 = |CE      20

  22~29 (PORT A) = IO0~IO7
  37~30 (PORT C) = A0~A7
  49~45 (Part of PORT L) = A8~A12
  see http://archive.monome.org/community/uploads/2012/02/Mega%20Port%20Diagram.png
*/

#include <Arduino.h>
#include "ProgServ.h"

#define _EEPROM_TYPE__ 256
#define  _VERSION_  "* Server V 2.7  24 juin 2021 *"  /* ajout lecture 27C128 Uvprom  */

//==========================   PROTOTYPES  ====================================
extern void BlinkLed(int);
extern int readLine (char * , int );
extern void do_Flash(char *) ;
extern void do_FlashEx(char *) ;
extern void do_Read (char *) ;
extern void do_Dump (void);
extern void do_Unprotect(void);
extern void do_Unprotect_2(void);
extern void do_Fill (char *) ;   
extern void SetPoortA_outputs();
extern unsigned int Pcourant ;

void(* resetFunc) (void) = 0;//declare reset function at address 0


//==========================   GLOBALES    ====================================
unsigned int Addr  = 0,  Offset = 0;
unsigned char Data = 0;
char InBuf  [300];
char OutBuf [32];
int rlen           = 0;
bool  Mode_uvprom = false; 
#define BAUDRATE   57600
//*******************************************
//************    MAIN   ********************
//*******************************************

void setup() {
	Serial.begin(BAUDRATE);         //  liaison avec le PC par l'USB

#if _DEBUG_SERIAL_ == 1
	Serial1.begin(BAUDRATE);      //  liaison avec teraterm pour le debug
#endif

#if _DEBUG_SERIAL_ == 2
	Serial2.begin(BAUDRATE);      //  liaison avec teraterm pour le debug
#endif

	pinMode(LED_PIN, OUTPUT);   // LED
	digitalWrite(LED_PIN, LOW);
	pinMode(E_WE, OUTPUT);
	pinMode(E_OE, OUTPUT);
	pinMode(E_CE, OUTPUT);
	DDRA = 0xFF; // set portA as outputs
	DDRC = 0xFF; // set portC as outputs
	DDRL = 0xFF; // set portL as outputs
	PORTA = 0;
	PORTC = 0;
	PORTL = 0;
  Serial.setTimeout(1000);   // 10 secondes de timeout

	SET_WE_HIGH; // Write Disabled
	SET_CE_HIGH; // Disable the chip
	SET_OE_HIGH; // Disable Output
	BlinkLed(100);
  PRLN("*****************************");
  PRLN(_VERSION_); 
  PRLN("*****************************");
  PRLN("*     Programme start       *");
  PRLN("*****************************");
 
 // while (Serial.available() == 0)     // attente de la connexion
      // {
      // rlen = readLine(InBuf, 2);
      // if (InBuf[0] == '$') break;  
      // }

	Serial.println("OK");     // obligatoirement vers USB
  DX("Offset=",Offset);
  DX("UVProm=",Mode_uvprom);
  DP("BaudRate=",BAUDRATE);

}

//
// attente d'une commande valide
// "H imprime l'usage
// "Rnb:addr"  read nb page de 256 octets eeprom
// "Fxx:addr1:addr2"  Fill avec xx entre addr1 et addr2
// "Oxxxx" ajoute un offset pour l'écriture
// "Q"   quitter le programme
// "B"   reboot
//*******************************************
//************    LOOP   ********************
//*******************************************
void loop() 
{
  unsigned char  c_in=0;
  while (Serial.available() == 0)     // attente d'une commande
      {
    //  rlen = Serial.readBytesUntil('\n', InBuf, 30);
      rlen = readLine(InBuf, 299);
      //if (rlen == 0) {PRLN("*");continue; } // echappement du time out
//  	    PRLN(InBuf)  ;
        c_in = InBuf[0] ;
        switch (c_in)
        {
          case 'R':
          case 'r':
      	  PRLN("commande Read");
          do_Read(InBuf);
          PRLN("Retour do_Read");
          break;
//==============================
          case 'D':
          case 'd':
      	  //PRLN("commande Dump");
          do_Dump();
          //PRLN("Retour do_Dump");
          break;
//==============================
           case 'S':
           case 's':
           Pcourant = Pcourant - 0x200;
           do_Dump();
           break;
//==============================
//          case 'W':
//          case 'w':
//            SetPoortA_outputs();
//            PRLN("commande Flash");
//            do_Flash(InBuf) ;
//            PRLN("Retour do_Flash");
//          break;
//==============================
          case ':':
            SetPoortA_outputs();
//            PRLN("Flash");
//            do_Flash(InBuf) ;
            do_FlashEx(InBuf) ;
//            SetPoortA_inputs();
          //  PRLN("Retour do_Flash");
          break;
//==============================
          case 'F':
          case 'f':
            PRLN("commande Fill");
            do_Fill(InBuf) ;
            PRLN("Retour do_Fill");
          break;
//==============================
          case 'Q':
          case 'q':
            PRLN("commande Quit");
            SET_WE_HIGH; // Write Disabled
            SET_CE_HIGH; // Disable the chip
            SET_OE_HIGH; // Disable Output
          break;
//==============================
          case 'B':
          case 'b':
            PRLN("Boot recu ! Reset en cours");
            for(int x=0;x<5;x++)  BlinkLed(200); 
            SET_WE_HIGH; // Write Disabled
            SET_CE_HIGH; // Disable the chip
            SET_OE_HIGH; // Disable Output
            digitalWrite(LED_PIN, HIGH);
            resetFunc(); //call reset 
          break;
//==============================
          case 'U':
          case 'u':
            PRLN("ATTENTION: Unprotect eeprom sequence 28C64");
            do_Unprotect();
          break;
//==============================
//==============================
          case 'P':
          case 'p':
            PRLN("ATTENTION: Unprotect eeprom sequence 28C256");
            do_Unprotect_2();
          break;
//==============================
// "H imprime l'usage
// "Rnb:addr"  read nb page de 256 octets eeprom
// "Fxx:addr1:addr2"  Fill avec xx entre addr1 et addr2
// "Oxxxx" ajoute un offset pour l'écriture
// "Q"   quitter le programme
// "B"   reboot

          case 'H':
          case 'h':
          PRLN("---------------------------------------------------");
          PRLN("Usage : adresses en hexa ");
          PRLN("R nb:adresse       imprime nb pages à partir de adresse ");
          PRLN("D  ( ou S  )       dump de la page courante ");
          PRLN("F debut:fin:valeur ecrit valeur entre l'adresse de debut et de fin  ");
          PRLN("O valeur           ajoute un index avant de flasher ");
          PRLN("U                  Sequence UnProtect EEprom 64");
          PRLN("P                  Sequence UnProtect EEprom 128");
          PRLN("B                  reboot l'arduino");
          PRLN("Q                  quitte le programme");
          PRLN("7                  mode UVprom (non actif)");
          PRLN("--------------------------------------------------- ");
          PRLN(" ");




          break;
//==============================
          case 'O':
          case 'o':
          Offset =strtol(InBuf+1,NULL,16);    // offset 
          Offset= Offset & 0xFFFF ;
          DX("Offset=",Offset);
          break;
//==============================
          case '7':
          //case 'u':
          Mode_uvprom = true;
          PRLN("Mode UVprom actif");
          break;
//==============================
          default :
      	    PRLN("commande ???");
      	    BlinkLed(10);
          break;

        }
  	}    /// end of while() 
}       /// end of loop()




