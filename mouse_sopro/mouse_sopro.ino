//----------------------bibliotecas----------------------
#include <Mouse.h>
#include <HX711_ADC.h> //https://github.com/olkal/HX711_ADC
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
#define AxisX A0
#define AxisY A1
#define led0 3
#define led1 4
#define led2 5
#define led3 6
#define led4 7
#define led5 8
#define led6 9
#define led7 10
#define led8 16
#define led9 14
const int HX711_dout = 2; //mcu > HX711 dout pin, must be external interrupt capable!
const int HX711_sck = 15; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

//----------------------Variaveis----------------------
//parametros do HX711
float calibrationValue=2500; // calibration value
unsigned long stabilizingtime = 1000; // tare preciscion can be improved by adding a few seconds of stabilizing time
boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
float dados_sensor=0;
volatile boolean newDataReady;



//parametros do joystick:
int range = 12;            // output range of X or Y movement
int threshold = range / 4;  // resting threshold
int center = range / 2;     // resting position value


//mouse
bool isLeftPressed = false;
bool isRightPressed = false;
int DELAY=0;


//LED
int ledState[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//----------------------funções----------------------
void funcMouse(float ad);
void funcLED(float ad);
void pinLEDs();
void controlLED();
int readAxis(int thisAxis); //função leitura do analogico
void dataReadyISR();

void setup() {
  Serial.begin(57600);
  Serial.println("Starting...");
  LoadCell.setSamplesInUse(1);
  LoadCell.begin(); //config
  Mouse.begin(); //inicia funcionalidade das funções de mouse

  pinLEDs();

  LoadCell.start(stabilizingtime, _tare);//calibração
    if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tempo limite, verifique a fiação MCU>HX711 e as designações dos pinos");
    while (1);
  }
  else {  
    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
    Serial.println("Startup is complete");
  }
  attachInterrupt(digitalPinToInterrupt(HX711_dout), dataReadyISR, FALLING);
}

void loop() {  
  
  if (newDataReady) {
    newDataReady = 0;
    dados_sensor=LoadCell.getData();
    Serial.print(dados_sensor);
    Serial.print("\t");
    funcMouse(dados_sensor);
    funcLED(dados_sensor);
    controlLED();
  }
  
  Mouse.move(readAxis(AxisX), readAxis(AxisY), 0);//movimento seta
  delay(10);
}

void funcMouse(float ad){  
 if (ad < -700) { //Hard sip
      if (!isRightPressed) {
        Mouse.press(MOUSE_RIGHT);
        isRightPressed = true;
        Serial.println("ok1");
      }
    } 
    else if (ad >= -700 && ad < -100 ) { //Soft sip
      DELAY++;
      if(DELAY>4){Mouse.move(0, 0, -1);Serial.println("ok2");}
    } 
    else if (ad >= -100 && ad <= 100) { //se estiver no meio, solta os dois botões
      DELAY=0;
      if (isLeftPressed) {Mouse.release(MOUSE_LEFT);isLeftPressed = false;}
      if (isRightPressed) {Mouse.release(MOUSE_RIGHT);isRightPressed = false;}
      Serial.println("ok3");
    } 
    
    else if (ad > 100 && ad <= 700) { //Soft puff
      DELAY++;
      if(DELAY>4){Mouse.move(0, 0, 1);Serial.println("ok4");}
    } 
    
    else if (ad > 700) { //Hard puff
      if (!isLeftPressed) {
        Mouse.press(MOUSE_LEFT);
        isLeftPressed = true;
        Serial.println("ok5");
      }
    }
}

void funcLED(float ad){
  if (ad<=-800){         ledState[0] = 1;ledState[1] = 0;ledState[2] = 0;ledState[3] = 0;ledState[4] = 0;ledState[5] = 0;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>-800&&ad<=-600){ledState[0] = 1;ledState[1] = 1;ledState[2] = 0;ledState[3] = 0;ledState[4] = 0;ledState[5] = 0;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>-600&&ad<=-400){ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 0;ledState[4] = 0;ledState[5] = 0;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>-200&&ad<=-50){ ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 0;ledState[5] = 0;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>-50&&ad<=50){   ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 0;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>50&&ad<=200){   ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 1;ledState[6] = 0;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>200&&ad<400){   ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 1;ledState[6] = 1;ledState[7] = 0;ledState[8] = 0;ledState[9] = 0;}
  if (ad>400&&ad<600){   ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 1;ledState[6] = 1;ledState[7] = 1;ledState[8] = 0;ledState[9] = 0;}
  if (ad>600&&ad<800){   ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 1;ledState[6] = 1;ledState[7] = 1;ledState[8] = 1;ledState[9] = 0;}
  if (ad>=800){          ledState[0] = 1;ledState[1] = 1;ledState[2] = 1;ledState[3] = 1;ledState[4] = 1;ledState[5] = 1;ledState[6] = 1;ledState[7] = 1;ledState[8] = 1;ledState[9] = 1;}
}

void pinLEDs(){
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);
}

void controlLED(){
  digitalWrite(3, ledState[0]);
  digitalWrite(4, ledState[1]);
  digitalWrite(5, ledState[2]);
  digitalWrite(6, ledState[3]);
  digitalWrite(7, ledState[4]);
  digitalWrite(8, ledState[5]);
  digitalWrite(9, ledState[6]);
  digitalWrite(10, ledState[7]);
  digitalWrite(16, ledState[8]);
  digitalWrite(14, ledState[9]);
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

//interrupt routine:
void dataReadyISR() {
  if (LoadCell.update()) {
    newDataReady = 1;
  }
}
