//vektor je vnoreny do sensor
//slouzi k ukladani casu
class vector {
public:
	unsigned long edgeUp; //time at which it goes from blue to black
	unsigned long edgeDown; //time at which it goes from black to blue (or cyan to blue)
	unsigned long pushTime;
	unsigned long pullTime;

	vector(){}
	//speed calculation, prevState is gonna be "c" for long stripes and "a" for normal ones
	void vecSpeed1(int l1, int s);
	void vecSpeed2(int l2, int s);
	
};

class sensor {
	int pinIn;

	int value;
	//creates a "debouncer" for the long stipes that have to be held down
	int prevValue;
	unsigned long prevTime;
	//prevState is gonna be "c" for shorter rectangles and "a" for longer ones
	char prevState; //a = blue, b = black, c = cyan

	

	//index pole
	int i;

public:
	sensor() {}
	//Vsimnete si, ze prevState je nastaven an "a", to je dulezite pro metodu sensor::start
	sensor(int in, int out) : pinIn(in), pinOut(out) {prevState = 'a';}
	int pinOut;
	//vsechny elementy jsou nastaveny na 0
	vector pole[5];
	//Detekce hran, ukladani casu a posouvani pole
	void edge(int upTrsh, int lowTrsh);
	void start(int bttn, int upTrsh, int lowTrsh);
	//Pouze pro debugging
	void tisk(int i);
	void vecSpeed(int i, int l1, int l2, int s);
	
};



//METODY VECTOR
//dodelat (TBD) do mainu if(edgeDown > edgeUp)...
void vector::vecSpeed1(int l1, int s) {
	unsigned long v = 0;
	v = edgeDown - edgeUp; //delta t
	v = l1 / v;
	Serial.print("Rychlost: ");
	Serial.println(v);
	//saving the time of push and pull for normal rectangles
	pushTime = edgeUp + (s / v);
	pullTime = edgeDown + (s / v);
	edgeDown = 0;
	edgeUp = 0;
}
void vector::vecSpeed2(int l2, int s) {
	unsigned long v = 0;
	v = edgeDown - edgeUp; //delta t
	v = l2 / v;

	//saving the time of push and pull for normal rectangles
	pushTime = edgeUp + (s / v);
	pullTime = NULL;
	edgeDown = 0;
	edgeUp = 0;
}


//METODY SENSOR
/*
//Snimani stavu
V main loop for cyklus hlida stavy jasu na senzorech, kazdy jednotlivy objekt senzoru
pak zapisuji vlastni casy hran, pocita rychlost a zapisuje cas push/pull do pole vnoreneho objektu vektor
na konci main loop se porovnavaji pull/push casy s realnym casem
Vyjimkou je stav c (cyan) pro kratsi obdelniky (ktere se musi drzet) v tomto pripade zajistuje pull up
primo tato metoda (sensor::edge)
*/
void sensor::edge(int upTrsh, int lowTrsh) {
	//prevStates: a = blue, b = black, c = cyan

	value = analogRead(pinIn);
	if (i >= 5)
		i = 0;

	if (value <= lowTrsh && prevState == 'a') {
		//from blue to black
		prevState = 'b';
		pole[i].edgeUp = millis();
	}
	else if (value > lowTrsh && value < upTrsh && prevState == 'b' && value == prevValue) {
		//from black to cyan
		if (millis() - prevTime >= 2) {
			prevState = 'c';
			pole[i].edgeDown = millis();

			//inkrementuji pole vektoru
			++i;
		}
		return;
	}
	else if (value >= upTrsh && prevState == 'b') {
		//from black to blue
		prevState = 'a';
		pole[i].edgeDown = millis();

		//inkrementuji pole vektoru
		++i;
	}
	else if (value >= upTrsh && prevState == 'c') {
		//from cyan to blue
		prevState = 'a';
		digitalWrite(pinOut, LOW);
	}

	/*Promene pro "debouncer" pro useky kde se to musi drzet
	Protoze arduino AD prevodnik meri hodne rychle, tak v podstate merime senzorem spojite!
	Hodnota senzoru proto vzdycky projde hranici vyhrazenou pro dlouhe useky, ktere se musi drzet,
	musime se ujistit, ze se hodnota na senzoru dale nemeni a opravdu se jedna o usek 'c'
	*/
	prevValue = value;
	prevTime = millis();
	}
/*
//startovaci sekvence
Arduino main loop najde na ktere ctvrtine je obdelnik a nastavi na ni prevState = b, jako black
potom startseq proklika kazdou ctvrtinu hraci plochy, protoze nemuze vedet kde je
start policko
*/
void sensor::start(int bttn, int lowTrsh, int upTrsh) {
	value = analogRead(pinIn);

	if (value <= lowTrsh){
		while (digitalRead(bttn)) {}
		prevState = "b";
		pole[0].edgeDown = millis();
	}

}

void sensor::tisk(int i) {
	Serial.print("Senzor ");
	Serial.print(i);
	Serial.print(" : ");
	Serial.println(value);
	Serial.println("|||||||||||||||||||||||||||");
	for (int j = 0; j < 5; ++j) {
		Serial.print("Edge up time: ");
		Serial.println(pole[j].edgeUp);

		Serial.print("Edge down time: ");
		Serial.println(pole[j].edgeDown);

		Serial.print("Push time: ");
		Serial.println(pole[j].pushTime);

		Serial.print("Pull time: ");
		Serial.println(pole[j].pullTime);
	}
	Serial.println("|||||||||||||||||||||||||||");
}

void sensor::vecSpeed(int i, int l1, int l2, int s) {
	if (pole[i].edgeDown > pole[i].edgeUp){

		if (prevState == 'a') {
			pole[i].vecSpeed1(l1, s);
		}
		else if (prevState == 'c') {
			pole[i].vecSpeed2(l2, s);
		}
	
	}
}



/*
Kalibrace senzoru pomoci tlacitka, nejdrive snima cernou, potom modrou pro vyvoreni intervalu <0,100>%
svetle modra je asi 65% od cerne, tzn. nastavi se dve hranice pro vytvoreni trech stavu
Tlacitko je pripojene na D12 pres pull up
Pro zjednoduseni je poradi barev dulezite! Nejdriv cerna potom modra.
*/

void calib(int Sen, int Pin, int bttn, int *upTrsh, int *lowTrsh) {
	
	int tmp = 0;
	digitalWrite(Pin, HIGH);
	while (digitalRead(bttn)) {}
	digitalWrite(Pin, LOW);
	*lowTrsh = analogRead(Sen);
	delay(500);

	digitalWrite(Pin, HIGH);
	while (digitalRead(bttn)) {}
	digitalWrite(Pin, LOW);
	*upTrsh = analogRead(Sen);
	delay(500);

	tmp = (*lowTrsh + *upTrsh) / 10;
	Serial.println(*lowTrsh);
	Serial.println(*upTrsh);
	Serial.println(tmp);
	*lowTrsh += 1 * tmp;
	*upTrsh -= 3 * tmp;
}
//Proklikani vsech policek SCRAPPED
/*void startseq(char *Pin) {
	for (int i = 0; i < 4; ++i) {
		digitalWrite(*(Pin + i), HIGH);
		delay(10);
		digitalWrite(*(Pin + i), LOW);
	}
}
*/
/*


Throw a loop into main to go through all four sensors and all of their timings.
*/
