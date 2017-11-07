//LCD SECTION
/*
 Hitachi HD44780 driver with 16 columns by 2 lines LCD display.
 The circuit:
 * LCD RS pin to digital pin 8  ->  alter to pin 2 
 * LCD Enable pin to digital pin 9  -> alter to pin 3
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * LCD Backlight pin to digital pin 10
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
 
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//digital pin 10 is for backlight control

//---------------------------------
//THERMOSTAT DISPLAY PARAMTERS HERE
//---------------------------------


//STATIC DATA STORED IN FLASH IN THERMOSTAT_BUILD_(MAIN)


//*NOTE MOST GLOBAL VARIABLES ARE IN MAIN HEADER FILE

//local parameters for Keypad
DFR_Key keypad;
int localKey = 0;

//-------------------------
//FUNCTIONS FOR LCD DISPLAY
//-------------------------

void setup_lcd() {
  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[30]));
  lcd.print(buffer);
  lcd.setCursor(0,1);
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[31]));
  lcd.print(buffer);
  delay(2000);
  
  lcd.clear();
  
  lcd.setCursor(0,0);
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[30]));
  lcd.print(buffer);
  lcd.setCursor(0,1);
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[31]));
  lcd.print(buffer);
  delay(2000);
  
  lcd.clear();

  keypad.setRate(10);
}

//function for reading buttons and navigating through options
void read_button()
{
  localKey = keypad.getKey();

  switch(localKey)
  {
    case 1: //SELECT KEY PRESSED 
    {
        select_record = !select_record;
    }
      break;
    case 2: //FAN KEY PRESSED and MODE KEY PRESSED
    {
      switch(select_record) //ALTER FAN STATUS if select key was previously recorded as pressed
      {
        case 0:
        {
          if(system_status == 0) //if system status is off then fan is also off
            fan_status = 0;
          else if(system_status == 2 || system_status == 3) //if auto OR eff is set for system then auto is set for fan
            fan_status = 2;
          else //for manual modes 
          {
            if(fan_status == 0)
              fan_status = 1;
            else
              fan_status = 0;
           }
        }
          break;
        case 1: //ALTER SYSTEM STATUS if select key was previously recorded as pressed
        {
           if(system_status == 3) //system status BEFORE key pressed
             system_status = 0;
           else
             system_status++;
             
            //updates made to FAN based on system status change
            if(system_status == 0) //if system status is off then fan is also off
              fan_status = 0;
   
            if(system_status == 2 || system_status == 3) //if auto OR eff is set for system then auto is set for fan
              fan_status = 2;
              
            request_temperature = 73; //set the request temperature to a default state again
        }
          break;
      }
        
     }
       break;
      case 3: //UP KEY PRESSED (Raise Setpoint)  OR if MANU MODE set A/C On/Off
      {
         if(system_status == 1) //if system is in manual mode set A/C On/Off
         {
           AC = !AC;
         }
         else
         {
           if(request_temperature != 85)
             request_temperature++;
         }
      }
         break;
        
      case 4: //DOWN KEY PRESSED (Lower Setpoint) OR if MANU MODE set HEAT On/Off
      {
          if(system_status == 1) //if system is in manual mode set Heat On/Off
          {
            HEAT = !HEAT;
          }
          else  
          {
            if(request_temperature != 52)
              request_temperature--;
          } 
      }
         break;
        
      case 5: //RIGHT KEY PRESSED (MENU ADVANCE)
      {
         if(display_mode == 3)
           display_mode = 0;
         else
           display_mode++;
      }
        break;
    } 
}

void set_display()
{
  lcd.setCursor(0, 0);
  //lcd.print("SYS:");
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[14]));
  lcd.print(buffer);
  
  lcd.setCursor(4, 0);
  switch(system_status)
  {
    case 0:  //DISPLAY "OFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
          lcd.print(buffer);
          break;
    case 1:  //DISPLAY "MAN"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[16]));
          lcd.print(buffer);
          break;
    case 2:  //DISPLAY "AUT" 
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[17]));
          lcd.print(buffer);
          break;
    case 3:  //DISPLAY "EFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[18]));
          lcd.print(buffer);
          break;
  }
  lcd.setCursor(9, 0);
  
  //lcd.print("FAN:");
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[19]));
  lcd.print(buffer);
  
  lcd.setCursor(13, 0);
  
  switch(fan_status)
  {
    case 0: //DISPLAY "OFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
          lcd.print(buffer);   
          break;
    case 1: //DISPLAY "ON "
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[20]));
          lcd.print(buffer);
          break;
    case 2: //DISPLAY "AUT"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[18]));
          lcd.print(buffer);
          break;
   }
  
   lcd.setCursor(0,1);
  
//display mode for second line: 0-Indoor Temperature & Humidity,
//                              1-Indoor Temperature & Outdoor Temperature
//                              2-PPM/CF(RAW) & Air Quality
//                              3-Barometric Pressure & Weather Forcast
  if(request_temperature == prev_request)  
  {
    //display_mode BEFORE key pressed
    switch(display_mode) //first 13 columns (0-12) available for use
    {
      case 0:
      {
        lcd.print("IN:");
        lcd.setCursor(3,1);
        if(indoor_temp > 99)
          lcd.print(indoor_temp);
        else
        {
          lcd.print(indoor_temp);
          lcd.print(" ");        
        }
        lcd.setCursor(6,1);
        lcd.print((char)223);
        lcd.setCursor(7,1);
        lcd.print("F ");
        lcd.setCursor(9,1);
        lcd.print("HUM:");
        lcd.setCursor(13,1);
        lcd.print(indoor_humidity);
        lcd.setCursor(15,1);
        lcd.print("%");
      }
        break;
      case 1:
      {
        lcd.print("I:");
        lcd.setCursor(2,1);
        if(indoor_temp > 100)
          lcd.print(indoor_temp);
        else
        {
          lcd.print(indoor_temp);
          lcd.print(" ");        
        }
        lcd.setCursor(5,1);
        lcd.print((char)223);
        lcd.setCursor(6,1);
        lcd.print("F ");
        lcd.setCursor(9,1);
        lcd.print("O:");
        lcd.setCursor(11,1);
        if(metar_table[0].temperature > 99)
          lcd.print(metar_table[0].temperature);
        else
        {
          lcd.print(metar_table[0].temperature);
          lcd.print(" ");        
        }
        lcd.setCursor(14,1);
        lcd.print((char)223);
        lcd.setCursor(15,1);
        lcd.print("F");
        
       }
        break;
      case 2:
      {
        //DISPLAY PPM AND ASSOCIATED AQI
        //   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  |                                      |
        //  |______________________________________|
        //  |P P M : x x   A Q I  :     G  O  O  D |
        //  |______________________________________|
//        strcpy_P(buffer, (char*)pgm_read_word(&string_table[25]));
//        lcd.print(buffer);
//        lcd.setCursor(4,1);
//        lcd.print(ppm);
//        lcd.setCursor(7,1);
//        strcpy_P(buffer, (char*)pgm_read_word(&string_table[26]));
//        lcd.print(buffer);
//        
//        if(ppm < 150)
//        {
//          strcpy_P(buffer, (char*)pgm_read_word(&string_table[27]));
//          lcd.print(buffer);
//        }
//        else if(ppm < 300)
//        {
//          strcpy_P(buffer, (char*)pgm_read_word(&string_table[28]));
//          lcd.print(buffer);
//        }
//        else
//        {
//          strcpy_P(buffer, (char*)pgm_read_word(&string_table[29]));
//          lcd.print(buffer);
//        }
      }
        break;
      case 3:
      {
        lcd.print("P:");
        lcd.setCursor(2,1);
        if(metar_table[0].temperature > 999)
          lcd.print(metar_table[0].pressure);
        else
        {
          lcd.print(metar_table[0].pressure);
          lcd.print(" ");        
        }
        lcd.setCursor(6,1);
        lcd.print("mb");
	lcd.setCursor(8,1);
		if(press_change == 0)
			lcd.print("- ");
		if(press_change == 1)
			lcd.print("+ ");
		if(press_change == 2)
			lcd.print("  ");
        lcd.setCursor(10,1);  
        //conditions 0-CLEAR, 1-CHANGE, 2-STORMY 
        if(cond == 0) //PRINTS "CLEAR"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[21]));
          lcd.print(buffer);
        }
        if(cond == 1) //PRINTS "CHANGE"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[22]));
          lcd.print(buffer);
        }
        if(cond == 2) //PRINTS "STORMY"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[23]));
          lcd.print(buffer);
        }
      }
        break;
    }
  }
  else 
  {
    //lcd.print("Temp Set:");
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[24]));
    lcd.print(buffer);
    lcd.setCursor(9,1);
    lcd.print(request_temperature);
    lcd.setCursor(11,1);
    lcd.print((char)223);
    lcd.setCursor(12,1);
    lcd.print("F   ");
    delay(1000);
    prev_request = request_temperature;
  }
}

void lcd_loop()
{
  set_display();
  read_button();
}

