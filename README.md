This is an Arduino and esp8266 library for the Thermistore.

Tutorial: 

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Thermistor. Check that the Thermistor folder contains `Thermistor.cpp` and `Thermistor.h`. Place the Thermistor library folder your `<arduinosketchfolder>/libraries/` folder. You may need to create the libraries subfolder if its your first library. Restart the IDE.

# Reef complete Thermistor Temperature #

Simple library to get temperature from thermistor. 
All test is doing with MF58 10k Thermistor with B: 3950.

## Code ##
**Basic contrusctor:**
Using a general custom coefficient precalculated that are very general.
```cpp
Thermistor therm(A3);
```
than read temperature.
```cpp
	float temp = therm.readTemperature();
```

**Activate full parameter formula:**
Pass false to flag of simple formula on constructor to activate the full standard Steinhart-Hart formula.
```cpp
Thermistor therm(A3, false);
```
change parameter with method (in setup or during execution of code).
```cpp
  // adjust resistance to 9900ohom, set 10kohom to nominal of thermistor, 24° nominal thermistor, and take 10 analogRead
  therm.setSeriesResistor(9900);
  therm.setThermistorNominalResistance(); // If nothing passed set to default 10000
  therm.setBCoefficent(); // If nothing passed set to default 3950
  // Adjust nominal temperature of the thermistor to 24.4 
  // default nominal resistance of thermistore is 10k, at 10k 25°
  therm.setTemperatureNominal(24.4);
  therm.setVccToThermisor(); // default Vcc to thermistor
```
do the same with only constructor.
```cpp
// Set pin to A3, using complex formula, adjust resistance to 9900ohom, set 10kohom to nominal of thermistor, 24° nominal thermistor, and take 10 analogRead
Thermistor therm(A3, false, 9900, 10000, 24.4, 3950, 10);
```

than read temperature.
```cpp
	float temp = therm.readTemperature();
```



## Thermistor connection schema ##
Vcc on thermistor
![Vcc on thermistor](https://github.com/xreef/Thermistor_library/blob/master/examples/base/connectionDiagramVccToThermistor.png)

Vcc on resistor
![Vcc on resistor](https://github.com/xreef/Thermistor_library/blob/master/examples/base/connectionDiagramVccToResistance.png)
