#include "Thermistor.h"
Thermistor::Thermistor(uint8_t pin, bool genericSimpleFormula, long seriesResistor, long thermistorNominalResistance,
		float temperatureNominal, long bCoefficent, uint8_t numSamples) {
	_pin = pin;
	_genericSimpleFormula = genericSimpleFormula;
	_thermistorNominalResistance = thermistorNominalResistance;
	_temperatureNominal = temperatureNominal;
	_seriesResistor = seriesResistor;
	_bCoefficent = bCoefficent;
	_numSamples = numSamples;
}

void Thermistor::setSimpleFormula(bool genericSimpleFormula){
	_genericSimpleFormula = genericSimpleFormula;
}

void Thermistor::setThermistorNominalResistance(long thermistorNominalResistance){
	_thermistorNominalResistance = thermistorNominalResistance;
}

void Thermistor::setTemperatureNominal(float temperatureNominal){
	_temperatureNominal = temperatureNominal;
}

void Thermistor::setSeriesResistor(long seriesResistor){
	_seriesResistor = seriesResistor;
}

void Thermistor::setBCoefficent(long bCoefficent){
	_bCoefficent = bCoefficent;
}

void Thermistor::setVccToThermisor(bool value){
	_vccToThermistor = value;
}

float Thermistor::readTemperature(bool scale) {
	uint8_t i;
	float average;

	// take N samples in a row, with a slight delay
	for (i = 0; i < NUMSAMPLES; i++) {
		samples[i] = analogRead(_pin);
		delay(10);
	}

	// average all the samples out
	average = 0;
	for (i = 0; i < NUMSAMPLES; i++) {
		average += samples[i];
	}
	average /= NUMSAMPLES;

	// convert the value to resistance
	float temperature = NAN;

	if (!_genericSimpleFormula){
		temperature = calculateSteinhartHart(average, _seriesResistor, _thermistorNominalResistance, _bCoefficent, _temperatureNominal);
	}else{
		temperature = calculateSteinhartHartReduced(average, _seriesResistor);
	}

	if (scale) {
		temperature = convertCtoF(temperature);
	}

	return temperature;
}

/**
 * Convert celsius to Fahrenheit
 */
float Thermistor::convertCtoF(float c) {
	return c * 1.8 + 32;
}

/**
 * Convert Fahrenheit to Celsius
 */
float Thermistor::convertFtoC(float f) {
	return (f - 32) * 0.55555;
}

#ifdef THERMISTORE_DEBUG
long Thermistor::readVcc(void) {
	#ifdef __AVR
		// Read 1.1V reference against AVcc
		// set the reference to Vcc and the measurement to the internal 1.1V reference
	#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
		ADMUX = _BV(MUX5) | _BV(MUX0);
	#else
		ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#endif

		delay(2); // Wait for Vref to settle
		ADCSRA |= _BV(ADSC); // Start conversion
		while (bit_is_set(ADCSRA, ADSC))
			; // measuring

		uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
		uint8_t high = ADCH; // unlocks both

		long result = (high << 8) | low;

		  result = 1083630L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
		  // scale_constant = internal1.1Ref * 1023 * 1000
		  // internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)
		return result; // Vcc in millivolts
	#else
//		float vdd = readVcc(); //  ESP.getVdd33(); //ESP.getVcc();
		return 3300;

	#endif
}

#endif

/*
 * Utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
 *    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 *
 * These coefficients seem to work fairly universally, which is a bit of a
 * surprise.
 *
 * Schematic:
 *   [Ground] -- [10k-pad-resistor] -- | -- [thermistor] --[Vcc (5 or 3.3v)]
 *                                               |
 *                                          Analog Pin 0
 *
 * In case it isn't obvious (as it wasn't to me until I thought about it), the analog ports
 * measure the voltage between 0v -> Vcc which for an Arduino is a nominal 5v, but for (say)
 * a JeeNode, is a nominal 3.3v.
 *
 */
float Thermistor::calculateSteinhartHartReduced(float average,
		long _seriesResistor) {
	float resistance = NAN;
	if (_vccToThermistor){
		// Ground to series resistor and vcc to thermistor
		resistance = _seriesResistor * ((1023.0 / average) - 1);
	}else{
		// Vcc to series resistor and Ground to thermistor
		resistance = _seriesResistor / ((1023 / average) - 1);
	}

	float Temp = log(resistance);

	Temp = 1
			/ (0.001129148 + (0.000234125 * Temp)
					+ (0.0000000876741 * Temp * Temp * Temp));
	Temp = Temp - 273.15;  // Convert Kelvin to Celsius

	#ifdef THERMISTORE_DEBUG
		float vcc = readVcc() / 1000;
		DEBUG_PRINT("ADC: ");
		DEBUG_PRINT(average,0);
		DEBUG_PRINT("/1023");                           // Print out RAW ADC Number
		DEBUG_PRINT(", vcc: ");
		DEBUG_PRINT(vcc, 2);
		DEBUG_PRINT(", pad: ");
		DEBUG_PRINT((_seriesResistor / 1000.0), 2);
		DEBUG_PRINT(" Kohms, Volts: ");
		  DEBUG_PRINT(((average*vcc)/1023.0),2);
		DEBUG_PRINT(", Resistance: ");
		DEBUG_PRINT(resistance);
		DEBUG_PRINT(" ohms, ");
		DEBUG_PRINT("Celsius: ");
		DEBUG_PRINTLN(Temp, 1);                             // display Celsius
	#endif

	return Temp;                                      // Return the Temperature
}
float Thermistor::calculateSteinhartHart(float average, long _seriesResistor,
		long _thermistorNominalResistance, long _bCoefficent,
		float _temperatureNominal) {
	float resistance = NAN;
	if (_vccToThermistor){
		// Ground to series resistor and vcc to thermistor
		resistance = _seriesResistor * ((1023.0 / average) - 1);
	}else{
		// Vcc to series resistor and Ground to thermistor
		resistance = _seriesResistor / ((1023 / average) - 1);
	}

	float steinhart = resistance / _thermistorNominalResistance;     // (R/Ro)
	steinhart = log(steinhart);                  		// ln(R/Ro)

	steinhart /= (float)_bCoefficent;                   		// 1/B * ln(R/Ro)
	steinhart += 1.0 / (_temperatureNominal + 273.15); 	// + (1/To)
	steinhart = 1.0 / steinhart;                 		// Invert
	float temperature = steinhart - 273.15;             // convert K to C

	#ifdef THERMISTORE_DEBUG
		float vcc = readVcc() / 1000;
		DEBUG_PRINT("ADC: ");
		DEBUG_PRINT(average,0);
		DEBUG_PRINT("/1023");                           // Print out RAW ADC Number
		DEBUG_PRINT(", vcc: ");
		DEBUG_PRINT(vcc, 2);
		DEBUG_PRINT(", pad: ");
		DEBUG_PRINT((_seriesResistor / 1000.0), 2);
		DEBUG_PRINT(" Kohms, Volts: ");
		DEBUG_PRINT(((average*vcc)/1023.0),2);
		DEBUG_PRINT(", Resistance: ");
		DEBUG_PRINT(resistance);
		DEBUG_PRINT(" ohms, ");
		DEBUG_PRINT("Celsius: ");
		DEBUG_PRINTLN(temperature, 1);                            // display Celsius
	#endif

	return temperature;
}
