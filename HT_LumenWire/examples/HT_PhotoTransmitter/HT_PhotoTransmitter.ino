/* Serial laser communicator, transmitter

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

HT_PhotoTransmitter laser;

void setup(){

  laser.set_speed(25000);      // must be 500+ bits/second and less than laser slew rate
  laser.begin();

  Serial.begin(9600);

}//end setup


//timer2 interrupt toggles LIGHT_SEND_PIN at determined speed to transmit each half bit
//via the manchester modulated signal.
ISR(TIMER2_COMPA_vect){
  
  // transmit message, if any
  laser.transmit();

}


void loop(){
  char incomingByte;
  uint16_t msg;
  
  // Read text from the serial window
  incomingByte = Serial.read();
  
  // Print incoming characters
  if(incomingByte != -1){    
    Serial.print(incomingByte);

    msg = hamming_byte_encoder(incomingByte);
    
    laser.manchester_modulate(msg);

    //delay(44);
  }
}

