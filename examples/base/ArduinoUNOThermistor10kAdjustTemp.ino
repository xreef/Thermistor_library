#include "Arduino.h"
#include "Thermistor.h"

Thermistor therm(A3, false);

void setup(void) {
  Serial.begin(115200);
  // Adjust nominal temperature of the thermistor to 24.4 (default nominal resistance of thermistore is 10k, at 10k 25°
  therm.setTemperatureNominal(24.4);
}

void loop(void) {

	float temp = therm.readTemperature();
	Serial.println(temp);
	delay(2000);
}
