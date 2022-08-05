

//Hiperparametros
float alpha = 0.6;
float gamma = 0.1;
float epsilon = 1; //chance de escolha aleatoria

int estado;
int *acao;
int goal = 0;
int maximo(int* vetor){
  return 0;
}

float q_table[6][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
};
int motor_table[6][4] = {
  {100,0,0,0},
  {0,0,100,0},
  {0,100,0,0},
  {0,0,0,100},
  {100,0,0,100},
  {0,100,100,0},
};

void setup() {
  // put your setup code here, to run once:
  estado = random(6);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(rand()%10<epsilon){
    //faz algo aleatorio
  }else{
    acao = maximo(q_table[estado]);
  }
  

}
