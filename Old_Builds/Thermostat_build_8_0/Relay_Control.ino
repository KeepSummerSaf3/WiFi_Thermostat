//RELAY CONTROL FUNCTION

//Relay control only (SHOULD BE WORKING FINE SO LIKELY THE PROBLEM IS NOT HERE). 
//Look to intelligence logic to make changes to operation.

void setup_relays()
{
  pinMode(HEAT_PIN, OUTPUT); // HEAT PUMP RELAY PIN
  pinMode(AC_PIN, OUTPUT); // AIR CONDITIONER RELAY PIN
  pinMode(FAN_PIN, OUTPUT); // DUCT FAN RELAY PIN
  
  digitalWrite(HEAT_PIN, HIGH);
  digitalWrite(AC_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);
}

//Relays are active low
void relay_control()
{
    if(AC && !EEPROM.read(5))
      digitalWrite(AC_PIN, LOW);
    else
      digitalWrite(AC_PIN, HIGH);
        
    if(HEAT && !EEPROM.read(4))
      digitalWrite(HEAT_PIN, LOW);
    else
      digitalWrite(HEAT_PIN, HIGH);
    
    if((HEAT || AC) && !EEPROM.read(7))
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
      if(fan_status == 1 && !EEPROM.read(7))
        digitalWrite(FAN_PIN, LOW);
    }
}
