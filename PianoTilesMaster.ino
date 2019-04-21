#include "header.h"

static const unsigned long l1 = 28000;  //delka pro normalni obdelniky
static const unsigned long l2 = 23000;  //delka pro podlouhle obdelniky (cerna cast)
static const unsigned long s = 30000; //vzdalenost mezi senzorem a prstikem
static const int Sen[] = {A0, A1, A2, A3};
static const int Pin[] = {2, 3, 4, 5};
static const int cal = 11;   //signal diode
static const int bttn = 12;


//Promenne programu
int upTrsh = 0;
int lowTrsh = 0;
unsigned long curTime;
unsigned long resTime = 0;

//Ulozeni pole objektu sensor pres construtor sensor::sensor(int Sen, int Pin)
sensor senArray[4]{{Sen[0],Pin[0]},{Sen[1],Pin[1]},{Sen[2],Pin[2]},{Sen[3],Pin[3]}};

void setup() {
  Serial.begin(115200);

  pinMode(bttn, INPUT_PULLUP);
  pinMode(cal, OUTPUT);
  //Nastaveni vstupnich pinu senzoru
  for(int i = 0; i < 4; ++i){
    pinMode(Sen[i], INPUT);
    delay(100);
  }
  
  //Nastaveni vystupnich pinu solenoidu
  for(int i = 0; i < 4; ++i){
    pinMode(Pin[i], OUTPUT);
    delay(10);
    digitalWrite(Pin[i], LOW);
  }
  delay(100);
  //Kalibrace pomoci tlacitka a leveho senzoru (A0)
  calib(Sen[0], cal, bttn, &upTrsh, &lowTrsh);

  Serial.print("Upper trsh: ");
  Serial.print(upTrsh); Serial.print(" ");
  Serial.print("Lower trsh: ");
  Serial.print(lowTrsh); Serial.print(" ");
  Serial.println("n");
  delay(100);
  
  while(digitalRead(bttn)){}//Awaiting user input
}



void loop() {
   curTime = millis();

//This block serves to reset the program without the need for new calibration
   //Need to assure the program has been running for at least 500 ms before this statement
   if(!digitalRead(bttn) && (curTime - resTime) >= 500){
    digitalWrite(cal, HIGH);
    delay(500);
    for(int i = 0; i < 4; ++i){
      senArray[i].reset();
    }
    digitalWrite(cal, LOW);
    
    delay(300);
    digitalWrite(cal, HIGH);
    while(digitalRead(bttn)){}
    digitalWrite(cal, LOW);
    
    for(int i = 0; i < 4; ++i){
    senArray[i].start(lowTrsh, upTrsh);
    }
   resTime = millis();
  }
  
//This block does all the calculating, pressing and whatnot
  for(int i = 0; i < 4; ++i){
     senArray[i].edge(upTrsh, lowTrsh);
     senArray[i].vecSpeed(l1, l2, s);
     senArray[i].pushCheck(curTime);
     senArray[i].pullCheck(curTime);
     senArray[i].tisk(i);
  }

//Just to visualize thresholds in serial plotter
  Serial.print("Upper trsh: ");
  Serial.print(upTrsh); Serial.print(" ");
  Serial.print("Lower trsh: ");
  Serial.print(lowTrsh); Serial.print(" ");

  Serial.println("n");
  

}
