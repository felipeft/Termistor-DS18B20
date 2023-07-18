#include <thermistor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdio.h>
#include <math.h>

//NTC
// Analog pin used to read the NTC
#define NTC_PIN               A0

// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3394,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor

// Global temperature reading
uint16_t temp;


//resistência
/**
  Obtém a resistência de uma porta analógica informada do microcontrolador. Para isso,
  deve-se estar conectado  um resistor em série na montagem de um divisor de tensão.

  @param pin - O pino da porta à ser medida a resistência.
  @param voltageUc - A tensão nominal do microcontrolador.
  @param adcResolutionUc - O valor máximo da saída do
    ADC (Arduino UNO = 1024.0 | ESP32 = 4095.0).
  @param resistenciaEmSerie - A resistência do resistor em série ao qual
    está conectado à porta analógica.

  @return a resistência, em OHM, obtida.
*/
float getResistencia(int pin, float voltageUc, float adcResolutionUc, float resistenciaEmSerie) {
  float resistenciaDesconhecida = 0;

  resistenciaDesconhecida =
    resistenciaEmSerie *
    (voltageUc /
     (
       (analogRead(pin) * voltageUc) /
       adcResolutionUc
     ) - 1
    );

  return resistenciaDesconhecida;
}


//reverse steinhart
float calculateR(double T, double T0, double B, double R0) {
    double result = R0 * exp(B * (1 / T - 1 / T0));
    return result;
}

void ReverseSteinhart(double T) {
    
    double T0 = 25, B = 3394, R0 = 10000;
    T += 273.15; 
    T0 += 273.15; 
    double R = calculateR(T, T0, B, R0);

    Serial.print(R);
}


//Dsb
#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer = { 0x28, 0x66, 0x14, 0x57, 0x04, 0xE1, 0x3C, 0x60 };

void printTemperature(DeviceAddress deviceAddress){
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Erro de leitura");
  } 
  else {
    Serial.print("DS18B20: "); 
    Serial.print(tempC);
    Serial.println(" *C");
    Serial.print("Resistencia esperada: ");
    ReverseSteinhart(tempC);
    Serial.print("\r\n");
  }
}

//main
void setup(){
  Serial.begin(9600);
  sensors.begin();      //inicializa sensores
  sensors.setResolution(insideThermometer, 12); //configura para resolução de 10 bits
}

void loop(){
  sensors.requestTemperatures();
  printTemperature(insideThermometer);
  temp = thermistor.read();   // Read temperature
  Serial.print("\r\n\r\n");
  
  delay(500);
}
