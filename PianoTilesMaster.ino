#include "header.h"
//Konstatny a IO obvody
static const unsigned long l1 = 28000;
static const unsigned long l2 = 23000;
//vzdalenost mezi senzorem a prstikem, premerit
static const unsigned long s = 30500;
static const int Sen[] = {A0, A1, A2, A3};
static const int Pin[] = {2, 3, 4, 5};
static const int bttn = 12;
unsigned long curTime;

//Promenne programu
//tresholdy budou muset byt dva (funkce pres pointery)
int upTrsh = 0;
int lowTrsh = 0;

//Ulozeni pole objektu sensor pres construtor sensor::sensor(int Sen, int Pin)
sensor senArray[4]{{Sen[0],Pin[0]},{Sen[1],Pin[1]},{Sen[2],Pin[2]},{Sen[3],Pin[3]}};

void setup() {
  Serial.begin(115200);

  pinMode(bttn, INPUT_PULLUP);
  
  //Nastaveni vstupnich pinu senzoru
  for(int i = 0; i < 4; ++i){
    pinMode(Sen[i], INPUT);
    delay(10);
  }
  
  //Nastaveni vystupnich pinu solenoidu
  for(int i = 0; i < 4; ++i){
    pinMode(Pin[i], OUTPUT);
    delay(10);
    digitalWrite(Pin[i], LOW);
  }
  delay(100);
  //Kalibrace pomoci tlacitka a leveho senzoru (A0)
  calib(Sen[0],Pin[0], bttn, &upTrsh, &lowTrsh);

  Serial.print("Upper trsh: ");
  Serial.print(upTrsh); Serial.print(" ");
  Serial.print("Lower trsh: ");
  Serial.print(lowTrsh); Serial.print(" ");
  Serial.println("n");
  delay(100);
  //Startovni nastaveni
  while(digitalRead(bttn)){}
  for(int i = 0; i < 4; ++i){
    senArray[i].start(bttn, lowTrsh, upTrsh);
  }
}



void loop() {
  curTime = millis();
  
 //analogRead a Serial.print v metodach
 
  
  for(int i = 0; i < 4; ++i){
     senArray[i].edge(upTrsh, lowTrsh);
     senArray[i].vecSpeed(l1, l2, s);
     senArray[i].pushCheck(curTime);
     senArray[i].pullCheck(curTime);
     senArray[i].tisk(i);
  }
  
   

  Serial.print("Upper trsh: ");
  Serial.print(upTrsh); Serial.print(" ");
  Serial.print("Lower trsh: ");
  Serial.print(lowTrsh); Serial.print(" ");
  Serial.println("n");
    //i pro identifikaci senzoru pri debuggingu
delay(20);
}
