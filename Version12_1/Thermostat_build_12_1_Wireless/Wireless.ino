//WIRELESS SECTION

//defines and configuration for AsyncLabs/Microchip module

// infrastructure - connect to AP
// adhoc - connect to another WiFi device
#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
uint8 local_ip[] = {192,168,1,88};     // IP address of WiShield
uint8 gateway_ip[] = {192,168,1,1};  // router or gateway IP address
uint8 subnet_mask[] = {255,255,255,0};  // subnet mask for the local network
const char ssid[] PROGMEM = {"OConnors"}; //max 32 bytes for SSID

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2
//unsigned char security_type = 0;

// WPA/WPA2 passphrase
const char security_passphrase[] PROGMEM = {"soccer12"};  // max 64 characters password

// WEP 128-bit keys (suggest to not use WEP due to security concerns)
// sample HEX keys
//prog_uchar wep_keys[] PROGMEM = {}; 
//{ 
//  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,  // Key 0
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 1
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 2
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Key 3
//};

// setup the wireless mode
unsigned char wireless_mode = WIRELESS_MODE_INFRA;
//SSID
unsigned char ssid_len;
//SECURITY KEY
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------

//DATA PROCESSING VARS
//byte count = 0;
bool processed = 0;

//Gather METAR data from local airfield to identify outside conditions

#define ICAO "station: KMSY" //USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
//IP Address for server
uint8 metar_ip[] = {192,168,1,10};
//char metar_server_hostname[] = "outerheaven";   //DO NOT USE CONST VALUES FOR NOW
//char metar_server_filename[] = "/metars/KMSY.txt";
//METAR Request
GETrequest METAR_req(metar_ip, 80, "outerheaven", "/metars/KMSY.txt");

//Thingspeak IP
uint8 thingspeak_ip[] = {184,106,153,149};
//Thingspeak HOST (part of header information)
//const static char TS_hostname[] PROGMEM = "api.thingspeak.com\nX-THINGSPEAKAPIKEY:NZUFCGMOVGHU7TUW";   //DO NOT USE CONST VALUES FOR NOW
//const char API_KEY[] PROGMEM = "NZUFCGMOVGHU7TUW";
//Thingspeak Request
POSTrequest post_data(thingspeak_ip, 80, "api.thingspeak.com\nX-THINGSPEAKAPIKEY:NZUFCGMOVGHU7TUW", "", TS_data_feed);

//---END OF CONFIGURATION PARAMETERS---

void METAR_data_feed(){}

void TS_data_feed()
{
  WiServer.print(F("&field1="));
  WiServer.print(indoor_temp);
  WiServer.print(F("&field2="));
  WiServer.print(indoor_humid);
  WiServer.print(F("&field3="));
  WiServer.print(ac_request);
  WiServer.print(F("&field4="));
  WiServer.print(heat_request);
  WiServer.print(F("&field5="));
  WiServer.print(fan_mode_request);
  WiServer.print(F("&field6="));
  WiServer.print(freeRam());
}


// Function that prints data from server (DEBUGGING)

void processData(char *data, int len)
{
  /*
  Serial.println(F("THINGSPEAK SERVER REPLY"));
  while (len-- > 0) {
    Serial.print(*(data++));
  } 
  Serial.println(F("END TRANSMISSION"));
  */
}

byte month_conv(char pos1, char pos2, char pos3)
{
  //Serial.print(F("Month: "));
  switch(pos1)
  {
    case 'A':
    {
      if(pos2 == 'p')
      {
        //Serial.print(F("Apr.")); 
        return 4;
      }
      else
        //Serial.print(F("Aug."));
        return 8; 
    }
    case 'D': /*Serial.print(F("Dec."));*/ return 12; 
    case 'F': /*Serial.print(F("Feb."));*/ return 2;
    case 'J': 
    {
      if(pos2 == 'a')
      {
        //Serial.print(F("Jan.")); 
        return 1; 
      }
      else if(pos3 =='l')
      {
        //Serial.print(F("Jul."));
        return 7;
      }
      else
        //Serial.print(F("Jun."));
        return 6;
    }
    case 'M':
    {
      if(pos3 == 'r')
      {
        //Serial.print(F("Mar."));
        return 3;
      }
      else
        //Serial.print(F("May."));
        return 5;
    }
    case 'N': /*Serial.print(F("Nov."));*/ return 11; 
    case 'O': /*Serial.print(F("Oct."));*/ return 10; 
    case 'S': /*Serial.print(F("Sep."));*/ return 9; 
    default: /*Serial.println(F("IDK"));*/ return 1; //default reutrns January
  }
}

// Function that prints data from the METAR server
void processMETARServerData(char * data, int len) 
{
  /* 
  Serial.println(F("METAR SERVER REPLY"));
  while (len-- > 0) {
    Serial.print(*(data++));
  } 
  Serial.println(F("END TRANSMISSION"));
  */

      if(processed == 0)
      {
         char * tmp = strstr(data, "Date:");  
         
         if(tmp != NULL)
         {
           // Uncomment for Time Debugging
           Serial.println(F("Found a time to sync with: Syncing"));
           //Serial.print(F("Month: "));
           
           byte month_ = month_conv(tmp[14], tmp[15], tmp[16]); 
           /*         
           Serial.print(F(", Day: "));
           Serial.print((tmp[11] - '0')*10 + (tmp[12] - '0'));
           Serial.print(F(", Year: "));
           Serial.print((tmp[18] - '0')*1000 + (tmp[19] - '0')*100 + (tmp[20] - '0')*10 + (tmp[21] - '0'));
           Serial.print(F(", Hour: "));
           Serial.print((tmp[23] - '0')*10 + (tmp[24] - '0'));
           Serial.print(F(", Minute: "));
           Serial.print((tmp[26] - '0')*10 + (tmp[27]- '0'));
           Serial.print(F(", Second: "));
           Serial.print((tmp[29] - '0')*10 + (tmp[30] - '0'));
           */
           
           //setTime("field for hour"   , "field for minute" , "field for second" , "field for day"    , "field for month", "fields for year")
           setTime(((tmp[23] - '0')*10 + (tmp[24] - '0')), ((tmp[26] - '0')*10 + (tmp[27]- '0')), ((tmp[29] - '0')*10 + (tmp[30] - '0')), ((tmp[11] - '0')*10 + (tmp[12] - '0')), month_, ((tmp[18] - '0')*1000 + (tmp[19] - '0')*100 + (tmp[20] - '0')*10 + (tmp[21] - '0'))); 
         }
         //Uncomment for Time Debugging
         //delay(1000);
         Serial.println(F("\n\nDATE MESSAGE:")); 
         Serial.println(tmp);
         
         /*
         for(i=0; i<35; ++i)
         {
           Serial.print(F("\nPosition: "));
           Serial.print(i);
           Serial.print(F("\t"));
           Serial.println(tmp[i]);
         }
         */
                  
         if(strstr(data, "temperature: ") != NULL && !processed)
         {
           //Serial.println(F("Found METAR Telegram"));
           process_weather(data);
           process_humidity(data);
           process_baropress(data);
           process_wind(data);
           process_temperature(data);
           process_op_data(data); 
           processed = true;
         }
           
         else
         {
           while (len-- > 0) {
              Serial.print(*(data++));
           } 
           Serial.println(F("Invalid METAR"));
           //count++;
         } 
      }    
}


void process_temperature(char * tmp)
{
  
  
    //temperature and dewpoint (relative humidity) section
    char * tmp2 = strstr(tmp, "temperature:");
         
    if(tmp2 != NULL)
    {
           
      if(tmp2[13] == '-')
      {
        if(tmp2[15] == '.')
          outdoor_temp = ((tmp2[14] - '0') + (tmp2[16] - '0')*0.1)*-1.0;        
        else if(tmp2[16] == '.')
          outdoor_temp = ((tmp2[14] - '0')*10.0 + (tmp2[15] - '0') + (tmp2[17] - '0')*0.1)*-1.0;
        else
          outdoor_temp = ((tmp2[14] - '0')*100.0 + (tmp2[15] - '0')*10.0 + (tmp2[16] - '0') + (tmp2[18] - '0')*0.1)*-1.0;
             
       }
       else
       {
         if(tmp2[14] == '.')
           outdoor_temp = (tmp2[13] - '0') + (tmp2[15] - '0')*0.1;
         if(tmp2[15] == '.')
           outdoor_temp = (tmp2[13] - '0')*10 + (tmp2[14] - '0') + (tmp2[16] - '0')*0.1;
          else
           outdoor_temp = (tmp2[13] - '0')*100 + (tmp2[14] - '0')*10 + (tmp2[15] - '0') + (tmp2[17] - '0')*0.1;
        } 
           
           
        /* 
         for(i=0; i<20; ++i)
         {
            Serial.print(F("\nPosition: "));
            Serial.print(i);
            Serial.print(F("\t"));
            Serial.println(tmp2[i]);
         }
           
        Serial.print(F("Outdoor Temp:"));
        Serial.println(var_data_[1].outdoor_temp);
        */
     }
}


void process_humidity(char * tmp)
{
  
    //Relative Humidity Section
    char * tmp2 = strstr(tmp, "rel humid:");
    
    if(tmp2 != NULL)
    {       
        outdoor_humid = (tmp2[11] - '0')*10 + (tmp2[12] - '0');

        /*
        Serial.println(F("\n\nHUMID MESSAGE:")); 
        for(i=0; i<20; ++i)
        {
           Serial.print(F("\nPosition: "));
           Serial.print(i);
           Serial.print(F("\t"));
           Serial.println(tmp2[i]);
        }

        Serial.print(F("Outdoor Humid:"));
        Serial.println(outdoor_humid);
        */
    }
}
         
        
void process_wind(char * tmp)
{    
     
     //Wind Speed Section   
     char * tmp2 = strstr(tmp, "wind data: ");

     //Serial.println(F("\n\nWIND MESSAGE:"));
     //Serial.println(tmp);
     
     if(tmp2 != NULL)
     {
        
        wind_speed = (tmp2[11] - '0')*100 + (tmp2[12] - '0')*10 + (tmp2[13] - '0');

        
        for(i=0; i<3; ++i)
        {
          if(tmp2[i+20] != 'x');
            wind_direction[i] = tmp2[i+20];            
        }
      }
/*
      Serial.println(F("\n\nWind MESSAGE:")); 
      for(i=0; i<25; ++i)
      {
         Serial.print(F("\nPosition: "));
         Serial.print(i);
         Serial.print(F("\t"));
         Serial.println(tmp2[i]);
      }

      Serial.print(F("Wind Speed: "));
      Serial.print(wind_speed);
      Serial.print(F("Wind Dir: ")); 
              for(i=0; i<3; ++i)
                Serial.print(wind_direction[i]);
                
                
      Serial.println(' ');
*/
}

void process_baropress(char * tmp)
{
  
     //Barometric Pressure section
         
     char * tmp2 = strstr(tmp, "pressure:");
     
     if(tmp2 != NULL)
     {
      /*
       Serial.print("Pieces: ");
       Serial.print((tmp2[10] - '0')*1000);
       Serial.print('+');
       Serial.print((tmp2[11] - '0')*100);
       Serial.print('+');
       Serial.print((tmp2[12] - '0')*10);
       Serial.print('+');
       Serial.println(tmp2[13] - '0');
       */
       outdoor_press = (tmp2[10] - '0')*1000 + (tmp2[11] - '0')*100 + (tmp2[12] - '0')*10 + (tmp2[13] - '0');
       //Serial.print(F("Outdoor Pressure:"));
       //Serial.println(outdoor_press);
       
       //Enable for Debugging Pressure
       /*
       for(i=0; i<18; ++i)
       {
        Serial.print(F("\nPosition: "));
        Serial.print(i);
        Serial.print(F("\t"));
        Serial.println(tmp2[i]);
       }
       Serial.print(F("Pressure:"));
       Serial.println(outdoor_press);
       */
     } 
}

void process_weather(char * tmp)
{
  
       char * tmp2 = strstr(tmp, "weather:");

       //Serial.println(F("\n\nWeather MESSAGE:"));
       //Serial.println(tmp);
     
       if(tmp2 != NULL)
       {
          for(i=0; i < WEATHER_DATA_LEN; ++i)
          {
            weather_data[i] = tmp2[i+9]; 
            //Serial.print(tmp2[i]);
            //Serial.print('=');
            //Serial.print(tmp2[i], DEC);
            //Serial.print(F(" != "));
            //Serial.print('\r', DEC);
            //Serial.print(F(" or != "));
            //Serial.println('\n', DEC);

            if(tmp2[i+8] == '\r' || tmp2[i+8] == '\n')
              break;
              
            //delay(1000);
          }
/*
          Serial.println(F("\n\nWeather MESSAGE:")); 
          for(i=0; i<25; ++i)
          {
             Serial.print(F("\nPosition: "));
             Serial.print(i);
             Serial.print(F("\t"));
             Serial.println(tmp2[i]);
          }
      
          Serial.println(F("\n"));
          for(i=0; i<WEATHER_DATA_LEN; ++i)
             Serial.println(weather_data[i]);
*/       
       }
}

void process_op_data(char * tmp)
{
  
       char * tmp2 = strstr(tmp, "ac:");

       //Serial.println(F("\n\nOP DATA MESSAGE:")); 
       if(tmp2 != NULL)
       {
         ac_request = tmp2[4];
         //Enable for Debugging 
         for(i=0; i<6; ++i)
         {
          //Serial.print(F("\nPosition: "));
          //Serial.print(i);
          //Serial.print(F("\t"));
          //Serial.println(tmp2[i]);
         }
         
          //Serial.print(F("AC Request: "));
          //Serial.println(ac_request);
       }

       tmp2 = strstr(tmp, "heat:");

       if(tmp2 != NULL)
       {
         heat_request = tmp2[6];
         //Enable for Debugging Pressure
         /*
         for(i=0; i<7; ++i)
         {
          Serial.print(F("\nPosition: "));
          Serial.print(i);
          Serial.print(F("\t"));
          Serial.println(tmp2[i]);
         }
         
          Serial.print(F("HEAT Request: "));
          Serial.println(heat_request);
          */
       }
}  

// This is our page serving function that generates web pages
boolean sendPage(char* URL) {

  Serial.print(F("URL Requested: "));
  Serial.println(URL);
  
  if(strcmp(URL, "/sys_off") == 0)
  {
    Serial.println(F("System Off"));
    system_state = 0;
    fan_mode_request = 0;
  }
  
  if(strcmp(URL, "/sys_on") == 0)
  {
    Serial.println(F("System On"));
    system_state = 1;
    
  }
  
  if(strcmp(URL, "/temp_inc") == 0)
  {
    Serial.println(F("Temp Increase"));
    if(curr_setpoint_temp < 85)
    {
      prev_setpoint_temp = curr_setpoint_temp;
      curr_setpoint_temp +=1;
    }

  }
  
  if(strcmp(URL, "/temp_dec") == 0)
  {
    Serial.println(F("Temp Decrease"));
    if(curr_setpoint_temp > 55)
    {
      prev_setpoint_temp = curr_setpoint_temp;
      curr_setpoint_temp -=1;
    }

  }
  
  if(strcmp(URL, "/ac_on") == 0)
  {
    Serial.println(F("AC On"));
    ac_request = 1;
  }
  
  if(strcmp(URL, "/ac_off") == 0)
  {
    Serial.println(F("AC Off"));
    ac_request = 0;
  }

  if(strcmp(URL, "/heat_on") == 0)
  {
    Serial.println(F("Heat On"));
    heat_request = 1;
  }

  if(strcmp(URL, "/heat_off") == 0)
  {
    Serial.println(F("Heat Off"));
    heat_request = 0;
  }
  
  if(strcmp(URL, "/fan_on") == 0)
  {
    Serial.println(F("Fan On"));
    fan_mode_request = 1;
  }

  if(strcmp(URL, "/fan_off") == 0)
  {
    Serial.println(F("Fan Off"));
    fan_mode_request = 0;
  }
   
  if(strcmp(URL, "/favicon.ico") == 0)
  {
    Serial.println(F("Favicon.ico"));
    return true;
  }

  if(strcmp(URL, "/aqi") == 0)
  {
    Serial.println(F("AQI Req."));
    if(ppm > 100)
      WiServer.print(F("POOR"));
    else if(ppm > 50)
      WiServer.print(F("FAIR"));
    else
      WiServer.print(F("GOOD"));
    return true;
  }

  if(strcmp(URL, "/wind_dir") == 0)
  {
    Serial.println(F("Wind Dir Req."));
    for(i=0; i<3; ++i)
      WiServer.print(wind_direction[i]);
    return true;
  }

  if(strcmp(URL, "/time") == 0)
  {
    Serial.println(F("Time Req."));
    WiServer.print(dayShortStr(weekday()));
    WiServer.print(F(" "));
    WiServer.print(monthShortStr(month()));
    WiServer.print(F(" "));
    WiServer.print(day());
    WiServer.print(F(" "));
    WiServer.print(year());
    WiServer.print(F(" "));
    if( hour() < 10)
      WiServer.print(F("0"));
    WiServer.print(hour());
    WiServer.print(F(":"));
    if( minute() < 10)
      WiServer.print(F("0"));
    WiServer.print(minute());
    WiServer.print(F(":"));
    if( second() < 10)
      WiServer.print(F("0"));
    WiServer.print(second());
    return true;
  }
  

    Serial.println(F("Sys Info"));
    WiServer.print(F("<html><html><body><head>")); //page start
    WiServer.print(F("System State: "));
    WiServer.print(system_state); //Print System Status Bit
    WiServer.print(F("<br>"));
    WiServer.print(F("Fan State: "));
    WiServer.print(fan_mode_request); //Print User Fan Mode Request
    WiServer.print(F("<br>"));
    WiServer.print(F("AC Req: "));
    WiServer.print(ac_request);
    WiServer.print(F("<br>"));
    WiServer.print(F("HEAT Req: "));
    WiServer.print(heat_request);
    WiServer.print(F("<br>"));
    WiServer.print(F("Indoor Temp: "));
    WiServer.print(indoor_temp); //Print Indoor Temperature
    WiServer.print(F("<br>"));
    WiServer.print(F("Indoor Humid: "));
    WiServer.print(indoor_humid); //Print Indoor Humidity
    WiServer.print(F("<br>"));
    WiServer.print(F("Outdoor Temp: "));
    WiServer.print(outdoor_temp); //Print Outdoor Temperature
    WiServer.print(F("<br>"));
    WiServer.print(F("Outdoor Humid: "));
    WiServer.print(outdoor_humid); //Print Outdoor Humidity
    WiServer.print(F("<br>"));
    WiServer.print(F("Setpoint Temp: "));
    WiServer.print(curr_setpoint_temp); //Print Current Setpoint Temperature
    WiServer.print(F("<br>"));
    WiServer.print(F("Wind Data: "));
    WiServer.print(wind_speed);
    WiServer.print(F(", "));
    for(i=0; i<3; ++i)
      WiServer.print(wind_direction[i]);
    WiServer.print(F("<br>"));
    WiServer.print(F("Outdoor Press: "));
    WiServer.print(outdoor_press);
    WiServer.print(F("<br>"));
    WiServer.print(F("Weather Data: "));
    for(i=0; i<WEATHER_DATA_LEN; ++i)
      WiServer.print(weather_data[i]);
    WiServer.print(F("<br>"));
    WiServer.print(F("PPM: "));
    WiServer.print(ppm);
    WiServer.print(F(", "));
    { //Print Indoor Air Quality Index
      if(ppm > 100)
        WiServer.print(F("POOR"));
      else if(ppm > 50)
        WiServer.print(F("FAIR"));
      else
        WiServer.print(F("GOOD"));
    } 
    WiServer.print(F("<br>"));
    WiServer.print(F("System Time: "));
    WiServer.print(F(" "));
    WiServer.print(dayShortStr(weekday()));
    WiServer.print(F(" "));
    WiServer.print(monthShortStr(month()));
    WiServer.print(F(" "));
    WiServer.print(day());
    WiServer.print(F(" "));
    WiServer.print(year());
    WiServer.print(F(" "));
    if( hour() < 10)
      WiServer.print(F("0"));
    WiServer.print(hour());
    WiServer.print(F("&#58;"));
    if( minute() < 10)
      WiServer.print(F("0"));
    WiServer.print(minute());
    WiServer.print(F("&#58;"));
    if( second() < 10)
      WiServer.print(F("0"));
    WiServer.print(second());
    WiServer.print(F("<br>"));
    WiServer.print(F("Uptime Seconds:"));
    WiServer.print(millis()/1000);
    WiServer.print(F("<br><br>"));
    WiServer.print(F("SMART THERMOSTAT v12.1")); //page heading
    WiServer.print(F("</body></html>")); //page end
    //Serial.println(F("Page End"));
    return true;

}

void wireless_setup() {

  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendPage);

  //WiServer to generate log messages (optional)
  //WiServer.enableVerboseMode(false);
  
  METAR_req.setReturnFunc(processMETARServerData);
  post_data.setReturnFunc(processData);
  
  METAR_req.submit(); 
}

void thingspeak_loop()
{
  post_data.submit();
  //Serial.println(F("Thingspeak Updated"));
} 

void metar_loop()
{
  processed = 0; //reset parser processed boolean
  //Serial.println(F("Metar Requested"));
  METAR_req.submit(); 
}

void wireless_loop(){  
  // Run WiServer
  WiFi_MetarQuery.check();
  WiFi_ThingspeakQuery.check();
  WiServer.server_task(); 
  //if(count > 30)
  //  software_Reset();
}

