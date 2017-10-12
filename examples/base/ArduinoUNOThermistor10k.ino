#include "Arduino.h"
#include "Thermistor.h"

Thermistor therm(A3);

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {

	float temp = therm.readTemperature();
	Serial.println(temp);
	delay(2000);
}
