//vektor je vnoreny do sensor
//slouzi k ukladani casu
class vector {
public:
	unsigned long edgeUp;
	unsigned long edgeDown;
	unsigned long pushTime;
	unsigned long pullTime;

	vector();

	//speed calculation, prevState is gonna be "c" for shorter and "a" for longer ones (TBD1)
	void vecSpeed(int l, int s);
};

class sensor {
	int pinIn;
	int pinOut;
	int value;
	//creates a "debouncer"
	int prevValue;
	unsigned long prevTime;
	//prevState is gonna be "c" for shorter rectangles and "a" for longer ones
	char prevState; //a = blue, b = black, c = cyan

	//vsechny elementy jsou nastaveny na 0
	vector pole[5];

	//index pole
	int i = 0;

public:
	sensor() {}
	sensor(int in, int out);

	//detekce hrany
	void edge(int upTrsh, int lowTrsh);
	void tisk(int i);
};



//metody vektoru
vector::vector() {
	edgeUp = 0;
	edgeDown = 0;
	pushTime = 0;
	pullTime = 0;
}
//dodelat (TBD)
void vector::vecSpeed(int l, int s) {
	unsigned long v = 0;
	//delta t
	v = edgeDown - edgeUp;
	v = l / v;

	//saving the time of push and pull for normal rectangles
	pushTime = edgeUp + (s / v);
	pullTime = edgeUp + ((s + l) / v);
}


//metody sensoru
sensor::sensor(int in, int out) {
	pinIn = in;
	pinOut = out;
	value = 0;
	prevState = "a";
	prevValue = 0;
	prevTime = 0;
}
void sensor::edge(int upTrsh, int lowTrsh) {

	value = analogRead(pinIn);
	if (i >= 5)
		i = 0;

	if (value <= lowTrsh && prevState == 'a') {
		//from blue to black
		prevState = "b";
		pole[i].edgeUp = millis();
	}
	else if (value > lowTrsh && value < upTrsh && prevState == 'b' && value == prevValue) {
		//from black to cyan
		if (millis() - prevTime >= 10) {
			prevState = "c";
			pole[i].edgeDown = millis();

			//inkrementuji pole vektoru
			++i;
		}
		return;
	}
	else if (value >= upTrsh && prevState == 'b') {
		//from black to blue
		prevState = "a";
		pole[i].edgeDown = millis();

		//inkrementuji pole vektoru
		++i;
	}
	else if (value >= upTrsh && prevState == 'c') {
		//from cyan to blue
		prevState = "a";
		digitalWrite(pinOut, LOW);
	}
	//promene pro debouncer
	prevValue = value;
	prevTime = millis();
	}

void sensor::tisk(int i) {
	Serial.print("Senzor ");
	Serial.print(i);
	Serial.print(" : ");
	Serial.println(value);
}



//Kalibrace momentalne casova, predelat na tlacitko
/*
(TBD)
void calib() {

}
*/
/*
Finish up the speed calc. for shorter rectangles (see TBD1) and pushTime pullTime calculations. Shorter rectangles don't need a pullTime
Figure out the calibration process using a button. First rectangle is right under the sensor so maybe add starting time for the first one? Also find out how to position the treshold rails.
Throw a loop into main to go through all four sensors and all of their timings.
*/
