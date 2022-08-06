#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "U8glib.h"

RF24 radio(7, 8); // CE, CSN
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

const byte address[6] = "00001";
unsigned long tempo;
char text[32]="";

void draw() {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_unifont);
  u8g.setScale2x2();          // Scale up all draw procedures
  u8g.setPrintPos(0, 12);
  u8g.print(text);
  u8g.undoScale();
}
void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  
  
  if (radio.available()) {
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
}
