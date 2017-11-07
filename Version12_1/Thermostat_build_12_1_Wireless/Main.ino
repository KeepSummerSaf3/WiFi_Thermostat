/*
Main Setup function and Main program loop
*/


//-------------------------SETUP FOR PROGRAM---------------------------
void setup()
{
  //Debugging only - start serial connection and set bode rate
  Serial.begin(9600);

  sensor_setup();
  
  setup_lcd();
  Serial.println(F("LCD Setup Complete"));
  
  RelaySR_Setup();
  Serial.println("Relays Configured");
  
  Serial.println(F("Acquiring Wireless signal"));
  wireless_setup();

}

//---------------------------MAIN PROGRAM LOOP----------------------------
int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void software_Reset()
{
  asm volatile(" jmp 0");
}

void check_freeRam()
{
  Serial.print(F("Free Ram: "));
  Serial.println(freeRam());
}

void data_readout()
{
  Serial.print(F("SYS STATE: "));
  Serial.println(system_state);
  Serial.print(F("FAN MODE: "));
  Serial.println(fan_mode_request);
  Serial.print(F("Setpoint Temp: "));
  Serial.println(curr_setpoint_temp);
  Serial.print(F("Indoor Temp: "));
  Serial.println(indoor_temp);
  Serial.print(F("Outdoor Temp: "));
  Serial.println(outdoor_temp);
  Serial.print(F("Outdoor Humidity: "));
  Serial.println(outdoor_humid);
  Serial.print(F("Indoor Humidity: "));
  Serial.println(indoor_humid);
  Serial.print(F("Atmos Press: "));
  Serial.println(outdoor_press);
  Serial.print(F("Weather: "));
  Serial.println(weather_data);
}

void loop()
{
  lcd_loop();
  wireless_loop();
  sensor_loop();
  
  //PIDLoops();
  relay_main_control();
  //test_relays();
  //FreeMem_Display.check();
  //ReadOut_Variables.check();

}
