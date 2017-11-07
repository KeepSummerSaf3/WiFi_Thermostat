//RELAY CONTROL FUNCTIONS

/*Relay control is facilitated through a 8 bit Shift Register (74HC595N).
 *1 Shift Register exists onboard, allowing for a maximum of 8 digitally controlled IO ports.
 *
 *All outputs use inverted logic as a reference(5V - LOW,  0V - High). Obviously just flip this if a mix and match is needed

//--------------EXAMPLE PINOUT DIAGRAM------------------
                           |------------------|
          Zone 2 Relay  -  | Y1          Vdd  |  - 5V
          Zone 3 Relay  -  | Y2           Y0  |  - Zone 1 Relay
          Zone 4 Relay  -  | Y3           DS  |  - Serial Data
          Zone 5 Relay  -  | Y4          !OE  |  - Output Enable (GND) 
          Zone 6 Relay  -  | Y5         ST_CP |  - Latch (Arduino Pin
          Zone 7 Relay  -  | Y6         SH_CP |  - Clock (Arduino Pin 7)
          Zone 8 Relay  -  | Y7           MR  |  - Master Reset (5V)
                   GND  -  |_Vss__________Q7'_|  - Serial Out (NC)


//----------VALVE CONTROL TABLE------------

Enable/Disable 5V latch will disable all relays

S3  S2  S1  |   Y0    Y1    Y2    Y3    Y4    Y5    Y6    Y7
0   0   0       Z     0     0     0     0     0     0     0      -   Zone 1 On
0   0   1       0     Z     0     0     0     0     0     0      -   Zone 2 On
0   1   0       0     0     Z     0     0     0     0     0      -   Zone 3 On
0   1   1       0     0     0     Z     0     0     0     0      -   Zone 4 On
1   0   0       0     0     0     0     Z     0     0     0      -   Zone 5 On
1   0   1       0     0     0     0     0     Z     0     0      -   Zone 6 On
1   1   0       0     0     0     0     0     0     Z     0      -   Zone 7 On
1   1   1       0     0     0     0     0     0     0     Z      -   Zone 8 On


RELAY WIRE COLOR CODES (I/O Microcontroller)

*/

void relay_shutdown()
{
  for(byte i= 0; i<16; ++i)
    relaySRSingle.writeBit(i, LOW);
}

void relay_on(byte rly)
{
  relaySRSingle.writeBit(rly, HIGH);
}

void relay_off(byte rly)
{
  relaySRSingle.writeBit(rly, LOW);
}

//Relays are active low so all logic here considers this

void ac_control(bool ac_state)
{
   if(system_state != 0 && ac_state == 1 && heat_request != 1) //AC ON
   {
     if((second() % 5) == 0)
       Serial.println(F("AC ON"));
     relay_on(AC_RELAY);
     relay_on(FAN_RELAY);
   }

   if(ac_state == 0 || system_state == 0) //AC OFF
   {
     if((second() % 5) == 0)
       Serial.println(F("AC OFF"));
     relay_off(AC_RELAY);
   }
}

void heat_control(bool heat_state)
{
   if(system_state != 0 && heat_state == 1 && ac_request != 1) //HEAT ON
   {
     if((second() % 5) == 0)
       Serial.println(F("HEAT ON"));
     relay_on(HEAT_RELAY);
     relay_on(FAN_RELAY);
   }

   if(heat_state == 0 || system_state == 0) //HEAT OFF
   {
     if((second() % 5) == 0)
       Serial.println(F("HEAT OFF"));
     relay_off(HEAT_RELAY);
   }
}

void fan_control(bool fan_state)
{
   if(system_state != 0 && fan_mode_request == 1) //FAN ON
   {
     if((second() % 5) == 0)
       Serial.println(F("FAN ON"));
     relay_on(FAN_RELAY);
   }

   if((fan_mode_request == 0 || system_state == 0) && heat_request == 0 && ac_request == 0) //FAN OFF
   {
     if((second() % 5) == 0)
       Serial.println(F("FAN OFF"));
     relay_off(FAN_RELAY);
   }
}

void RelaySR_Setup()
{
  relaySRSingle.setBitCount(8);
  relaySRSingle.setPins(RelaySR_Data_Pin, RelaySR_Clock_Pin, RelaySR_Latch_Pin);

  //shutdown_SRs();
  
  Serial.println(F("Relay SR Test"));
  for(byte i= 0; i<16; ++i)
  {
    Serial.print(F("Zone ")); 
    Serial.print(i);
    Serial.print(F(" On..."));
    relaySRSingle.writeBit(i, HIGH);
    delay(2000);
    relaySRSingle.writeBit(i, LOW);
    delay(2000);
    relaySRSingle.writeBit(i, HIGH);
    delay(2000);
    relaySRSingle.writeBit(i, LOW);
    //valves_off(i);
    Serial.println(F("Off"));
    delay(1000);
  }
  
  relay_shutdown(); //set everything to off as part of initialization
   
}

void relay_main_control()
{
    
    if(millis() < 150000) //kill all relays for 2.5 minutes
      relay_shutdown();
    else
    {
      if(ac_request < prev_ac_request)
      {
         Serial.println(F("AC Shutdown, Fan Off in 60"));
         ac_temp_time = millis();
         ac_control(0);
         prev_ac_request = ac_request;
      }
      else if(ac_request > prev_ac_request)
      {
         Serial.println(F("AC & FAN ON"));
         ac_control(1);
         fan_control(1);
         prev_ac_request = ac_request;
      }
      else if((millis() - ac_temp_time) < (MAX_RELAY_TIME * 1000))
      {
         Serial.println(F("AC Shutdown, Fan Off"));
         fan_control(0);
         ac_temp_time = 0;
      }
      else if(ac_temp_time == 0)
      {
         //Serial.println(F("Normal AC Mode Sect."));
         ac_control(ac_request);
         fan_control(fan_mode_request);
      }

      
      if(heat_request < prev_heat_request)
      {
         Serial.println(F("HEAT Shutdown, Fan Off in 60"));
         heat_temp_time = millis();
         heat_control(0);
         prev_heat_request = heat_request;
      }
      else if(heat_request > prev_heat_request)
      {   
         Serial.println(F("HEAT & FAN ON"));
         heat_control(1);
         fan_control(1);
         prev_heat_request = heat_request;
      }
      else if((millis() - heat_temp_time) < (MAX_RELAY_TIME * 1000))
      {
         Serial.println(F("HEAT Shutdown, Fan Off"));
         fan_control(0);
         heat_temp_time  = 0;
      }
      else if(heat_temp_time == 0)
      {
         //Serial.println(F("Normal HEAT Mode Sect."));
         heat_control(heat_request);
         fan_control(fan_mode_request);
      }

      if(fan_mode_request != prev_fan_mode_request)
      {
         Serial.println(F("Setting Fan mode"));
         prev_fan_mode_request = fan_mode_request;
         fan_control(fan_mode_request);  
      }
      
    }
    
}




