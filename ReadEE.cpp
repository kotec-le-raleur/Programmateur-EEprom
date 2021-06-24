// contient les fonctions relatives à READ

#include <Arduino.h>
#include "ProgServ.h"

//=================  EXTERNALS   =======================
extern void SetPoortA_outputs();
extern void SetPoortA_inputs();
extern int  readLine(char * , int );
extern bool  Mode_uvprom ; 


byte readValue (int adr);
void do_Read() ;
void do_Dump();
unsigned int Pcourant       = 0;

//**************************READ XXX octets à l'adresse add********************
void do_Read(char *cmd)    
{
    char * pc = NULL;
    unsigned int Addr=0, Data=0 , nbp=0, ix=0, iy=0, iz=0;
 //   char Line[32];
    char Bufout[128];
   
   // memset(Line,0,sizeof(Line));
   // szline =  readLine(Line, sizeof(Line) - 1);

    DP("line=",cmd);
  //  pc = strtok (&cmd[1],":");  // on échappe le "R"
  //  nbp =strtol(pc,NULL,10);    // nombre de page pour le dump 
  //  if (nbp == 0) nbp++;        // au moins une page
  //  pc = strtok(NULL,":");
  //  Addr = strtol(pc,NULL,16); 
    Addr = strtol(&cmd[1],NULL,16); 
    Addr = Addr & 0xFF00;       // on commence en début d'une page 
    Pcourant = Addr; // + nbp * 0x100;

  //  sprintf(Bufout,"nb page=%d nbp Adresse =0x%04X   ",nbp, Addr); PRLN(Bufout); 
    SetPoortA_inputs();
    
    for (iz=0;iz<1;iz++)  // 1 seule page
    {
        for (ix=0; ix<16 ;ix++)       //page
        {
            sprintf(Bufout," %04X   ",Addr + (iz*256) + (ix*16)); PR(Bufout);   
            for (iy=0; iy<16; iy++)         // ligne
                {
                Data = readValue(Addr + (iz*256) + (ix*16) + iy);
                sprintf(Bufout,"%02X ",Data); PR(Bufout);
                }
            PRLN(" ");
        }
        PRLN(" ");
    }
}


//===============================================================================
//**************************READ XXX octets à l'adresse add********************
void do_Dump(void)    
{
    char * pc = NULL;
    unsigned int Addr=0, Data=0 , nbp=0, ix=0, iy=0, iz=0;
 //   char Line[32];
    char Bufout[128];
   
   // memset(Line,0,sizeof(Line));
   // szline =  readLine(Line, sizeof(Line) - 1);

    nbp = 1;
    Addr = Pcourant;
    Pcourant = Addr +  0x100;  // pour le coup d'apres 

    sprintf(Bufout,"1 page= Adresse =0x%04X   ", Addr); PRLN(Bufout); 
    SetPoortA_inputs();
    for (iz=0;iz<nbp;iz++)  // numero de page
    {
        for (ix=0; ix<16 ;ix++)       //page
        {
            sprintf(Bufout," %04X   ",Addr + (iz*256) + (ix*16)); PR(Bufout);   
            for (iy=0; iy<16; iy++)         // ligne
                {
                Data = readValue(Addr + (iz*256) + (ix*16) + iy);
                sprintf(Bufout,"%02X ",Data); PR(Bufout);
                }
            PRLN(" ");
        }
        PRLN(" ");
    }
}


//===============================================================================
byte readValue (int adr) {
 // DX("read adr=",adr);
	
  SET_WE_HIGH; // disable ALL
  SET_OE_HIGH;
  SET_CE_HIGH;
  PORTC = lowByte (adr);  // lower byte of the address (first 8 bits)
  PORTL = highByte(adr);  // upper byte of the address (only 5 bits are used, the address is a 13 bit number)
  delayMicroseconds(2);
  byte r = 0;
  SET_OE_LOW;
  SET_CE_LOW;
  delayMicroseconds(2);
  // read the data
  r = PINA;
  SET_CE_HIGH;
  SET_OE_HIGH;
  return r;
}