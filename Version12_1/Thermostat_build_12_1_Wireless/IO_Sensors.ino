//THERMOSTAT SENSORS 

//DHT Sensor data wire on Analog Pin A1
//1K resistor between VCC and the data pin (strong pullup)

// Setup a DHT22 instance
DHT dht_IndoorSensor1;

// Dust Sensor Definitions
#define DustSensor_LED 9 //LED Pin
#define DustSensor_InPin A2 //Sensor Analog Input Pin

#define DustSensorSamplingTimeWait 280 //Timeout period before sampling
//#define DustSensorDeltaTime 40  //waiting time after reading

void sensor_setup()
{
  dht_IndoorSensor1.setup(A1);
  
}

void dust_setup()
{
  pinMode(DustSensor_LED, OUTPUT);
}

void DHT_read()
{ 
  //Serial.print(F("DHT Status: "));
  //Serial.println(dht_IndoorSensor1.getStatusString());
  //Serial.print(F("Temp in C: "));
  //Serial.println(dht_IndoorSensor1.getTemperature());
  indoor_temp = (9.0/5.0) * dht_IndoorSensor1.getTemperature() + 32;
  indoor_humid = dht_IndoorSensor1.getHumidity();

  //indoor_temp = random(68, 75);
  //indoor_humid = random(40, 70);

}
/*
void Dust_read()
{
    ppm = random(0, 250);
    
    digitalWrite(DustSensor_LED, LOW); // power on the LED (Active Low)
    delayMicroseconds(DustSensorSamplingTimeWait);
    float voMeasured = analogRead(DustSensor_InPin); // read the dust value from Analog Pin
    //delayMicroseconds(DustSensorDeltaTime);
    digitalWrite(DustSensor_LED, HIGH); // turn the LED off (Active Low)

    ppm = 0.17*(voMeasured * (3.3 / 1024)) - 0.1;
}
*/

void sensor_loop()
{
    DHT_Sensor_Read.check();
    //Dust_Sensor_Read.check();    
 
}

