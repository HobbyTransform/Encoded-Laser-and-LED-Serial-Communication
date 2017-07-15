/* Serial laser communicator, receiver

 The circuit:
 * laser VCC pin to pin 6, - pin to ground
 * photodiode signal on pin 7, power pins as normal

 Note:
 * timer 2 used
 * For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...
 
 Author: Andrew R. from HobbyTransform (http://hobbytransform.com/)
 Written in 2016
*/

#include <HT_hamming_encoder.h>
#include <HT_light_modulator.h>

HT_PhotoReceiver pdiode;

void setup(){

  pdiode.set_speed(25000);      // bits/sec, must match laser bit transfer rate
  pdiode.begin();

  Serial.begin(9600);

}//end setup


//timer2 interrupts LIGHT_RECEIVE_PIN at determined speed to receive each half bit
//via the manchester modulated signal.
ISR(TIMER2_COMPA_vect){
  
  //receive message, if any
  pdiode.receive();
  
}


void loop(){
  
  //print and return most recently received byte, if any, only once
  pdiode.printByte();
  
}

