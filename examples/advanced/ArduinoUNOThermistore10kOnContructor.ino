#include "Arduino.h"
#include "Thermistor.h"

// Set pin to A3, using complex formula, adjust resistance to 9900ohom, set 10kohom to nominal of thermistor, 24° nominal thermistor, and take 10 analogRead
Thermistor therm(A3, false, 9900, 10000, 24, 3950, 10);

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {

	float temp = therm.readTemperature();
	Serial.println(temp);
	delay(2000);
}
