#include "Arduino.h"
#include "Thermistor.h"

// Set pin to A3, using complex formula
Thermistor therm(A3, false);

void setup(void) {
  Serial.begin(115200);

  // adjust resistance to 9900ohom, set 10kohom to nominal of thermistor, 24° nominal thermistor, and take 10 analogRead
  therm.setSeriesResistor(9900);
  therm.setThermistorNominalResistance(); // If nothing passed set to default 10000
  therm.setBCoefficent(); // If nothing passed set to default 3950
  therm.setTemperatureNominal(24);
  therm.setVccToThermisor(); // default Vcc to thermistor
}

void loop(void) {
	float temp = therm.readTemperature();
	Serial.println(temp);
	delay(2000);
}
