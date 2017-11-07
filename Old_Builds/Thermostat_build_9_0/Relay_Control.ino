//RELAY CONTROL FUNCTION

//Relay control only (SHOULD BE WORKING FINE SO LIKELY THE PROBLEM IS NOT HERE). 
//Look to intelligence logic to make changes to operation.

void setup_relays()
{
  pinMode(HEAT_PIN, OUTPUT); // HEAT PUMP RELAY PIN
  pinMode(AC_PIN, OUTPUT); // AIR CONDITIONER RELAY PIN
  pinMode(FAN_PIN, OUTPUT); // DUCT FAN RELAY PIN
}

//Relays are active low
void relay_control()
{
    if(AC && !disable_ac)
      digitalWrite(AC_PIN, LOW);
    else
      digitalWrite(AC_PIN, HIGH);
        
    if(HEAT && !disable_heat)
      digitalWrite(HEAT_PIN, LOW);
    else
      digitalWrite(HEAT_PIN, HIGH);
    
    if((HEAT || AC) && !disable_fan)
      digitalWrite(FAN_PIN, LOW);
    else
      digitalWrite(FAN_PIN, HIGH);
     
    if(system_status == 1)
    {
      if(fan_status == 0)  //Turn all off
      {
        digitalWrite(HEAT_PIN, HIGH);
        digitalWrite(AC_PIN, HIGH);
        digitalWrite(FAN_PIN, HIGH);
      }  
      if(fan_status == 1 && !disable_fan)
        digitalWrite(FAN_PIN, LOW);
    }
}
