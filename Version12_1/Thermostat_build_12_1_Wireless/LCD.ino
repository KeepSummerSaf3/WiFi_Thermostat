//LCD SECTION
/*
 Hitachi HD44780 driver with 16 columns by 2 lines LCD display.
 PINOUT CONNECTIONS:
 
 //      LCD BOARD PINS                   SR OUPUTS        SR INPUTS              MICROCONTROLLER PINS
 * LCD RS pin                        <--- PIN 2 (Q2)       PIN 14 (DATA)    <---  (DIO)PIN 8
 * LCD Enable                        <--- PIN 12(RCK)      PIN 12 (RCK/EN)  <---  (DIO)PIN 6
 * LCD D4 pin                        <--- PIN 3 (Q3)       PIN 11 (CLOCK)   <---  (DIO)PIN 7
 * LCD D5 pin                        <--- PIN 4 (Q4)
 * LCD D6 pin                        <--- PIN 5 (Q5)
 * LCD D7 pin                        <--- PIN 6 (Q6)
 * LCD R/W pin to GND
 * LCD Backlight pin to digital pin 10
 * VDD to +5V 
 * VSS to GND
 * wiper to LCD VO pin (pin 3)
 */
 
// initialize the library with the numbers of the interface pins
LiquidCrystal_SR lcd(LCD_DATA_PIN, LCD_CLOCK_PIN, LCD_ENABLE_PIN);

//---------------------------------
//THERMOSTAT DISPLAY PARAMTERS HERE
//---------------------------------

//Select key 
boolean select_status = 0; 

//STATIC DATA STORED IN FLASH IN THERMOSTAT_BUILD_(MAIN)

//*NOTE MOST GLOBAL VARIABLES ARE IN MAIN HEADER FILE

//local parameters for Keypad
//DFR_Key keypad;
//int localKey = 0;

//-------------------------
//CUSTOM CHARACTERS
//-------------------------

//byte thermometer[8] = { B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110 };
//byte raindrop[8] = { B00100, B00100, B01010, B01010, B10001, B10001, B10001, B01110 };
//-------------------------
//FUNCTIONS FOR LCD DISPLAY
//-------------------------

void setup_lcd() {
  
  clear_line(0);
  clear_line(1);
  pinMode(BACKLIGHT_PIN, OUTPUT); // Set LCD Backlight Pin Control
    
  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  //Print thermostat title (reused page heading)
  lcd.print(F("SMART THERMOSTAT"));
  lcd.setCursor(0,1);
  lcd.print(F("v12.1"));

  for(i=0; i<155; i+=5)
  {
    analogWrite(BACKLIGHT_PIN, i);
    delay(100);
  }
  delay(3000);
  
  for(i=155; i > 0; i-=5)
  {
    analogWrite(BACKLIGHT_PIN, i);
    delay(100);
  }
  delay(3000);

  analogWrite(BACKLIGHT_PIN, 100);
  
  clear_line(0);
  clear_line(1);
  

  set_display_freemem();
}

void clear_line(boolean line_num)
{
  lcd.setCursor(0, line_num);
  for(i = 0; i < 16; ++i)
    lcd.print(" ");
}

//function for reading buttons and navigating through options
/*
void read_button()
{
  //localKey = keypad.getKey();
  localKey = 0;
  Serial.print(F("Reading button"));
  switch(localKey)
  {
    case 1: //SYSTEM MODE KEY PRESSED 
    {
        if(op_data_[1].system_state == 2) //current system status
          op_data_[1].system_state = 0;
        else
          op_data_[1].system_state += 1;
             
        //updates made to FAN based on system status change
        if(op_data_[1].system_state == 0) //if system status is off then fan is also off
        {
          op_data_[1].fan_mode_request = 0;
          AC = HEAT = fan_state = false;
        }
        
        if(op_data_[1].system_state == 2) //if auto is set for system then auto is set for fan
          op_data_[1].fan_mode_request = 2;

       
        if(op_data_[1].system_state != op_data_[0].system_state)
        { 
          Serial.println(F("AC & HEAT reset"));
          op_data_[1].setpoint_temperature = op_data_[0].setpoint_temperature;; //set the request temperature to the previous request
          AC = HEAT = fan_state = false;
        }
         
 
        op_data_[0].system_state = op_data_[1].system_state;  //current system state becomes previous system state     
    }
      break;
    case 2: //SELECT KEY 
    {
       select_status = !select_status;        
     }
       break;
     case 3: //UP KEY PRESSED (Raise Setpoint)  OR if MANU MODE set A/C On/Off
     {
        if(op_data_[1].system_state == 1) //if system is in manual mode set A/C On/Off
        {
          ac_request = !ac_request;
          
          clear_line(1);
          lcd.setCursor(0,1);
          if(AC) //Display AC ON In MANU Mode - LINE 1
            lcd.print(F("AC ON"));          
          else //Display AC OFF In MANU Mode - LINE 2
            lcd.print(F("AC OFF"));
         }
        else
        {
          if(op_data_[1].setpoint_temperature < 86)
          {
            op_data_[0].setpoint_temperature = op_data_[1].setpoint_temperature;
            op_data_[1].setpoint_temperature += 1;
          }
        }
     }
        break;
        
     case 4: //DOWN KEY PRESSED (Lower Setpoint) OR if MANU MODE set HEAT On/Off
     {
         if(op_data_[1].system_state == 1) //if system is in manual mode set Heat On/Off
         {     
           heat_request = !heat_request;
           
           clear_line(1);
           lcd.setCursor(0,1);
           if(HEAT) //Display HEAT ON In MANU Mode - LINE 1
             lcd.print(F("HEAT ON"));          
           else //Display HEAT OFF In MANU Mode - LINE 2
             lcd.print(F("HEAT OFF"));
         }
         else  
         {
           if(op_data_[1].setpoint_temperature > 54)
           {
             op_data_[0].setpoint_temperature = op_data_[1].setpoint_temperature;
             op_data_[1].setpoint_temperature -= 1;
           }
         } 
     }
        break;
        
     case 5: //RIGHT KEY PRESSED (MENU ADVANCE)
     {
        if(select_status)
        {
          if(display_mode == 4)
            display_mode = 0;
          else
            display_mode++;
            
          clear_line(1);
        }
        else
        {
          if(op_data_[1].system_state == 0) //if system status is off then fan is also off
              op_data_[1].fan_mode_request = 0;
          if(op_data_[1].system_state == 2) //if auto is set for system then fan is set to auto mode
              op_data_[1].fan_mode_request = 2;
        }
     }
      break;
             
   }
  delay(200); 
}
*/

void set_display_system_state()
{
  lcd.setCursor(0, 0);
  lcd.print(F("SYS:"));
  if(system_state == 0)
    lcd.print(F("OFF "));
  else
  {
    if(ac_request == 1)
      lcd.print(F("AC  "));
    else if(heat_request == 1)
      lcd.print(F("HEAT"));
    else
      lcd.print(F("STBY"));
  }
}

void set_display_fan_state()
{
  lcd.setCursor(9, 0);
  lcd.print(F("FAN:"));

  if(fan_mode_request == 1 || ac_request == 1 || heat_request == 1)
    lcd.print(F("ON "));
  else
    lcd.print(F("OFF"));
}

//DISPLAY UPDATES
//display mode for second line: 0-Time and Date                             //set_display_time()
//                              1-Indoor Temperature & Indoor Humidity,     //set_display_indoor_th
//                              2-Indoor Temperature & Outdoor Temperature  //set_display_outdoor_th
//                              3-PPM/CF(RAW) & Air Quality                 //set_display_aqi
//                              5-Barometric Pressure & Weather Forcast     //set_display_forecast
//                              4-Set Temperature                           //set_display_setpoint
//                              6-Show Free Memory                          //set_display_freemem


void set_display_time()
{
  clear_line(1);
  //DISPLAY DATE AND TIME (LOCAL?)
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T |
  //  |_______________________________________|
  //  | D A Y   M O N   X X     X  X  :  X  X |   //Display Day of week, Month and day of month,  then hour and minute
  //  |_______________________________________|
  lcd.setCursor(0,1);
  lcd.print(dayShortStr(weekday())); 
  lcd.print(' ');
  lcd.print(monthShortStr(month()));
  lcd.print(' ');
  if(day() < 10)
    lcd.print('0');
  lcd.print(day());
  lcd.print(' ');
  if(hour() < 10)
    lcd.print('0');
  lcd.print(hour());
  lcd.print(':');
  if(minute() < 10)
    lcd.print('0');
  lcd.print(minute());
}

void set_display_th(byte temp, byte humid, bool io)
{
  clear_line(1);      
  //DISPLAY TEMP AND HUMIDITY (Indoor / Outdoor)
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T |
  //  |________ ______________________________|
  //  | I T : x x x * F   H  U  M  :  x  x  % |
  //  |_______________________________________|

  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T |
  //  |________ ______________________________|
  //  | O T : x x x * F   H  U  M  :  x  x  % |
  //  |_______________________________________|
  //Serial.print(F("Temperature"));
  //Serial.println(temp);
  //Serial.print(F("Humidity"));
  //Serial.println(humid);
  lcd.setCursor(0,1);
  if(io) //indoor
    lcd.print(F("OT:"));
  else
    lcd.print(F("IT:"));
  lcd.print(temp, DEC);
  lcd.setCursor(6,1);
  lcd.print((char)223);
  lcd.print('F');
  lcd.print(F(" HUM:"));
  lcd.print(humid, DEC);
  lcd.print('%');
}

void set_display_aqi()
{
  clear_line(1);
  //DISPLAY AQI
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T|
  //  |________ _____________________________|
  //  | P P M : x x x   A Q  I  :  G  O  O  D|      
  //  |______________________________________|
  lcd.setCursor(0,1);
  lcd.print(F("PPM:"));
  lcd.print(ppm, DEC);
  lcd.setCursor(7,1);
  lcd.print(F(" AQI:"));    
  if(ppm > 100)
    lcd.print(F("POOR"));
  else if(ppm > 50)
    lcd.print(F("FAIR"));
  else
    lcd.print(F("GOOD"));
}

void set_display_press()
{
  clear_line(1);
  //DISPLAY BAROMETRIC PRESSURE AND WEATHER
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T|
  //  |________ _____________________________|
  //  | P : x x x x m b      C  L  E  A  R   |  
  //  |______________________________________|
  // Serial.print(F("Atm. Presst:"));
  //Serial.println(outdoor_press);
  lcd.setCursor(0,1);
  lcd.print(F("P:"));
  lcd.print(outdoor_press, DEC);
  lcd.print(F("mb"));
  lcd.setCursor(10,1);
  lcd.print(weather_data);
}

void set_display_setpoint()
{
  clear_line(1);
  //DISPLAY SET TEMPERATURE
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T|
  //  |______________________________________|
  //  | T E M P   S E T :    x  x  *  F      |  
  //  |______________________________________|
  //Serial.print(F("Setpoint:"));
  //Serial.println(curr_setpoint_temperature);
  lcd.setCursor(0,1);
  lcd.print(F("TEMP SET: "));
  lcd.print(curr_setpoint_temp, DEC);
  lcd.print((char)223);
  lcd.print('F'); 
}

void set_display_freemem()
{
  clear_line(1);
  //DISPLAY FREE MEMORY
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T|
  //  |______________________________________|
  //  | F R E E   M E M :    x  x  x     B   |  
  //  |______________________________________|
  lcd.setCursor(0,1);
  lcd.print(F("FREE MEM: "));
  lcd.print(freeRam(), DEC);
  lcd.print(F(" B")); 
}

void set_display_wind()
{
  clear_line(1);
  //DISPLAY WIND DATA
  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | S Y S : A U T     F  A  N  :  A  U  T|
  //  |______________________________________|
  //  | W I N D :   x x x k  t  s     D  I  R|  
  //  |______________________________________|

  lcd.setCursor(0,1);
  lcd.print(F("WIND: "));
  lcd.print(wind_speed);
  lcd.print(F("kts "));
  lcd.print(wind_direction);
}

/*
void scroll_display(char * data)
{

  clear_line(1);

  //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  //  | D A Y   M O N   X X     X  X  :  X  X |   //Display Day of week, Month and day of month,  then hour and minute
  //  |_______________________________________|
  //  | S C R O L L I N G    D  A  T  A       |
  //  |_______________________________________|

  uint8_t chars_printed = 0;  //number of actual characters printed
  uint8_t scroll_count = 0; //number scroll count

  lcd.setCursor(0, 1);

  //Serial.print(F("Printing Data"));
  //Serial.println(data);

  while(data[chars_printed] != NULL)
  {
    chars_printed++;
    //Serial.print(data[chars_printed]);
    //Serial.print(',');
    lcd.print(data[chars_printed]);
  }

  //Serial.print(F("Chars printed: "));
  //Serial.println(chars_printed);

  while (chars_printed > scroll_count)
  {
    //Serial.print(F("Scroll count: "));
    //Serial.println(scroll_count);

    //Serial.println(F("Clearing Line"));
    //clear_line(1);
    lcd.setCursor(0, 1);

    for (byte k = 0; k < 16; ++k)
    {
      if (scroll_count + k > chars_printed)
      {
        //Serial.print(F("Past Array Bounds at "));
        //Serial.print(scroll_count + k);
        //Serial.print(F(" of "));
        //Serial.print(chars_printed);
        //Serial.println(F(", printing space character: "));
        lcd.print("  ");
      }
      else
      {
        char tmp_char = data[scroll_count + k];

        if (tmp_char == NULL)
        {
          //Serial.print(F("Skipping Null"));
        }
        else
        {
          //Serial.print(F("Printing char: "));
          //Serial.println(tmp_char);
          lcd.print(tmp_char);
        }
      }
    }
    scroll_count++;
  }
  scroll_data = " ";
}
*/

void display_logic()
{
  
  set_display_system_state();
  set_display_fan_state();

  if((second() <= 7) && (second() > 0))
    set_display_th(outdoor_temp, outdoor_humid, 1);
  if((second() <= 14) && (second() > 7))
    set_display_th(indoor_temp, indoor_humid, 0);
  if((second() <= 21) && (second() > 14)) 
    set_display_aqi();
  if((second() <= 28) && (second() > 21))
    set_display_setpoint();
  if((second() <= 35) && (second() > 28))
    set_display_press();
  if((second() <= 49) && (second() > 42))
    set_display_wind();
    //scroll_display(wind_data, WIND_DATA_LEN);
  if((second() <= 56) && (second() > 49))
    set_display_freemem();
  if(second() > 56)
    set_display_time();
  
}

void lcd_loop()
{
  //display_logic();
  //read_button();
  DisplayUpdate.check();
}





