//-----------------------DATA PROCESSOR FUNCTION--------------------------
boolean process_deadzone()
{
   //Indoor temperature deadzone
    temp_diff = indoor_temp - request_temperature;
    temp_diff = abs(temp_diff);
    if(temp_diff < temp_deadband_val)
      return true;
    else 
      return false; 
}

void process_data()
{
    temp_diff = metar_table[0].temperature - metar_table[1].temperature;
    press_diff = metar_table[0].pressure - metar_table[1].pressure;
	
    //outside temperature calculation (out_temp_change:  0 - Steady, 1- Rising, 2- Falling)
    if(metar_table[0].temperature > metar_table[1].temperature && abs(temp_diff) > 3) //outside temperature is rising
      out_temp_change = 1;
    else if(metar_table[0].temperature < metar_table[1].temperature && abs(temp_diff) > 3) //outside temperature is dropping
      out_temp_change = 2;
    else //equivalent temperature
      out_temp_change = 0;

    //pressure calculation (conditions 0-CLEAR, 1-CHANGE, 2-STORMY) 
    if(metar_table[0].pressure > metar_table[1].pressure  && abs(press_diff) > 5) //pressure is rising
    {
      //Serial.println("Pressure Rising");
      press_change = 0; //incdicates +/- on LCD
      if(metar_table[0].pressure < 995)
        cond = 2;
      else if(metar_table[0].pressure > 1025)
        cond = 1;
      else
        cond = 0;
    }
    else if(metar_table[0].pressure < metar_table[1].pressure && abs(press_diff) > 5) //pressure is falling
    {
      //Serial.println("Pressure Falling");
      press_change = 1; //incdicates +/- on LCD
      if(metar_table[0].pressure < 995)
        cond = 2;
      else if(metar_table[0].pressure > 1025)
	cond = 1;
      else
	cond = 0;
    }
    else //equivalent pressure
    {
      //Serial.println("Pressure Steady");
      press_change = 2; //incdicates +/- on LCD
      if(metar_table[0].pressure < 995)
        cond = 2;
      else if(metar_table[0].pressure > 1025)
        cond = 1;
      else
        cond = 0;
     }
     
     //capture the date and time for sync from http header
     

  data_lockout = true;
	
}

//-------------------END DATA PROCESSOR FUNCTION----------------------
