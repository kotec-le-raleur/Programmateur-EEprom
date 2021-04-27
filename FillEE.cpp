// contient les fonctions relatives à FILL in eeprom


#include <Arduino.h>
#include "ProgServ.h"

//=================  EXTERNALS   =======================
extern void SetPoortA_outputs();
extern void SetPoortA_inputs();
extern int  readLine(char * , int );
extern void writeValue (int adr, byte val) ;               //avec delai de 8 ms
extern void writeValueEx (int adr, byte val) ;          //sans delai de 8 ms

//===============================================================================

//*******************************************
//************   do_fill   *****************
//*******************************************
//   F1000:1200:45
void do_Fill(char * cmd)    
{
  char * pc = NULL;
  unsigned int Addr1=0, Data=0 , Addr2=0;
  char Bufout[100];
  char * p = NULL;
  unsigned long Tp1;
  
 PRLN("Fill memory");     // attente d'une ligne formatée pour flasher
 {
     strcpy(Bufout,cmd);
      Tp1 = millis();
      p = cmd;
      // on extrait  addr1, addr2 et data de la chaine reçue	
      p++;   // pour echapper le F   
      
      pc = strtok (p,":");
      Addr1 = strtol(pc,NULL,16); // adresse de début
      pc = strtok(NULL,":");
      Addr2 = strtol(pc,NULL,16); 
      pc = strtok(NULL,":");
      Data = strtol(pc,NULL,16); 
      Data = Data & 0xFF;


      PR("Do_F:Line="); PR(Bufout);  
      sprintf(Bufout,"start=0x%04X   end=0x%04X  data=0x%02X",Addr1, Addr2, Data)  ; PRLN(Bufout);  
      SetPoortA_outputs();
      while( Addr2 > Addr1)
      {for (int x=0;x<32;x++)
        {
          writeValueEx (Addr1, Data) ;        // flashage effectif par block de 32
          if ((Addr1 & 0x00FF) == 0){ DX("add=",Addr1);}
          Addr1++;
        }
        delay(10);
      }
      DP("temps=", millis()-Tp1);
	}
	// sortie quand c'est fini !
    DP("Fin de process ","Filling OK");
    return;
	
}