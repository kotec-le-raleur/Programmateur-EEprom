// contient quelques fonctions utiles 


#include <Arduino.h>
#include "ProgServ.h"


void BlinkLed( int tx);
int readLine(char * buffer, int len);
void SetPoortA_outputs() ;
void SetPoortA_inputs() ;

//===============================================================================
void BlinkLed( int tx)
{ digitalWrite(LED_PIN, HIGH);
  delay (tx);
  digitalWrite(LED_PIN, LOW);
  delay (tx);
}

//===============================================================================
// Read a line of data from the serial connection.
// read serial data until linebreak or buffer is full
//===============================================================================
int readLine(char * buffer, int len)
{
    char c = ' ';
    int ix = 0;
			do {
        if (Serial.available())
        {
            c = Serial.read();
            buffer[ix] = c; ix++;
        }
    } while ((c != '\n') && (ix < len));
  buffer[ix] = 0;
  return (ix);
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