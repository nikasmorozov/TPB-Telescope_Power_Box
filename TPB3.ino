//Libraries
#include <DHT.h>

//Constants
#define DHTPIN 12     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

int heater_pin1 = 3;
int heater_pin2 = 5;
int heater_pin3 = 9;
int thermistorPin1 = 1;
int thermistorPin2 = 5;
int temperatureOverDP = 20;
int delayBtwReadings = 2000;

//Variables
int chk;
float RH;  //Stores humidity value
float TEMP; //Stores temperature value
int Vo1;
int Vo2;
float R1 = 10000;
float logR2, R2, T, Tc1, Tc2, dewPoint;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int heaterCyclePercent1;

int heaterCycle1, heaterCycle2;

void setup() {
  Serial.begin(9600);

  dht.begin();

  pinMode(heater_pin1, OUTPUT);
  pinMode(heater_pin2, OUTPUT);
}

void loop() {
  //Read data and store it to variables hum and temp
    RH = dht.readHumidity();
    TEMP = dht.readTemperature();
   
  Vo1 = analogRead(thermistorPin1);
  Vo2 = analogRead(thermistorPin2);

  Tc1 = calculateTemperature(Vo1);
  Tc2 = calculateTemperature(Vo2);

  dewPoint = 243.04*(log(RH/100)+((17.625*TEMP)/(243.04+TEMP)))/(17.625-log(RH/100)-((17.625*TEMP)/(243.04+TEMP)));

   

  heaterCycle1 = map(Tc1 * 100, dewPoint * 100, (dewPoint + temperatureOverDP) * 100, 255, 0);
  heaterCycle2 = map(Tc2 * 100, dewPoint * 100, (dewPoint + temperatureOverDP) * 100, 255, 0);

  heaterCycle1 = constrain(heaterCycle1, 0, 255);
  heaterCycle2 = constrain(heaterCycle2, 0, 255);

 analogWrite(heater_pin1, heaterCycle2);
  analogWrite(heater_pin3, heaterCycle1);
  
//  DC
  analogWrite(heater_pin2, 255);

  heaterCyclePercent1 = map(heaterCycle1, 0, 255, 0, 100);
  

//Prints temp and humidity values to serial monitor
   Serial.print("Temp: ");
    Serial.print(TEMP);
    Serial.print(" C");
    
    Serial.print(", Humidity: ");
    Serial.print(RH);
    
    Serial.print(" %, Dew Point: ");
    Serial.print(dewPoint);
    Serial.println(" C");

  Serial.print("Probe 1: ");
  Serial.print(Tc1);
  Serial.print(" C");
  Serial.print(", Heater Cycle 1: ");
  Serial.print(heaterCyclePercent1);
  Serial.print("% (8bit: ");
  Serial.print(heaterCycle1);
  Serial.println(")");
  
  Serial.print("Probe 2: "); 
  Serial.print(Tc2);
  Serial.print(" C");
  Serial.print(", Heater Cycle 2: ");
  Serial.println(heaterCycle2);

  Serial.println();

 delay(delayBtwReadings);
}

float calculateTemperature(int Vo){
//  previous bad formula
//  R2 = R1 * (1023.0 / (float)Vo - 1.0);

  R2 = (R1 * (float)Vo) / (1023.0 - (float)Vo);

  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  return T - 273.15;
}
