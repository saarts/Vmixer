#include <avr/interrupt.h>
#define correctionmicros 1500
#define refreshinterval 17900

//uint16_t refreshinterval = 16000;
uint16_t newmicro = 0;
uint16_t pin1highmicro = 0;
uint16_t inputpulse1 = 0;
uint16_t oldinputpulse1 = 0;
uint16_t pin2highmicro = 0;
uint16_t inputpulse2 = 0;
uint16_t oldinputpulse2 = 0;
uint8_t last1 =0;
uint8_t last2 =0;
uint16_t right = 1500;
uint16_t left = 1500;
uint16_t currentframe= 0;
uint16_t refresh;
uint16_t oldrefresh;
uint8_t conlost = 0;




void setup() {

//Serial.begin(9600);


DDRD = DDRD | 0b10100000;
DDRB |= 0b00000000;

cli();
PCICR |= 0b00000001;    // turn on port b
PCMSK0 |= 0b00000011;    // turn on pin PB0, which is PCINT0, physical pin 14
sei();
}

void loop() {

  if(inputpulse1 >2100 || inputpulse2 >2100 || inputpulse1 <900 || inputpulse2 <900){
    conlost =1;
    left = 1500;
    right = 1500;
     servorefresh();
    }else{
    conlost = 0;  
      }
if(conlost !=1){
uint16_t treshold =  inputpulse1 - oldinputpulse1;
  if(treshold > 6){
    calculate();
    oldinputpulse1 = inputpulse1;
  }

    treshold =  inputpulse2 - oldinputpulse2;
    if(treshold > 6){
    calculate();
    oldinputpulse2 = inputpulse2;
  }
  
  servorefresh();
}

}

void calculate(){
  right=inputpulse1+inputpulse2-1500;
  left=inputpulse1-inputpulse2+1500;
  }
  


void servorefresh(){
  
  refresh = micros();
 // uint8_t oldSREG = SREG;
  //cli();
  currentframe=refresh-oldrefresh;
 // SREG = oldSREG;   

   if(PIND & B10000000){
   if(currentframe >right){
     PORTD &= ~B10000000;
     }
   }
   
   if(PIND & B00100000){
   if(currentframe >left){
     PORTD &= ~B00100000;
     }
   }

  if(currentframe > refreshinterval){
    PORTD |= B10100000;
    oldrefresh=refresh;
    }

}



ISR(PCINT0_vect){
  newmicro = micros();  
   if(last1<1 && PINB & B00000001){
      pin1highmicro = newmicro;
      last1+= 1;
      }else if(last1 >0 && !(PINB & B00000001)){
      inputpulse1 = newmicro-pin1highmicro;
      last1 = 0;
        }

      if(last2<1 && PINB & B00000010){
      pin2highmicro = newmicro;
      last2 += 1;
      }else if(last2 >0 && !(PINB & B00000010)){
      inputpulse2 = newmicro-pin2highmicro;
      last2 = 0;
        } 
  }
  
