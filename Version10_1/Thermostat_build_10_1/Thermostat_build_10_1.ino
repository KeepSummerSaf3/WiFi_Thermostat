/*
 * Digital SMART Thermostat sketch v8.0a  -- developed by GRANT OCONNOR 
 
	I could not have done this without the included 3rd party libraries and the arduino IDE
	developed by many, very intelligent and resourceful individuals.

	        * WiServer Library - developed by the now defunct AsyncLabs but is required for the Microchip Wifi
                                     module on this board for 802.11b protocols.
		* DFRKey - developed for the DFRobot LCD with integrated, multiplexed, tact switches 
       		* DHT22 - library for handling data from DHT22 humidity and temperature transducer.
			  This one was most definately a development cost saver.
	        * LCD_SR - library that extends the functions of the core arduino LCD library for using a shift register 
                           to increase pin availability for other uses.
	Of course much thanks to the folks developing at arduino for their libraries
 
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
 *  Bottom Line this is open source so please feel free to modify and make changes as you please although, I
        hope you give some credit for the moderate difficulty in development of this system.
 
 *  It is also probably best noted here that this uses a modified Duemillanove board from Linksprite.
	The Microchip WiFi module on this board is not compatible with the Arduino Wifi Library at all.
	I plan to expand the hardware in a couple of different models but as for now this is the module I
	will be using as the base platform. 
 
    Some of the Goals of the Project Include:
	1. Digital touch display for control of HVAC system (fan and temperature)
	2. Transducers attached to arduino for indoor temperature, humidity, particulates, and airflow
	3. Allows for data retrieval using local METAR weather reports to assist in efficiency. 
           Data retrieved includes outside temperature and outside barometric pressure from closest airfield.
           METARs are retrieved, decoded and formatted on my server to significantly reduce overhead and complication.
           This is one major aspect of the system.
        4. Low cost and flexible. Analog user feedback (LCD and pushbuttons) can be removed from the system while conrol
           can be done entirely from from a web interface.
 
 * Remember this is an alpha build for now so the reliability and functionality has not been proven
 * over long periods of time. Please forward any support email to me directly or through my webpage.
  
 * CONTACT INFORMATION:
 * NAME: GRANT OCONNOR
 * EMAIL: wenyuryaktalksback@gmail.com   ----- ya I know its stupid but likely this will be changed 
 * WEBPAGE: oconnors.dyndns-server.com/home ----- Hosted myself. I know its bare but I am just one person
 *                                                with a job and a mostly normal life so I don't have all
 *                                                day to build it. I don't wish to spend the money to make it
 *						  professional right now either.
 
 
Stable Release version changelog:

Complete change of board to allow for easier programming and easier to use communication API.

*/
//Main libraries
#include <arduino.h>  //main arduino library
#include <Ethernet.h> //ethernet library
#include <SD.h>       //sd card library
#include <SPI.h>      //arduino interconnect library

//LCD library
#include <LiquidCrystal_SR.h> //LCD library for use with shift register
//Keyboard library
#include <DFR_Key.h> //library for integrated tact switches on LCD shield

//Sensor Library
#define DHT22_NO_FLOAT
#include <DHT22.h> 

//Clock Library
#include <Time.h>

#define backlight 8
#define HEAT_PIN 3
#define AC_PIN   8
#define FAN_PIN  9
//--------------------
//--GLOBAL VARIABLES--
//--------------------

//STRINGS FOR LCD
prog_char off_[] PROGMEM = {"OFF"}; 
prog_char auto_[] PROGMEM = {"AUT"};

// ORIGINALLY SET to 9 but may be able to use as pointer
//resize accordingly to the size of the largest string above
char buffer[31]; 

//reusable incrementor
byte i=0;

boolean swap = true; //swap for AQI and PPM line

//sensor variables
byte indoor_temp = 73;
byte indoor_humidity = 50;
byte press_change = 2; //indicates +/- on LCD for pressure change
byte cond = 0;  //conditions 0-CLEAR, 1-CHANGE, 2-STORMY 
byte out_temp_change = 0; //indicates temperature change (0 - Steady, 1- Rising, 2- Falling

int temp_diff = 0;
//int press_diff = 0;

//----------------------USER DEFINABLE DATA-------------------------
//user definable data that will be set in web server

byte temp_deadband_val; //deadband value (set to 255 if you dont want deadband)
byte heat_cutoff_val; //(only 0 to 100 accepted)
byte ac_cutoff_val; //(only 0 to 100 accepted)
byte humidity_cutoff_val; //(only 0 to 100 accepted)
bool disable_heat; //disables the heater
bool disable_ac; //disables the ac
bool disable_humidifier; //disables the humidifer
bool disable_fan; //disables the fan
//boolean vacant = false; //house is empty or not empty

//DATA STORAGE for TIME TABLE

// event time hour[0], event time minute[1], request temperature[2]
uint8_t schedule_data[3];            

//-----------------------END USER DEFINABLE DATA-------------------------------

//----dust sensor parameters
//float ppm=0; 
int ppm_val = 0;
//float dustdensity = 0;
//float ppmpercf = 0;
//----

//wireless section global variables

//DATA STORAGE FOR METAR DATA
typedef struct METAR_DATA
{
  int temperature;
  int pressure;
  
} METAR_DATA;
struct METAR_DATA metar_table[2];

char DOW_[] = "SUN";
char MONTH_[] = "JAN";
//lcd section gloabal variables

//temperature request
byte request_temperature = 73;
byte prev_request = 74;

//fan status: 0-OFF, 1-ON, 2-AUTO
byte fan_status = 0; 

//system status: 0-SYS OFF, 1-MANU, 2-AUTO (Smart Weather Mode)
byte system_status = 0;
byte prev_system_status = 0;

//logic for AC and HEAT
boolean AC = false;
boolean HEAT = false;

//display mode for second line: 0-Temperature & Humidity
//                              1-Inside Temperature & Outside Temperature
//                              2-Air Quality
//                              3-Barometric Pressure & Weather
byte display_mode = 0;

File sdfile_descrpt;
//-----------------------END GLOBAL VARIABLES-----------------------------

//-------------------------SETUP FOR PROGRAM---------------------------
void setup()
{
  //Debugging only - start serial connection and set bode rate
  //Serial.begin(9600);
  //Serial.println("Serial line init");
  
  //initialize the Wiznet and SD card hardware
  setup_sd();
  //Serial.println("SD Card Data OK");
  setup_relays();
  //Serial.println("Relays Configured");
  setup_lcd();
  //Serial.println("LCD Setup Complete");
  wired_setup();
  //Serial.println("Ethernet Setup Complete");
  delay(1000);  
  metar_table[0].temperature = 75;
  metar_table[1].temperature = 75;
  metar_table[0].pressure = 1030;
  metar_table[1].pressure = 1030; 
  //Serial.println(freeRam());
}

//---------------------------MAIN PROGRAM LOOP----------------------------
//int freeRam () {
//  extern int __heap_start, *__brkval; 
//  int v; 
//  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
//}

void loop()
{
  wired_loop();
  sensor_loop();
  //process_data(); 
  
  if((minute() % 5 == 0) && (second() == 0))
  {
    if(analogRead(A0) == 1023)
      digitalWrite(backlight, HIGH);
    
    smart_functions();   //inside intelligence
  }
  
  //if(system_status == 3 && (minute() == 30 || minute() == 0) && second() < 5)
  //  read_schedule();
  
  relay_control();
  lcd_loop(); 
  
  if(analogRead(A0) != 1023)
    digitalWrite(backlight, LOW);

}


