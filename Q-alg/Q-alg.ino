#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DueFlashStorage.h>


#include "U8glib.h"

#define PIN_CE 7
#define PIN_CSN 8
#define MESQ_1 3
#define MESQ_2 4
#define MDIR_1 5
#define MDIR_2 6
#define PIN_IR 2

#define TAM_X 120
#define TAM_Y 90
#define NUM_ESTADOS 5400
#define OFFSET 0
#define NUM_ACOES 6

#define POT 50

#define x(E) E[0]
#define y(E) E[1]

DueFlashStorage dueFlashStorage;
RF24 radio(PIN_CE, PIN_CSN); // CE, CSN
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

const byte address[6] = "00001";
unsigned long tempo;
char text[32] = "";


//Hiperparametros
float alphaQ = 0.6;
float gammaQ = 100;
float epsilonQ = 0.5; //chance de escolha aleatoria

int estado, prox_estado;
int coordenada[2], prox_coordenada[2];
int goal = 0;
int acao_atual = 0;
int i;

float ganho = 10; //multiplicador da recompensa
float q_array[7] = {0};
float vetor_nulo[7] = {0};


void inicializa_q_table() {
  byte b2[sizeof(float) * 6];
  for (i = 0; i < NUM_ESTADOS; i++) {
    memcpy(b2, &q_array, 6 * sizeof(float));
    dueFlashStorage.write(i * 24, b2, 6 * sizeof(float));

    //    u8g.firstPage();
    //    do {
    //      draw(i);
    //    } while ( u8g.nextPage() );
  }
}

void le_q_table(int estado) {
  Serial.println("Lendo q_table...");
  byte *b = dueFlashStorage.readAddress(estado * 28);
  memcpy(q_array, b, sizeof(q_array));
  
  if (q_array[6]) {
    Serial.println("Primeira vez nesse endereço");
    escreve_q_table(vetor_nulo,estado);
    
    byte *b = dueFlashStorage.readAddress(estado * 28);
    memcpy(q_array, b, sizeof(q_array));
  }
  Serial.print("Lido:");
  for (int k = 0; k < 6; k++) {
    Serial.print(q_array[k]);
    Serial.print(" ");
  }
  Serial.println("");

}

void escreve_q_table(float* vetor, int estado) {
  int tam = 7*sizeof(float);
  byte b2[tam];
  memcpy(b2, vetor, tam);
  dueFlashStorage.write(estado * 28, b2, tam);
}




int motor_table[NUM_ACOES][4] = {
  {1, 0, 0, 0},
  {0, 0, 1, 0},
  {1, 0, 0, 1},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {0, 0, 0, 1}
};

void draw() {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_unifont);
  u8g.setScale2x2();          // Scale up all draw procedures
  u8g.setPrintPos(0, 12);
  u8g.print(text);
  //  u8g.print(coordenada[0]);
  //  u8g.print(" ");
  //  u8g.print(coordenada[1]);
  u8g.undoScale();
}


int le_estado(int* coordenada) {
  String s;
  for (i = 0; i < 1; i++) {
    if (radio.available()) {
      radio.read(&text, sizeof(text));
      s = String(text);
      int indexOfEspaco = s.indexOf(' ');
      //      Serial.println(indexOfEspaco);
      coordenada[0] = s.substring(0, indexOfEspaco + 1).toInt();
      coordenada[1] = s.substring(indexOfEspaco + 1).toInt();
    }
  }

  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
  int aux = TAM_X * ((coordenada[1] + TAM_Y / 2)) + (coordenada[0] + TAM_X / 2);
  //  Serial.println(aux);
  //  delay(5);

  return aux;
}

void executa(int acao) {
  int pot = 0;
  pot = POT * motor_table[acao][0];
  analogWrite(MESQ_1, constrain(pot, 0, 255));
  pot = POT * motor_table[acao][1];
  analogWrite(MESQ_2, constrain(pot, 0, 255));
  pot = POT * motor_table[acao][2];
  analogWrite(MDIR_1, constrain(pot, 0, 255));
  pot = POT * motor_table[acao][3];
  analogWrite(MDIR_2, constrain(pot, 0, 255));
  delay(500);
  analogWrite(MESQ_1, 0);
  analogWrite(MESQ_2, 0);
  analogWrite(MDIR_1, 0);
  analogWrite(MDIR_2, 0);


//  Serial.print("M>>");
//  Serial.println(motor_table[acao][0]);
//  delay(5);
}

void treina(int episodio) {


  for (i = 0; i < episodio; i++) {
    estado = le_estado(coordenada);
    Serial.print("COORD:");
    Serial.print(coordenada[0]);
    Serial.print(" ");
    Serial.println(coordenada[1]);
    if (random(100) <= gammaQ) {
      acao_atual = random(5);
    } else {
      //      Serial.println("wsuqwqal");
//      delay(5);
      le_q_table(estado);
      //      Serial.println("QCEl");
//      delay(5);
      acao_atual = maximoIndice(q_array);
      //      Serial.println("KKKKKal");
//      delay(5);
    }
    Serial.print("A>> ");
    Serial.println(acao_atual);
    executa(acao_atual);

    delay(5);

    prox_estado = le_estado(prox_coordenada);
    Serial.print("PROX:");
    Serial.print(prox_coordenada[0]);
    Serial.print(" ");
    Serial.println(prox_coordenada[1]);

    float R = recompensa(coordenada, prox_coordenada);
    Serial.print("R: ");
    Serial.println(R);

    le_q_table(estado);
    Serial.print("Q-ARRAY:");
    for (int k = 0; k < 6; k++) {
      Serial.print(q_array[k]);
      Serial.print(" ");
    }
    Serial.println("");
    float q_atual = q_array[acao_atual];

    Serial.print("q_atual: ");
    Serial.println(q_atual);
    le_q_table(prox_estado);

    Serial.println(maximo(q_array));
    q_atual += alphaQ * (R + gammaQ * maximo(q_array) - q_atual);
    q_array[acao_atual] = q_atual;


    escreve_q_table(q_array,estado);

    estado = prox_estado;
//    Serial.println("FIM DO EPISODIO ");
//    Serial.println("-----------------------");
//    Serial.println("-----------------------");
    if(gammaQ>10){
      gammaQ -= random(5);
    }else{
      gammaQ = 10;
    }
    delay(1000);
  }
}
int maximoIndice(float* qtable) {
  int j = random(5);
  int maior = 0;
  float aux = qtable[j];
  for (j = 0; j < NUM_ACOES; j++) {
    if (qtable[j] > aux) {
      maior = j;
    }
  }
  return maior;
}
float maximo(float* qtable) {
  return qtable[maximoIndice(qtable)];
}
float recompensa(int* coordenada, int* prox_coordenada) {
  float dist1 =  sqrt(pow((coordenada[0]), 2) + pow((coordenada[1]), 2));
  float dist2 = sqrt(pow((prox_coordenada[0]), 2) + pow((prox_coordenada[1]), 2));
  return ganho*(dist2 - dist1);
}
void setup() {

  pinMode(MESQ_1, OUTPUT);
  pinMode(MESQ_2, OUTPUT);
  pinMode(MDIR_1, OUTPUT);
  pinMode(MDIR_2, OUTPUT);
  pinMode(PIN_IR, INPUT);

  pinMode(13, OUTPUT);



  Serial.begin(115200);
  Serial.println("*************************************");
  Serial.println("*         COMECO DO PROGRAMA        *");
  Serial.println("*************************************");
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  //  inicializa_q_table();
  digitalWrite(13, LOW);
  delay(2000);
  digitalWrite(13, HIGH);
  //  escreve_q_table(0);
//  le_q_table(0);
    treina(100);

}
void loop() {
  
  //  //   picture loop
  //  u8g.firstPage();
  //  do {
  //    draw();
  //  } while ( u8g.nextPage() );


}
