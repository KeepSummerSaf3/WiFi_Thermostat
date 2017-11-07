/*
THIS IS THE SECTION FOR EEPROM STORAGE
Below is a table showing where user data is stored between sessions

long is 4 BYTES
ints are 2 BYTES
unsigned int, char, byte are...well 1 BYTE

1024 BYTES as split in 4 BYTE SEGMENTS and 16 BYTE BLOCKS

0 - 3     0 - byte temp_deadband_val = 3; //deadband value (set to 255 if you dont want deadband)
          1 - byte heat_cutoff_val = 60; //(only 0 to 100 accepted)
          2 - byte ac_cutoff_val = 60; //(only 0 to 100 accepted)
          3 - byte humity_cutoff_val = 60; //(only 0 to 100 accepted)
4 - 7     4 - boolean disable_heat = false; //disables the heater
          5 - boolean disable_ac = false; //disables the ac
          6 - boolean disable_humidifer = false; //disables the humidifer
          7 - boolean disable_fan = false; //disables the fan
8 - 11    8 - boolean vacant = false; //house is empty or not empty 
12 - 15   

16 - 19    
20 - 23   
24 - 27   Encryption Key  k[0] , k[1]  //stores
28 - 31   Encryption Key  k[2] , k[3]  //stores

32 - 35   ------------------------
36 - 39   SCH_table[0]     //storage for WEEKEND Table request temperatures
40 - 43
44 - 47   ------------------------

48 - 51   ------------------------
52 - 55   SCH_table[1]     //storage for WEEKDAY Table request temperatures
56 - 59
60 - 63   ------------------------

64 - 67    
68 - 71   
72 - 75   
76 - 79

80 - 83
84 - 87
86 - 89
90 - 93

94 - 
*/
/*
void setup_default()
{
   temp_deadband_val = EEPROM.read(0);
   heat_cutoff_val = EEPROM.read(1);
   ac_cutoff_val = EEPROM.read(2);
   humity_cutoff_val = EEPROM.read(3);
   
   disable_heat = EEPROM.read(4);
   disable_ac = EEPROM.read(5);
   disable_humidifer = EEPROM.read(6);
   disable_fan = EEPROM.read(7);
   vacant = EEPROM.read(8);
}

//provides the schedule for the thermostat in scheduled mode
boolean loaded = false;
*/
void read_schedule()
{
  /*
   if(!loaded)
   {
     loaded = load_schedule();
   }
   
   for(byte j = 0; j < 3; ++j)
   {
     switch(weekday()-1)
     {
       case 1:
       case 2:
       case 3:
       case 4:
       case 5:
       {
         if(SCH_table[1].ITEMS_table[j].hour_ == (isPM() ? (hour()+12) : hour()) && SCH_table[1].ITEMS_table[j].minute_ == minute())
         {
           if(SCH_table[1].ITEMS_table[j].request_temp != 0)
             request_temperature = SCH_table[1].ITEMS_table[j].request_temp; 
         } 
       }break;
       
       default:
       {
         if(SCH_table[0].ITEMS_table[j].hour_ == (isPM() ? (hour()+12) : hour()) && SCH_table[0].ITEMS_table[j].minute_ == minute())
         {
           if(SCH_table[0].ITEMS_table[j].request_temp != 0)
             request_temperature = SCH_table[0].ITEMS_table[j].request_temp; 
         }
       }break;
     }      
   }
  */ 
}

boolean load_schedule()
{
   //EEPROM_readAnything(32, SCH_table[0]);
   //EEPROM_readAnything(48, SCH_table[1]);
   //return true;
}

void write_user_data()
{
   //EEPROM.write(0, temp_deadband_val);
   //EEPROM.write(1, heat_cutoff_val);
   //EEPROM.write(2, ac_cutoff_val);
   //EEPROM.write(3, humity_cutoff_val);
   //EEPROM.write(4, disable_heat);
   //EEPROM.write(5, disable_ac);
   //EEPROM.write(6, disable_humidifer);
   //EEPROM.write(7, disable_fan);
   //EEPROM.write(8, vacant);
   //EEPROM_writeAnything(32, SCH_table[0]);
   //EEPROM_writeAnything(48, SCH_table[1]);
}

