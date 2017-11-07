//ETHERNET SECTION

//Ethernet configuration parameters ----------------------------------------

byte unit_mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x58, 0x28};

//End of Ethernet configuration parameters ---------------------------------

char * metarServerName;
char metarFile[8];  

EthernetClient weather_client;

// Thingspeak Data Server
//IPAddress ip_thingspeak_server = {184,106,153,149};
//char ThingspeakServerName[] = "api.thingspeak.com";
//char APIKey_write[] = "NZUFCGMOVGHU7TUW";


//---END OF CONFIGURATION PARAMETERS---

void wired_setup() {

  Ethernet.begin(unit_mac);
  Serial.println("Ethernet connected");
}

char * data;

void wired_loop()
{
  if((minute() % 30) == 0)  //METAR request every 5 minutes 
  {
      metar_table[1].temperature = metar_table[0].temperature;
      metar_table[1].pressure = metar_table[1].pressure;
      //retreive METARS
      if(weather_client.connect(metarServerName,80));
      {
         //Serial.println(F("connected to webserver"));
         weather_client.print(F("GET /"));
         weather_client.print(metarFile);
         weather_client.print(F(" HTTP/1.1\r\n\r\n"));
         //client.print(F(" HTTP/1.0"));
      }
  }

  if(weather_client.available())
  {
    i=0;
    
    do
    {
       buffer[i] = weather_client.read();
       ++i;
       
    }
    while(weather_client.available() && buffer[i] != '\n');
       
      
       data = strstr(buffer, "temperature:");
        
       if(data != NULL)
       {
         if(data[13] == '-')
         {
//           if(data[15] == '.')
//             metar_table[0].temperature = ((data[14] - '0') + (data[16] - '0')*0.1)*-1.0;        
//           else if(data[16] == '.')
//             metar_table[0].temperature = ((data[14] - '0')*10.0 + (data[15] - '0') + (data[17] - '0')*0.1)*-1.0;
//           else
//             metar_table[0].temperature = ((data[14] - '0')*100.0 + (data[15] - '0')*10.0 + (data[16] - '0') + (data[18] - '0')*0.1)*-1.0;
         }
         else
         {
//           if(data[14] == '.')
//             metar_table[0].temperature = (data[13] - '0') + (data[15] - '0')*0.1;
//           if(data[15] == '.')
//             metar_table[0].temperature = (data[13] - '0')*10 + (data[14] - '0') + (data[16] - '0')*0.1;
//           else
//             metar_table[0].temperature = (data[13] - '0')*100 + (data[14] - '0')*10 + (data[15] - '0') + (data[17] - '0')*0.1;
         }    
       }
       //CONVERSION TO FAHRENHEIT
       //metar_table[0].temperature = round(((9.0/5.0)*metar_table[0].temperature) + 32.0);
       //Serial.print("Temperature: ");
       //Serial.print(metar_table[0].temperature);
       //Serial.println(" F");
       
       data = strstr(data, "pressure:");
        
       if(data != NULL)
       {
//          if(data[13] == ' ' || data[13] == '.')
//            metar_table[0].pressure = (data[10] - '0')*100 + (data[11] - '0')*10 + (data[12] - '0');
//          else
//            metar_table[0].pressure = (data[10] - '0')*1000 + (data[11] - '0')*100 + (data[12] - '0')*10 + (data[13] - '0');
          
          weather_client.stop();
       }    
   } 
}

