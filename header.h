//vektor je vnoreny do sensor
//slouzi k ukladani casu
class vector {
public:
	unsigned long edgeUp; //time at which it goes from blue to black
	unsigned long edgeDown; //time at which it goes from black to blue (or cyan to blue)
	unsigned long pushTime;
	unsigned long pullTime;
	unsigned long speedV;//this is only an auxiliary variable to account for the long stripes (could've been handled better)
						//otherwise it's always 0 so it's just taking up space (maybe optimize this with mem alloc)
	vector(){}
	//speed calculation, prevState is gonna be "c" for long stripes and "a" for normal ones
	void vecSpeed1(int l1, int s);
	void vecSpeed2(int l2, int s);
	
};

//Add const to constant vars
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
	int pinOut;

	sensor() {}
	//Vsimnete si, ze prevState je nastaven an "a", to je dulezite pro metodu sensor::start
	sensor(int in, int out) : pinIn(in), pinOut(out) {prevState = 'a';}

	//vsechny elementy jsou nastaveny na 0
	vector pole[5];
	//Detekce hran, ukladani casu a posouvani pole
	void edge(int upTrsh, int lowTrsh);
	void start(int bttn, int upTrsh, int lowTrsh);
	//Pouze pro debugging odkomentuj v metodach
	void tisk(int i);
	void vecSpeed(int l1, int l2, int s);
	void pushCheck(unsigned long curTime);
	void pullCheck(unsigned long curTime);
	void reset();
	
};



//METODY VECTOR
void vector::vecSpeed1(int l1, int s) {
	unsigned long v = 0;
	v = edgeDown - edgeUp; //delta t
	v = l1 / v;
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
	speedV = s / v;

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
Vyjimkou je stav c (cyan) pro kratsi obdelniky (ktere se musi drzet) v tomto pripade zapisuje pullTime
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
		if (millis() - prevTime >= 10) {
			prevState = 'c';
			pole[i].edgeDown = millis();
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
	else if (value >= upTrsh && prevState == 'c' && pole[i].pullTime == NULL && pole[i].pushTime == 0) {
		//from cyan to blue
		prevState = 'a';

		//Saving pullTime outside of vectotr:vecSpeed2, inicially was NULL
		pole[i].pullTime = millis() + pole[i].speedV;
		pole[i].speedV = 0;
		//inkrementuji pole vektoru
		++i;
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
*/
void sensor::start(int bttn, int lowTrsh, int upTrsh) {
	value = analogRead(pinIn);

	Serial.print("Sensor: ");
	Serial.print(value);

	if (value <= lowTrsh){
			
		while (digitalRead(bttn)) {}
		prevState = 'b';
		
		pole[0].edgeUp = millis();
	}

	Serial.print("; State: ");
	Serial.println(prevState);

}

void sensor::reset() {
	for (int j = 0; j < 5; ++j) {
		pole[j].edgeDown = 0;
		pole[j].edgeUp = 0;
		pole[j].pushTime = 0;
		pole[j].pullTime = 0;
		pole[j].speedV = 0;
	}
		prevState = 'a';
		i = 0;
		digitalWrite(pinOut, LOW);
	
}

void sensor::tisk(int i) {

	Serial.print("Sensor: "); Serial.print(value); Serial.print(" ");
	/*
	Serial.println("|||||||||||||||||||||||||||");
	for (int j = 0; j < 5; ++j) {
		Serial.print("Edge up time: ");
		Serial.print(pole[j].edgeUp); Serial.print(" ");

		Serial.print("Edge down time: ");
		Serial.print(pole[j].edgeDown); Serial.println(" ");

		Serial.print("Push time: ");
		Serial.print(pole[j].pushTime); Serial.print(" ");

		Serial.print("Pull time: ");
		Serial.print(pole[j].pullTime); Serial.print(" ");
	}
	Serial.println("|||||||||||||||||||||||||||");
	*/
}	

void sensor::vecSpeed(int l1, int l2, int s) {
	for (int j = 0; j < 5; ++j) {
		if (pole[j].edgeDown > pole[j].edgeUp) {

			if (prevState == 'a') {
				pole[j].vecSpeed1(l1, s);
			}
			else if (prevState == 'c') {
				pole[j].vecSpeed2(l2, s);
			}

		}
	}
}

void sensor::pushCheck(unsigned long curTime) {
	for (int j = 0; j < 5; ++j) {
		if ((curTime >= pole[j].pushTime) && (pole[j].pushTime != 0)) {
			digitalWrite(pinOut, HIGH);
			pole[j].pushTime = 0;
		}
	}
}

void sensor::pullCheck(unsigned long curTime) {
	for (int j = 0; j < 5; ++j) {
		if ((curTime >= pole[j].pullTime) && (pole[j].pullTime != 0)) {
			digitalWrite(pinOut, LOW);
			pole[j].pullTime = 0;
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
	*lowTrsh += 5 * tmp;
	*upTrsh -= 1 * tmp;
}

/*
Change the calibrating LED to a different pin (now D2);
*/
