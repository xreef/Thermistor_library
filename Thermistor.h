/** \mainpage Thermistor library
 *
 * MIT license
 * written by Renzo Mischianti
 */

#ifndef Thermistor_h
#define Thermistor_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Uncomment to enable printing out nice debug messages.
// #define THERMISTORE_DEBUG

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef THERMISTORE_DEBUG
	#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
#endif

#include <math.h>

// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000

class Thermistor {
public:
	/**
	 * Very long constructor, but only one parameter is mandatory _pin
	 * @param pin The Analog pin of thermistor
	 * @param genericSimpleFormula
	 * If true we use a simple formula with coefficient very gneric
	 * https://playground.arduino.cc/ComponentLib/Thermistor2,
	 * the only parameter that are considered is seriesResistor
	 * If false we use a common Steinhart-Hart Thermistor Equation and consider all parameter
	 * https://learn.adafruit.com/thermistor/using-a-thermistor
	 * @param seriesResistor Resistance in ohom
	 * @param thermistorNominalResistance Nominal resistance of thermistor (datasheet)
	 * @param temperatureNominal Nominal temperature of thermistor (datasheet)
	 * @param bCoefficent B coefficient of thermistor (datasheet)
	 * @param numSamples How many analog read (10 millisec of delay)
	 */
	Thermistor(
			uint8_t pin,
			bool genericSimpleFormula = true,
			long seriesResistor = SERIESRESISTOR,
			long thermistorNominalResistance = THERMISTORNOMINAL,
			float temperatureNominal = TEMPERATURENOMINAL,
			long bCoefficent = BCOEFFICIENT,
			uint8_t numSamples = NUMSAMPLES);

	/**
	 * Change formula true is simple formula false complete
	 */
	void setSimpleFormula(bool genericSimpleFormula = true);
	/**
	 * Thermistor nominal resistance
	 */
	void setThermistorNominalResistance(long thermistorNominalResistance = THERMISTORNOMINAL);
	/**
	 * Thermistor nominal temperature
	 */
	void setTemperatureNominal(float temperatureNominal = TEMPERATURENOMINAL);
	/**
	 * Series resistor
	 */
	void setSeriesResistor(long seriesResistor = SERIESRESISTOR);
	/**
	 * B Coefficent
	 */
	void setBCoefficent(long bCoefficent = BCOEFFICIENT);
	/**
	 * If true vcc must connect to thermistor and ground to series resistance
	 */
	void setVccToThermisor(bool value = true);
	/**
	 * Get temperature
	 * @param scale True == Fahrenheit; False == Celcius (default)
	 */
	float readTemperature(bool scale = false);
	/**
	 * Convert celsius to Fahrenheit
	 */
	float convertCtoF(float);
	/**
	 * Convert Fahrenheit to Celsius
	 */
	float convertFtoC(float);
private:
	uint16_t samples[NUMSAMPLES];
	uint8_t _pin;
	long _thermistorNominalResistance = THERMISTORNOMINAL;
	float _temperatureNominal = TEMPERATURENOMINAL;
	long _seriesResistor = SERIESRESISTOR;
	long _bCoefficent = BCOEFFICIENT;
	bool _genericSimpleFormula = true;
	uint8_t _numSamples = NUMSAMPLES;

	bool _vccToThermistor = true;

	#ifdef THERMISTORE_DEBUG
		long readVcc(void);
	#endif

	float calculateSteinhartHartReduced(float average, long _seriesResistor);
	float calculateSteinhartHart(float average, long _seriesResistor, long _thermistorNominalResistance, long _bCoefficent, float _temperatureNominal);
};

#endif
