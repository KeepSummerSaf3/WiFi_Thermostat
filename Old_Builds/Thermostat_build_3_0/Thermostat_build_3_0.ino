/*
 * Digital SMART Thermostat sketch v3.0a  -- developed by GRANT OCONNOR 
 
	I could not have done this without the included 3rd party libraries and the arduino IDE
	developed by many, very intelligent and resourceful individuals.
	
		* DFRKey - developed for the DFRobot LCD with integrated, multiplexed, tact switches 
        DEFUNCT * AnalogMuxDemux - library that makes it much easier to uses multiplexers and demultiplexers
		* DHT22 - library for handling data from DHT22 humidity and temperature transducer.
			  This one was most definately a development cost saver.
	
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
	3. Allows for communication with noaa METARs to help improve 
	   system efficiency. METARs are retrieved, decoded and formatted on my server
	   to significantly reduce overhead and complication
 
 * Remember this is an alpha build for now so the reliability and functionality has not been proven
 * over long periods of time. Please forward any support email to me directly or through my webpage.
  
 * CONTACT INFORMATION:
 * NAME: GRANT OCONNOR
 * EMAIL: wenyuryaktalksback@gmail.com   ----- ya I know its stupid but likely this will be changed 
 * WEBPAGE: oconnors.dyndns-server.com/home ----- Hosted myself. I know its bare but I am just one person
 *                                                with a job and a mostly normal life so I don't have all
 *                                                day to build it. I don't wish to spend the money to make it
 *						  professional right now either.
 
 
 * Hardware System Description: included in separate document
*/

//libraries
#include <arduino.h>  //main arduino library
#include <WiServer.h> //ya this library at times was the biggest pain to deal with especially since it has
					  //been abandoned by the now defunct AsyncLabs. Its pretty big and uses the lion share 
					  //of flash memory. The Arduino WiFi library can not be used since the TCP/IP Stack is
					  //totally different
#include <LiquidCrystal.h> //LCD library
#include <DFR_Key.h> //library for integrated tact switches on LCD shield
//#include <Metro.h> //Timer library for some timed tasks
#include <DHT22.h> 

//--------------------
//--GLOBAL VARIABLES--
//--------------------

//STATIC DATA STORED IN FLASH
prog_char page_start[] PROGMEM =   {"<html><html><body><head>"}; 
prog_char line_break[] PROGMEM =   {"<br>"};
prog_char page_end[] PROGMEM =     {"</body></html>"};
prog_char page_heading[] PROGMEM = {"SMART THERMOSTAT v4.0"};
prog_char page_refresh[] PROGMEM = {"<meta http-equiv=\"refresh\" content=\"5\">"};
prog_char redirect[] PROGMEM = {"<meta http-equiv=\"refresh\" content=\"0;URL=\'home'\">"};

prog_char button_start[] PROGMEM = {"<button type = \"button\""};

prog_char button_sysauto[] PROGMEM = {"onclick=\"location.href=\'sysauto\'\"> AUTO</button>"};
prog_char button_sysoff[] PROGMEM = {"onclick=\"location.href=\'sysoff\'\"> OFF</button>"};
prog_char button_syseff[] PROGMEM = {"onclick=\"location.href=\'syseff\'\"> EFF</button>"};

prog_char button_increase_temp[] PROGMEM = {"onclick=\"location.href=\'temp_inc\'\"> TEMP +</button>"};
prog_char button_decrease_temp[] PROGMEM = {"onclick=\"location.href=\'temp_dec\'\"> TEMP -</button>"};

prog_char indoor_temp_graph[] PROGMEM = {"<img src=\"https://api.cosm.com/v2/feeds/99111/datastreams/0.png?"};
prog_char end_img_src[] PROGMEM = {"width=730&height=250&colour=F15A24&timezone=UTC\">"};

//STRINGS FOR LCD
prog_char sys_[] PROGMEM = {"SYS:"};
prog_char off_[] PROGMEM = {"OFF"}; 
prog_char auto_[] PROGMEM = {"AUT"};
prog_char sys_manual[] PROGMEM = {"MAN"};
prog_char sys_efficiency[] PROGMEM = {"EFF"};

prog_char fan[] PROGMEM = {"FAN:"};
//prog_char fan_off[] PROGMEM = {"OFF"};  <---RESUSED FROM ABOVE
prog_char fan_on[] PROGMEM = {"ON "};
//prog_char fan_auto[] PROGMEM = {"AUT"}; <---REUSED FROM ABOVE

prog_char barom_stormy[] PROGMEM = {"STORMY"};
prog_char barom_clear[] PROGMEM =  {"CLEAR "};
prog_char barom_change[] PROGMEM = {"CHANGE"};

prog_char temp_set[] PROGMEM = {"Temp Set:"};

prog_char ppm[] PROGMEM = {"PPM:"};
prog_char AQI[] PROGMEM = {"AQI: "};
prog_char AQI_good[] PROGMEM =  {"GOOD"};
prog_char AQI_normal[] PROGMEM ={"NORM"};
prog_char AQI_poor[] PROGMEM =  {"POOR"};

prog_char name_[] = {"SMART THERMOSTAT"};
prog_char sys_version[] = {"v6.0a"};

prog_char w_messg1[] = {"AQUIRING"};
prog_char w_messg2[] = {"WIRELESS"};

PROGMEM const char *string_table[] = 
{
   page_start,  //0
   line_break,  //1
   page_end,    //2
   page_heading, //3
   
   button_start, //4
   
   button_sysoff, //5
   button_sysauto, //6
   button_syseff, //7
   
   button_increase_temp, //8
   button_decrease_temp, //9
   
   page_refresh, //10
   redirect, //11
   
   indoor_temp_graph, //12
   end_img_src,  //13
   
   
   //STRINGS FOR LCD
   sys_,  //14
   off_, //15
   auto_, //16
   sys_manual, //17
   sys_efficiency, //18
   
   fan,  //19
   fan_on, //20
   
   barom_stormy, //21
   barom_clear,  //22
   barom_change, //23
   
   temp_set,  //24
   
   ppm,      //25
   AQI,      //26
   AQI_good,  //27
   AQI_normal, //28
   AQI_poor, //29
   
   name_,      //30
   sys_version, //31
   
   w_messg1, //32
   w_messg2, //33
   
};

char buffer[65]; //resize accordingly to the size of the largest string above

//Metro timer/counter (1s resolution)
//Metro timer = Metro(1000);
uint8 five_min_cnt = 0; //5 minute timer for smart functions and pachube request
unsigned int metar_server_cnt = 0; //60 minute timer for metar data retrieval

//locks and other stuff
boolean data_lockout = true; //lock for when to process data

//sensor variables
byte indoor_temp = 70;
byte indoor_humidity = 20;
byte press_change = 2; //indicates +/- on LCD for pressure change
byte cond = 0;  //conditions 0-CLEAR, 1-CHANGE, 2-STORMY 
byte out_temp_change = 0; //indicates temperature change (0 - Steady, 1- Rising, 2- Falling

boolean vacant = false; //house is empty or not empty

//----dust sensor parameters
//float ppm=0; 
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


//lcd section gloabal variables

//timout period for lcd panel
//byte sleep_countdown = 120;
//value for analog read
//uint8 read_val = 1023;

//temperature request
byte request_temperature = 73;
byte prev_request = 74;

//select button
boolean select_record = 0;

//fan status: 0-OFF, 1-ON, 2-AUTO
byte fan_status = 0; 

//system status: 0-SYS OFF, 1-MANU, 2-AUTO, 3-EFF (SMART Efficiency Mode)
byte system_status = 0;

//air conditioner settings
boolean AC = false;
boolean HEAT = false;

//display mode for second line: 0-Temperature & Humidity, 1-PPM/CF(RAW) & Air Quality
//                              2-Inside Temperature & Outside Temperature
//                              3-Barometric Pressure & Weather Forcast
byte display_mode = 0;


unsigned long time = 0;
//-----------------------END GLOBAL VARIABLES-----------------------------


//-------------------------SETUP FOR PROGRAM---------------------------
void setup()
{
  //Debugging only - start serial connection and set bode rate
  Serial.begin(9600);
  
  metar_table[0].temperature = 75;
  metar_table[1].temperature = 75;
  metar_table[0].pressure = 1030;
  metar_table[1].pressure = 1030;
  
  setup_lcd();
  Serial.println("LCD Setup Complete");
  wireless_setup();
  Serial.println("Wireless Setup Complete");
  
  pinMode(13, OUTPUT); // DUCT FAN RELAY PIN
  pinMode(12, OUTPUT); // AIR CONDITIONER RELAY PIN
  pinMode(11, OUTPUT); // HEATER RELAY PIN
 
  time = millis();   
}

//---------------------------MAIN PROGRAM LOOP----------------------------
void loop()
{
  
  if(time > millis())
    time = millis();
  
  if((millis() - time) > 1000)
  {
      five_min_cnt++; //5 minute timer for smart functions and pachube request
      Serial.print("Timer1: ");
      Serial.println(five_min_cnt);
      metar_server_cnt++; //60 minute timer for metar data retrieval
      Serial.print("Timer2: ");
      Serial.println(metar_server_cnt);
        
      time = millis();
  }
  wireless_loop();
//  sensor_loop();
  if(five_min_cnt > 254) //5 minute timer
  {
    //smart_functions();
    five_min_cnt = 0;
  }
//  if(!data_lockout)
//    process_data();
//    
//  relay_control();
//  lcd_loop();
    
//  if(timer.check() == 1)
//  {
//    read_val = analogRead(A0);
//    //Serial.print("READ VAL: ");
//    //Serial.println(read_val);
//    
//    if((read_val == 1023 || read_val == 255) && sleep_countdown == 0)
//    {
//      digitalWrite(10, LOW);
//      sleep_countdown = 0;
//    }
//    if((read_val == 1023 || read_val == 255) && sleep_countdown > 1)
//    {
//      sleep_countdown--;
//      //Serial.print("SLEEPING: ");
//      //Serial.println(sleep_countdown);
//    }
//    if(read_val != 1023 && read_val != 255)
//    {
//      digitalWrite(10, HIGH);
//      sleep_countdown = 120;
//      //Serial.println("INTERRUPTED");
//    }
//  }
 
  delay(10);
  
}
