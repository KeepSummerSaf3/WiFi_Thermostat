/*
THIS IS THE SECTION FOR EEPROM STORAGE
Below is a table showing where user data is stored between sessions

long is 4 BYTES
ints are 2 BYTES
unsigned int, char, byte are...well 1 BYTE

1024 BYTES as split in 4 BYTE SEGMENTS and 16 BYTE BLOCKS

0 - 3     
4 - 7     
8 - 11    
12 - 15   

16 - 19   
20 - 23   
24 - 27   
28 - 31   

32 - 35   
36 - 39   
40 - 43   
44 - 47   

48 - 51    
52 - 55   
56 - 59
60 - 63   

64 - 67    
68 - 71   
72 - 75   
76 - 79

80 - 83
84 - 87
86 - 89
90 - 93


//END SECTION ON EEPROM STORAGE NOTES ----------------------------
//BEGIN SD CARD STORAGE INFORMATION ------------------------------
/*
const size_t bufferLen = 30;

void setup_sd()
{
  Serial.println("Mem:");
  Serial.println(freeRam());
  pinMode(10, OUTPUT);
  //SPI.begin();
  SD.begin(4);
  if(ini())
    file_check();
  else
    Serial.println("Failure to open file. Check SD card");
}

boolean file_check()
{
   byte i=0;
   
   Serial.print("Unit MAC: ");
   for(i=0; i<8; ++i)
   {
     Serial.print(unit_mac[i]);
     if(i == 7)
       Serial.println();
     else
       Serial.print(':');
   }
   Serial.print("Unit IP: ");
   for(i=0; i<4; ++i)
   {
     Serial.print(unit_ip[i]);
     if(i == 3)
       Serial.println();
     else
       Serial.print(',');
   }
   Serial.print("Yahoo Server IP: ");
   for(i=0; i<4; ++i)
   {
     Serial.print(ip_weather_server[i]);
     if(i == 3)
       Serial.println();
     else
       Serial.print(',');
   }
   Serial.print("Thingspeak Server IP: ");
   for(i=0; i<4; ++i)
   {
     Serial.print(ip_thingspeak_server[i]);
     if(i == 3)
       Serial.println();
     else
       Serial.print(',');
   }
   Serial.print("API KEY: ");
   Serial.println(APIKey_write);
   Serial.print("Weather URL: ");
   Serial.println(weather_data);
   Serial.print("Time: ");
   Serial.print(schedule_data[0]);
   Serial.print(':');
   Serial.print(schedule_data[1]);
   Serial.print(" Req. Temp: ");
   Serial.println(schedule_data[2]);
}

boolean ini()
{

  char buffer[bufferLen];
  IniFile ini("/setup/thermostat.ini");
  if(!ini.open())
  {
    Serial.print("Unable to open file");
    return 0;
  }
  
  if(!ini.getMACAddress("network", "unit_mac", buffer, bufferLen, unit_mac))
    Serial.print("not read 1");
  if(!ini.getIPAddress("network", "unit_ip", buffer, bufferLen, unit_ip))
    Serial.print("not read 2");
  if(!ini.getIPAddress("network", "weather_server", buffer, bufferLen, ip_weather_server))
    Serial.print("not read 3");
  if(!ini.getIPAddress("network", "thingspeak_server", buffer, bufferLen, ip_thingspeak_server))
    Serial.print("not read 4");
  if(!ini.getValue("network", "thingspeak_key", buffer, bufferLen, APIKey_write, NULL))
    Serial.print("not read 5");
  if(!ini.getValue("network", "weather_url", buffer, bufferLen, weather_data, NULL))
    Serial.print("not read 6");
  
  Serial.println("net read complete");
  
  //user data
  ini.getValue("user_settings", "temp_deadband", buffer, bufferLen, temp_deadband_val);
  ini.getValue("user_settings", "heat_cutoff", buffer, bufferLen, heat_cutoff_val);
  ini.getValue("user_settings", "ac_cutoff", buffer, bufferLen, ac_cutoff_val);
  ini.getValue("user_settings", "humidity_cutoff", buffer, bufferLen, humidity_cutoff_val);
  ini.getValue("user_settings", "disable_heat", buffer, bufferLen, disable_heat);
  ini.getValue("user_settings", "disable_ac", buffer, bufferLen, disable_ac);
  ini.getValue("user_settings", "disable_humidifier", buffer, bufferLen, disable_humidifier);
  ini.getValue("user_settings", "disable_fan", buffer, bufferLen, disable_fan);
  
  ini.getScheduleData("schedule", "Weekday_Wake", buffer, bufferLen, schedule_data);
  Serial.println(bufferLen);
  for(byte i=0; i< bufferLen; ++i)
    Serial.print(buffer[i]);
  Serial.println("user settings read complete");
  
  ini.close();
}

//void read_data(char * str, int * data, byte skip)
//{
//   char * temp_buf;
//
//   for(byte i = skip; str[i] != ' '; ++i)
//     temp_buf += str[i];
//   
//   *data = atoi(temp_buf); 
//}
//
//void read_data(char * str, byte * data, byte skip)
//{
//   char * temp_buf;
//
//   for(byte i = skip; str[i] != ' '; ++i)
//     temp_buf += str[i];
//   
//   *data = (byte)atoi(temp_buf); 
//}
//
//void read_weather()
//{
//  char * file_buffer;
//  sdfile_descrpt = SD.open("/metars/metar.txt");
//  
//  while(sdfile_descrpt.available() > 0)
//  {
//    for(byte i=0; sdfile_descrpt.peek() != '\n'; ++i)
//    {
//      buffer[i] += sdfile_descrpt.read();
//    }
//    
//    if(strstr(buffer, "yweather:atmosphere") != NULL)
//    {
//      if(strstr(buffer,"pressure=") != NULL)
//      {
//        metar_table[1].pressure = metar_table[0].pressure;
//        read_data(strstr(buffer,"pressure=") , &metar_table[0].pressure, 9);
//      }
//      if(strstr(buffer,"rising=") != NULL)
//      {
//        read_data(strstr(buffer,"rising=") , &press_change, 7);
//      }
//    }
//    if(strstr(buffer, "yweather:condition") != NULL)
//    {
//      if(strstr(buffer,"temp=") != NULL)
//      {
//        metar_table[1].temperature = metar_table[0].temperature;
//        read_data(strstr(buffer,"temp=") , &metar_table[0].pressure, 5);
//      } 
//    }
//  } 
//}

boolean compare_time()
{
  if((isPM() ? (hour()+12) : hour()) == schedule_data[0] && schedule_data[1] == minute())
  {
    request_temperature = schedule_data[2];
    return true;
  }
  else
    return false;
}

void read_schedule()
{
  IniFile ini("/setup/thermostat.ini");
  
  ini.getScheduleData("schedule", "Weekday_Wake", buffer, bufferLen, schedule_data);
   
  if(compare_time())
    return;
  
  ini.getScheduleData("schedule", "Weekday_Leave", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
    
  ini.getScheduleData("schedule", "Weekday_Arrive", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
    
  ini.getScheduleData("schedule", "Weekday_Sleep", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
    
  ini.getScheduleData("schedule", "Weekend_Wake", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
  
  ini.getScheduleData("schedule", "Weekend_Leave", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
    
  ini.getScheduleData("schedule", "Weekend_Arrive", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
    
  ini.getScheduleData("schedule", "Weekend_Sleep", buffer, bufferLen, schedule_data);
    
  if(compare_time())
    return;
  
}
*/
//
//void write_user_data()
//{
//
//}

