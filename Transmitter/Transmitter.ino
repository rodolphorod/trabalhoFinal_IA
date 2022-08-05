#include <SPI.h>
#include <nRF24L01.h>
#include "printf.h"
#include <RF24.h>

RF24 radio(8, 9); // CE, CSN

const byte address[6] = "00001";
unsigned long tempo;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  Serial.begin(115200);
  while(millis()-tempo>2000)
    tempo = millis();
}
char c[32];

void loop() {
  if(Serial.available()>0 && millis()-tempo>100){
    char c[32]="";
    Serial.readBytesUntil('\n',c,32);
    delay(50);
    Serial.print("Enviando ");
    Serial.println(c);
    delay(200);
    radio.write(&c, sizeof(c));
    while(millis()-tempo>100)
      tempo = millis();
    tempo = millis();
  }
  
}
