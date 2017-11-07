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
*/


void setup_sd()
{
  //Serial.println("Mem:");
  //Serial.println(freeRam());
  //pinMode(10, OUTPUT);
  //SPI.begin();
  //if(!SD.begin(4))
  SD.begin(4);
    //Serial.println(F("Init failed"));
  if(!config_file_read())
  {
    return;
    //Serial.println(F("Read error occured. Check SD card"));
  }
  else
    file_check();

}

void read_line(char * buff)
{
   memset(&buff[0], 0, sizeof(buff));

   for(i=0; i < 31; ++i)
   {
     buff[i] = sdfile_descrpt.read();
     delay(500);
     //Serial.print(buff[i]);
     if(buff[i] == '\n')
       break;
     if(buff[0] == '#')
     {
       //Serial.println(F("\nFound comment"));
       do   //fast forward until a new line is read
       {
         buff[0] = sdfile_descrpt.read(); 
         delay(500);
         //Serial.print(buff[0]);        
       }while(buff[0] != '\n');
     }  
   }
}


boolean file_check()
{
   //Serial.print(F("\nServer Name:"));
   //Serial.println(metarServerName);
   //Serial.print(F("\nMetar File:"));
   //Serial.println(metarFile);
}

boolean config_file_read()
{
  sdfile_descrpt = SD.open("thermostat.txt");
  
  if(sdfile_descrpt)
  {
    //Serial.println(F("opened file"));
     
    //read from SD while not End of File
    while(sdfile_descrpt.available())
    {
      read_line(buffer);
   
      if(strcmp("[user_settings]", buffer))
      {
        //Serial.println(F("Found user_settings section"));
        read_line(buffer);
        temp_deadband_val << buffer[0] << buffer[1] << buffer[2];
        heat_cutoff_val = buffer[4]*100 + buffer[5]*10 + buffer[6];
        ac_cutoff_val = buffer[8]*100 + buffer[9]*10 + buffer[11];
        humidity_cutoff_val = buffer[13]*100 + buffer[14]*10 + buffer[15];
      }
      
      if(strcmp("[disablements]", buffer))
      {
        read_line(buffer);
        disable_heat = buffer[0];
        disable_ac = buffer[2];
        disable_humidifier = buffer[4];
        disable_fan = buffer[6];
      }
      
      if(strcmp("[EOF]", buffer))
      {
        //Serial.println(F("End of file...exiting"));
        break;
      }
    }
    //Serial.println(F("Closing file"));
    sdfile_descrpt.close();
    return 1;
    
  }
  else
  {
    //Serial.println(F("Failure to open file"));
    return 0;
  } 
}

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
  //sdfile_descrpt = SD.open("schedule.txt");
  for(i=0; i < 8; ++i)
  {
    //read_line(buffer);
    schedule_data[0] = buffer[0]*10 + buffer[1];
    schedule_data[1] = buffer[3]*10 + buffer[4];
    schedule_data[2] = buffer[6]*10 + buffer[7];
    if(compare_time())
      break;
  }
  
}


