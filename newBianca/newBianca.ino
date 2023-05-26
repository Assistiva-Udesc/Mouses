#include <Mouse.h>


#define verde 2
#define azul 3
#define vermelho 4
#define amarelo 5
//#define right 6
//#define left 7
//#define down 8
//#define up 9
//#define potenciometro A2
#define botao_prog 10

#define AxisX A8
#define AxisY A9

#define sopro A0


// parameters for reading the joystick:
int range = 18;             // output range of X or Y movement
int threshold = range / 4;  // resting threshold
int center = range / 2;     // resting position value


int sens, timer,ad;

struct Botao {
  int estadoAtual = 1;
  int estadoPassado = 1;
  int debounce = 1;
  int opcao;
};

Botao b1, b2, b3, b4;

void estadoBotao();
void prog();
void select_opcao(int x,Botao *p);
int readAxis(int thisAxis);

void setup() {
  Serial.begin(9600);
  Mouse.begin();

  pinMode(verde, INPUT_PULLUP);
  pinMode(azul, INPUT_PULLUP);
  pinMode(vermelho, INPUT_PULLUP);
  pinMode(amarelo, INPUT_PULLUP);
//  pinMode(right, INPUT_PULLUP);
//  pinMode(left, INPUT_PULLUP);
//  pinMode(down, INPUT_PULLUP);
//  pinMode(up, INPUT_PULLUP);
  pinMode(botao_prog,INPUT_PULLUP);

  //preset dos comandos dos botoes
  b1.opcao = 4;
  b2.opcao = 1;
  b3.opcao = 3;
  b4.opcao = 2;
}

void loop() {
  estadoBotao();

  if (b1.debounce == 0) select_opcao(b1.opcao, &b1); //switch VERDE press
  if (b2.debounce == 0) select_opcao(b2.opcao, &b2); //switch AZUL press
  if (b3.debounce == 0) select_opcao(b3.opcao, &b3); //switch VERMELHO press
  if (b4.debounce == 0) select_opcao(b4.opcao, &b4); //switch AMARELO press
  
  
  if (!digitalRead(botao_prog)){  //switch de programacao press
    Serial.print("botao prog press\n");
    timer++;
    if (timer>200){timer=0; prog();} //espera um determinado tempo para q a funçao prog seja executada
  }

  
//  sens=map(analogRead(potenciometro),0,1023,1012,1023); //faz a leitura do potenciometro e ja converte para uma faixa de 1012 até 1023
//  if(sens==1023)sens=0;//quando o valor for 1023 dizemos q ele é zero para nao haver movimento do cursor
  
//  if (!digitalRead(right))Mouse.move(-sens, 0, 0); //movimento right, porta 6
//  if (!digitalRead(left))Mouse.move(sens, 0, 0); //movimento left, porta 7
//  if (!digitalRead(down))Mouse.move(0, -sens, 0); //movimento down, porta 8
//  if (!digitalRead(up))Mouse.move(0, sens, 0); //movimento up, porta 9

  int xReading = readAxis(AxisX);
  int yReading = readAxis(AxisY);
  Mouse.move(xReading, yReading, 0);

  ad=analogRead(sopro);
  Serial.print(ad);
  Serial.print("\n");
}

void prog() {
  int ok;
  b1.opcao = 0;
  b2.opcao = 0;
  b3.opcao = 0;
  b4.opcao = 0;
  for (int cont = 1; cont < 5; cont++){
    ok=1;
    do {
      estadoBotao();
      if (((b1.debounce == 0) && (b1.opcao == 0))){b1.opcao = cont; ok=0;}
      if (((b2.debounce == 0) && (b2.opcao == 0))){b2.opcao = cont; ok=0;}
      if (((b3.debounce == 0) && (b3.opcao == 0))){b3.opcao = cont; ok=0;}
      if (((b4.debounce == 0) && (b4.opcao == 0))){b4.opcao = cont; ok=0;}
    } while (ok);
  }
}


void estadoBotao() {
  b1.estadoPassado = b1.estadoAtual;
  b1.estadoAtual = digitalRead(verde);
  if (b1.estadoAtual == 0 && b1.estadoPassado == 1)b1.debounce = 0;
  else b1.debounce = 1;

  b2.estadoPassado = b2.estadoAtual;
  b2.estadoAtual = digitalRead(azul);
  if (b2.estadoAtual == 0 && b2.estadoPassado == 1)b2.debounce = 0;
  else b2.debounce = 1;

  b3.estadoPassado = b3.estadoAtual;
  b3.estadoAtual = digitalRead(vermelho);
  if (b3.estadoAtual == 0 && b3.estadoPassado == 1)b3.debounce = 0;
  else b3.debounce = 1;

  b4.estadoPassado = b4.estadoAtual;
  b4.estadoAtual = digitalRead(amarelo);
  if (b4.estadoAtual == 0 && b4.estadoPassado == 1)b4.debounce = 0;
  else b4.debounce = 1;

  delay(10);
}


int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {  //zona morta
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}


void select_opcao(int x, Botao *p){
  switch (x) {
      case 1: Mouse.click(MOUSE_LEFT); break;
      case 2: Mouse.click(MOUSE_RIGHT); break;
      case 3: Mouse.click(MOUSE_LEFT); delay(200); Mouse.click(MOUSE_LEFT); break;
      case 4: Mouse.press(MOUSE_LEFT);p->opcao=5; break;
      case 5: Mouse.release(MOUSE_LEFT);p->opcao=4; break;
      default: break;
  }
}
