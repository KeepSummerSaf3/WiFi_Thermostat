//RELAY CONTROL FUNCTION

//Relay control only. Look to intelligence logic to make changes to operation.
void relay_control()
{
    if(AC)
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
    }
    else
    {
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);    
    }
    if(HEAT)
    {
      digitalWrite(13, HIGH);
      digitalWrite(11, HIGH);
    }
    else
    {
      digitalWrite(11, LOW);
      digitalWrite(13, LOW);
    }
    
    if(fan_status == 0)
      digitalWrite(13, LOW);  
    else if(fan_status == 1)
      digitalWrite(13, HIGH);
    
}
