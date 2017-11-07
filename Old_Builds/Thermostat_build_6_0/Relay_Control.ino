//RELAY CONTROL FUNCTION

//Relay control only. Look to intelligence logic to make changes to operation.

//Relays are active low

// Pin 11 - HEAT
// Pin 12 - AC
// Pin 13 - FAN

void relay_control()
{
    if(AC)
    {
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
    }
    else
    {
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);    
    }
    
    if(HEAT)
    {
      digitalWrite(11, LOW);
      digitalWrite(13, LOW);
    }
    else
    {
      digitalWrite(11, HIGH);
      digitalWrite(13, HIGH);
    }
    
    if(system_status == 1)
    {
      if(fan_status == 0)  //Turn all off
      {
        digitalWrite(11, HIGH);
        digitalWrite(12, HIGH);
        digitalWrite(13, HIGH);
      }  
      else if(fan_status == 1)
        digitalWrite(13, LOW);
    }
}
