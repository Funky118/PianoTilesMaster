#include "header.h"
//Konstatny a IO obvody
static const unsigned long l = 28000;
//vzdalenost mezi senzorem a prstikem, premerit
static const unsigned long s = 30500;
static const char Sen[] = {A0, A1, A2, A3};
static const char Pin[] = {2, 3, 4, 5};

//Promenne programu
//tresholdy budou muset byt dva (funkce pres pointery)
int upTrsh = 0;
int lowTrsh = 0;

void setup() {
  Serial.begin(9600);
  
  //Nastaveni vstupnich pinu
  for(int i = 0; i < 4; ++i){
    pinMode(Sen[i], INPUT);
  }
  
  //Nastaveni vystupnich pinu
  for(int i = 0; i < 4; ++i){
    pinMode(Pin[i], OUTPUT);
    digitalWrite(Pin[i], LOW);
  }
  delay(100);
}
//Ulozeni pole objektu sensor pres construtor sensor::sensor(int Sen, int Pin)
sensor senArray[4]{{Sen[0],Pin[0]},{Sen[1],Pin[1]},{Sen[2],Pin[2]},{Sen[3],Pin[3]}};

void loop() {
  for(int i = 0; i < 4; ++i){
    
    //analogRead a Serial.print v metodach
    senArray[i].edge(upTrsh, lowTrsh);
    
    //i pro identifikaci senzoru pri debuggingu
    senArray[i].tisk(i);
  }
  delay(1);
}

/*
 * #Kalibrace senzoru pomoci tlacitka, naprogramovat startovaci sekvenci v headeru
 * 
 * #Nakoupit trimry na kalibraci jednotlivych senzoru
 * 
 * #Dopsat header.h
 * Dodělat edge metodu, vypocet rychlosti a casy stisku a odepnuti
 * Napsat funkci pro vypocet railů (blue = highest, black = lowest, cyan = 65%) - needs further testing
 */
