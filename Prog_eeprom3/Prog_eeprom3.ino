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
#include "ProgEEprom.h"

#define _EEPROM_TYPE__ 256

void(* resetFunc) (void) = 0;//declare reset function at address 0
unsigned int Addr   = 0;
unsigned char Data = 0;
char InBuf  [128];
char OutBuf [32];
int rlen           = 0;

//*******************************************
//************    MAIN   ********************
//*******************************************

void setup() {
	Serial.begin(9600);         //  liaison avec le PC par l'USB
	Serial1.begin(115200);      //  liaison avec teraterm pour le debug
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

	SET_WE_HIGH; // Write Disabled
	SET_CE_HIGH; // Disable the chip
	SET_OE_HIGH; // Disable Output

	delay(950);
	Serial.println("OK");     // obligatoirement vers USB
  PRLN("Programme start");
	BlinkLed(500);
	
}


//
// attente d'une commande valide
// "UN"  unprotect ====> annulé
// "PR"  protect   ====> annulé
// "FL"  flash
// "RE"  read eeprom
// "Q"   quitter le programme
// après flash on attend des couples addresse, data avec adresse < 0x8000 
// si addresse > 0x8000 et data = 0xff  finde la boucle de flashage 
//*******************************************
//************    LOOP   ********************
//*******************************************
void loop() 
{
  while (Serial.available() == 0)     // attente d'une commande
      {
      rlen = Serial.readBytesUntil('\n', InBuf, 30);
      if (rlen == 0) continue;
  	
      	// if (strncmp(InBuf,"UN",2)==0)
      	// {
      	    // DP("UN","OK");
      		  // do_Unprotect() ;
            // DP("outbuf=",OutBuf);
      	// }
      	// else if (strncmp(InBuf,"PR",2)==0)
      	// {
      	    // Serial1.println("PR");
      		  // do_Protect();
            // DP("outbuf=",OutBuf);
      	// }
      	
      	if (strncmp(InBuf,"FL",2)==0)
      	{
      	    PRLN("FL");
            do_Flash();
            DP("outbuf=",OutBuf);
      	}
      	
      	else if (strncmp(InBuf,"RE",2)==0)
      	{
      	    PRLN("RE");
            do_Read_EE() ;
            DP("outbuf=",OutBuf);
		}
      	
      	else if (strncmp(InBuf,"QUIT",4)==0)
      	{
      	    PRLN("QUIT"); break;
      	}
		
      	else if (strncmp(InBuf,"boot",4)==0)
      	{
          for(int x=0;x<10;x++)  BlinkLed(200);
      	  PRLN("Reset en cours");
          SET_WE_HIGH; // Write Disabled
          SET_CE_HIGH; // Disable the chip
          SET_OE_HIGH; // Disable Output
          digitalWrite(LED_PIN, HIGH);
          delay(1000);
          resetFunc(); //call reset 
          break;
      	}
      	
      	else 
      	{
      	    PRLN("commande ???");
      	    BlinkLed(500);
      	}	
  	}    /// end of while() 
}       /// end of loop()


//   line=!24:1000:00:0D:0A:5A:4D:43:38:30:20:43:6F:6D:70:75:74:65:72:09:09:32:30:31:35:20
//   !4:0:F3:C3:00:01
//*******************************************
//************   do_flash   *****************
//*******************************************
void do_Flash()    
{
  char * pc = NULL;
  unsigned int Addr=0, Data=0 , nbo=0, szline=0, ix=0;
  char Line[128];
  char * p = NULL;
  unsigned long Tp1;
  
  while (Serial.available() == 0)     // attente d'une ligne formatée pour flasher
    {
      memset(Line,0,sizeof(Line));
      szline = Serial.readBytesUntil('\n', Line, 100);
      if (szline == 0)    continue;
      if (Line[0] == 'q') break;
      if (Line[0] != '!') continue;
      

      Tp1 = millis();

      DP("line=",Line);
      p = Line;
      // on extrait nbo, addr et data de la chaine reçue	  
      p++;   // pour echapper le "!" au debut
      
      pc = strtok (p,":");
      nbo =atoi(pc);
      pc = strtok(NULL,":");
      Addr = strtol(pc,NULL,16); 
      Addr = Addr & 0xFFFF;       
      DP("nbo    =",nbo);
      DX("address=0x",Addr);
      if (Addr >= 0x8000)	{	break;}
      SetPoortA_outputs();
      for(ix=0;ix<nbo;ix++)
      {
        pc = strtok (NULL, ":");
        Data = (byte)strtol(pc,NULL,16); 
      //	DX("data=0x",Data);
        writeValue (Addr+ix, Data) ;        // flashage effectif
      }
	    Serial.write('$');  //   acquitement = $	
      DP("temps=", millis()-Tp1);
	}
	
	strcpy(OutBuf,"Fin processus FL ");
    DP("Fin de process ","Flash OK");
	
}
//**************************READ XXX octets à l'adresse add********************
void do_Read_EE()    
{
  char * pc = NULL;
  unsigned int Addr=0, Data=0 , nbo=0, szline=0, ix=0, iy=0;
  char Line[128];
  while (Serial.available() == 0)     // attente d'une ligne formatée pour lire 
    {
      memset(Line,0,sizeof(Line));
      szline = Serial.readBytesUntil('\n', Line, 100);
      if (szline == 0)    continue;
      if (Line[0] == 'q') break;

    
      DP("line=",Line);
              // on extrait nbo, addr  de la chaine reçue	  
      
      pc = strtok (Line,":");
      nbo =atoi(pc);
      pc = strtok(NULL,":");
      Addr = strtol(pc,NULL,16); 
      Addr = Addr & 0xFF00;       // on commence en début d'une page 
      DP("nbo     =",nbo);
      DX("address = 0x",Addr);

      SetPoortA_inputs();

      if (Addr >= 0x8000)	{	break;}
      for (ix=0; ix<nbo ;ix+=16)
      {
        PR("Addr="); PX(Addr+ix);
        for (iy=0; iy<16; iy++)
        {
          Data = readValue(Addr + ix +iy);
         PR(" : "); PX(Data);
        }
        PRLN(" ");
      }
    }
    PR("Fin de read");
}

//===============================================================================
byte readValue (int adr) {
 // DX("read adr=",adr);
	
  SET_WE_HIGH; // disable ALL
  SET_OE_HIGH;
  SET_CE_HIGH;

  PORTC = lowByte(adr); // lower byte of the address (first 8 bits)
  PORTL = highByte(adr); // upper byte of the address (only 5 bits are used, the address is a 13 bit number)
  delayMicroseconds(2);
  //PORTA = 0;
  byte r = 0;
  //delayMicroseconds(2);
  SET_OE_LOW;
  SET_CE_LOW;
  delayMicroseconds(2);
  // read the data
  r = PINA;

  //delayMicroseconds(1);
  SET_CE_HIGH;
  SET_OE_HIGH;
  return r;
}
//===============================================================================

void SetPoortA_outputs() {
  DDRA = 0xFF;  // set portA as outputs
  delay(10);    // we need a delay to switch from input to output (at least 500 microseconds)
}
//===============================================================================

void SetPoortA_inputs() {
  PORTA = 0;
  DDRA = 0x00; // set portA as Inputs
  delay(10); // we need a little delay to switch from input to output (at least 500 microseconds)
}
//===============================================================================

void writeValue (int adr, byte val) {
	
 // PR("writeValue-> add="); PX(adr);  DX(" data=",val);
  SET_OE_HIGH;    // output disable
  SET_CE_HIGH;				//select  
  SET_WE_HIGH;

  // now put the address and data on the bus
 
  PORTC = lowByte(adr); // lower byte of the address (first 8 bits)
  PORTL = highByte(adr); // upper byte of the addr(only 7 bits are used, the addr is a 15 bit number for 32K)
  delayMicroseconds(50);
  SET_CE_LOW;             //  |CE goes LOW  // Chip enable
  SET_WE_LOW;             //  |WE goes LOW  // Write enable
  PORTA = val;  // this is the DATA
  delayMicroseconds(1);   //   a ajuster au scope/analyseur logique
  SET_WE_HIGH;            //  |WE goes HIGH // Write disable
  SET_CE_HIGH;            //  |CE goes HIGH // Chip disable
  SET_OE_LOW;            //   output disable
  delay(8);              // 8 mS pour laisser finir le write BYTE 
}
//===============================================================================
// version sans delay pour écriture par page
void writeValueEx (int adr, byte val) {
	
 // PR("writeValue-> add="); PX(adr);  DX(" data=",val);
  SET_OE_HIGH;    // output disable
  SET_CE_HIGH;				//select  
  SET_WE_HIGH;

  // now put the address and data on the bus
 
  PORTC = lowByte(adr); // lower byte of the address (first 8 bits)
  PORTL = highByte(adr); // upper byte of the addr(only 7 bits are used, the addr is a 15 bit number for 32K)
  delayMicroseconds(50);
  SET_CE_LOW;             //  |CE goes LOW  // Chip enable
  SET_WE_LOW;             //  |WE goes LOW  // Write enable
  PORTA = val;  // this is the DATA
  delayMicroseconds(1);   //   a ajuster au scope/analyseur logique
  SET_WE_HIGH;            //  |WE goes HIGH // Write disable
  SET_CE_HIGH;            //  |CE goes HIGH // Chip disable
  SET_OE_LOW;            //   output disable
}

//===============================================================================
void BlinkLed( int tx)
{ digitalWrite(LED_PIN, HIGH);
  delay (tx);
  digitalWrite(LED_PIN, LOW);
  delay (tx);
}

/*
a tester
// Read a line of data from the serial connection.
char * readLine(char * buffer, int len)
{
    for (int ix = 0; (ix < len); ix++)
    {
         buffer[ix] = 0;
    }

    // read serial data until linebreak or buffer is full
    char c = ' ';
    int ix = 0;
    do {
        if (Serial.available())
        {
            c = Serial.read();
            if ((c == '\b') && (ix > 0))
            {
                // Backspace, forget last character
                --ix;
            }
            buffer[ix++] = c;
            Serial.write(c);
        }
    } while ((c != '\n') && (ix < len));

    buffer[ix - 1] = 0;
    return buffer;
}









//===============================================================================
#if _EEPROM_TYPE__ == 64
#warning  "attention eeprom 64k  8 KB"
void do_Protect() {
  PRLN ("in_do_protect 28C64");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;
  writeValue (0x1555, 0xAA);
  writeValue (0x0AAA, 0x55);
  writeValue (0x1555, 0xA0);
  SET_CE_LOW;

  strcpy(OutBuf,"PR_OK");
}
//===============================================================================

void unprotect() {
  PRLN ("in_unprotect 28C64");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;
  writeValue (0x1555, 0xAA);
  writeValue (0x0AAA, 0x55);
  writeValue (0x1555, 0x80);

  writeValue (0x1555, 0xAA);
  writeValue (0x0AAA, 0x55);
  writeValue (0x1555, 0x20);
  writeValue (0x0000, 0x00);
  SET_CE_LOW;
}
#endif

//===============================================================================
#if _EEPROM_TYPE__ == 256
#warning  "attention eeprom 256k  32 KB"
void do_Protect() {
  PRLN ("in_do_protect 28C256");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;
  writeValue (0x5555, 0xAA);
  writeValue (0x2AAA, 0x55);
  writeValue (0x5555, 0xA0);
  SET_CE_LOW;

  strcpy(OutBuf,"PR_OK");
}
//===============================================================================

void unprotect() {
  PRLN ("in_unprotect 28C256");
  SetPoortA_outputs();
  SET_OE_HIGH;
  SET_CE_HIGH;
  writeValue (0x5555, 0xAA);
  writeValue (0x2AAA, 0x55);
  writeValue (0x5555, 0x80);

  writeValue (0x5555, 0xAA);
  writeValue (0x2AAA, 0x55);
  writeValue (0x5555, 0x20);
  writeValue (0x0000, 0x00);
  SET_CE_LOW;
}
#endif

*/


