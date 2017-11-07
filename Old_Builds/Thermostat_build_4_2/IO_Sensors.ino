//THERMOSTAT SENSORS 

//DHT Sensor data wire on Analog Pin A1
//4.7K resistor between VCC and the data pin (strong pullup)

//Dust Sensor on Analog Pin A2
//LED control on Digital Pin 1


// Setup a DHT22 instance
DHT22 myDHT22(A1);
DHT22_ERROR_t errorCode;

//Metro wait_timer = Metro(0.25); //timer for LED 

//int dustPin=0; 
//int delayTime=280; 
//int delayTime2=40; 
//float offTime=9680; 
//int dustVal=0; 

void DHT_read()
{ 
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
	case DHT_ERROR_NONE:
        {
	  indoor_temp = ((9.0/5.0)*(myDHT22.getTemperatureC() + 32.0)-33);
	  indoor_humidity = myDHT22.getHumidity();
//          Serial.println(indoor_temp);
//          Serial.println(indoor_humidity);
	}
          break;
//	default: 
//	Serial.print("An Error Occured");
   }
}

//void Dust_read()
//{
//  digitalWrite(1,LOW); // power on the LED (Active Low)
//  if(wait_timer.check() == 1) 
//    dustVal = analogRead(A2); // read the dust value 
//  ppm = ppm+dustVal; 
//  digitalWrite(1,HIGH); // turn the LED off 
//  
//  dustdensity = 0.17*(ppm/0.0049)-0.1; 
//  ppmpercf = ((ppm/0.0049)-0.0256)*120000; 
//  if (ppmpercf < 0) 
//    ppmpercf = 0; 
//  if (dustdensity < 0 ) 
//    dustdensity = 0; 
//  if (dustdensity > 0.5)
//    dustdensity = 0.5; 
//  
//  //Serial.println(dustVal);
//  ppm=0;  
//}

void sensor_loop()
{
  if(five_min_cnt > 254)
  { 
      //Dust_read();
      DHT_read();    
  }
}

