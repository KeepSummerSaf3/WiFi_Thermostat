/*
 * Digital SMART Thermostat sketch v7.0a  -- developed by GRANT OCONNOR 
 
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

1.Added DateTime library to the system so this will expand functionality to allow setting timed actions for HVAC
2.Added displayed date and time to the lcd output.
3.Major upgrade to timing system so everything is synced correctly now.
  Time drift can still occur (couple of seconds each day) but the clock will be synced at regular intervals


* Hardware System Description: included in separate document
*/

//libraries
#include <arduino.h>  //main arduino library
#include <WiServer.h> //ya this library at times was the biggest pain to deal with especially since it has
					  //been abandoned by the now defunct AsyncLabs. Its pretty big and uses the lion share 
					  //of flash memory. The Arduino WiFi library can not be used since the TCP/IP Stack is
					  //totally different
#include <LiquidCrystal_SR.h> //LCD library for use with shift register
#include <DFR_Key.h> //library for integrated tact switches on LCD shield
#include <DHT22.h> 
#include <Time.h>
#include <TimerOne.h>
#include <math.h>

#define backlight 10
#define HEAT_PIN 4
#define AC_PIN   2
#define FAN_PIN  3
//--------------------
//--GLOBAL VARIABLES--
//--------------------

//STATIC DATA STORED IN FLASH
prog_char page_start[] PROGMEM =   {"<html><html><body><head>"}; 
prog_char line_break[] PROGMEM =   {"<br>"};
prog_char page_end[] PROGMEM =     {"</body></html>"};
prog_char page_heading[] PROGMEM = {"SMART THERMOSTAT v7"};
prog_char page_refresh[] PROGMEM = {"<meta http-equiv=\"refresh\" content=\"5\">"};
prog_char redirect[] PROGMEM = {"<meta http-equiv=\"refresh\" content=\"0;URL=\'home'\">"};

prog_char button_start[] PROGMEM = {"<button type = \"button\""};

prog_char button_sysauto[] PROGMEM = {"onclick=\"location.href=\'sysauto\'\"> AUTO</button>"};
prog_char button_sysoff[] PROGMEM = {"onclick=\"location.href=\'sysoff\'\"> OFF</button>"};
prog_char button_syseff[] PROGMEM = {"onclick=\"location.href=\'syseff\'\"> EFF</button>"};

prog_char button_increase_temp[] PROGMEM = {"onclick=\"location.href=\'temp_inc\'\"> TEMP +</button>"};
prog_char button_decrease_temp[] PROGMEM = {"onclick=\"location.href=\'temp_dec\'\"> TEMP -</button>"};

prog_char indoor_temp_graph[] PROGMEM = {"<img src=\"https://api.cosm.com/v2/feeds/99111/datastreams/0.png?&title=Indoor%20Temperature"};
prog_char indoor_humid_graph[] PROGMEM = {"<img src=\"https://api.cosm.com/v2/feeds/99111/datastreams/1.png?&title=Indoor%20Humidity"};

prog_char end_img_src[] PROGMEM = {"&width=730&height=200&colour=F15A24&show_axis_labels=true&timezone=UTC\">"};

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

prog_char AC_LCD[] PROGMEM =   {"AC"};
prog_char HEAT_LCD[] PROGMEM = {"HEAT"};

prog_char form_start[] PROGMEM = {"<form name=\"input\" method=\"get\"action=\">"};
prog_char prog_form1[] PROGMEM = {"\"Temp Deadband:<input type=\"text\"name=\"deadband\">F<br>Heater Cutoff Temp:<input type"};
prog_char prog_form2[] PROGMEM = {"\"text\"name=\"heat_cutoff\">F<br>AC Cutoff Temp:<input type=\"text\"name=\"ac_cutoff\">F<br>"};
prog_char prog_form3[] PROGMEM = {"Humidity Cutoff:<input type=\"text\"name=\"humid_cutoff\">%<br>Vacancy Temp Control:<input ty"};
prog_char prog_form4[] PROGMEM = {"pe=\"radio\" name=\"vacant\"value=\"Yes\"><br><input type=\"submit\"value=\"Submit\"></form>"};

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
   
   //additions to graphs for v4.2
   indoor_humid_graph, //30
   
   //additions to AC/HEAT on display v4.2
   AC_LCD,    //31
   HEAT_LCD,   //32
   
   form_start, //33
   prog_form1, //34
   prog_form2, //35
   prog_form3, //36
   prog_form4 //37
};

char buffer[92]; //resize accordingly to the size of the largest string above

boolean swap = true; //swap for AQI and PPM line

//data lockout to process when updated data is actually received from server
boolean data_lockout = true;

//sensor variables
byte indoor_temp = 73;
byte indoor_humidity = 50;
byte press_change = 2; //indicates +/- on LCD for pressure change
byte cond = 0;  //conditions 0-CLEAR, 1-CHANGE, 2-STORMY 
byte out_temp_change = 0; //indicates temperature change (0 - Steady, 1- Rising, 2- Falling

int temp_diff = 0;
int press_diff = 0;

//user definable data that will be set in web server
byte temp_deadband_val = 3; //deadband value (set to 255 if you dont want deadband)
byte heat_cutoff_val = 60; //(only 0 to 100 accepted)
byte ac_cutoff_val = 60; //(only 0 to 100 accepted)
byte humity_cutoff_val = 60; //(only 0 to 100 accepted)
boolean vacant = false; //house is empty or not empty


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

//timout period for lcd panel
byte sleep_countdown = 120;

//temperature request
byte request_temperature = 73;
byte prev_request = 74;

//select button
boolean select_record = 0;

//fan status: 0-OFF, 1-ON, 2-AUTO
byte fan_status = 0; 

//system status: 0-SYS OFF, 1-MANU, 2-AUTO, 3-EFF (SMART Efficiency Mode)
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

//-----------------------END GLOBAL VARIABLES-----------------------------

//-------------------------SETUP FOR PROGRAM---------------------------
void setup()
{
  //Debugging only - start serial connection and set bode rate
  //Serial.begin(9600);
  
  metar_table[0].temperature = 75;
  metar_table[1].temperature = 75;
  metar_table[0].pressure = 1030;
  metar_table[1].pressure = 1030;
  
  //delay(10000);
  setup_relays();
  //Serial.println("Relays Configured");
  setup_lcd();
  //Serial.println("LCD Setup Complete");
  wireless_setup();
  //Serial.println("Wireless Setup Complete");
  setTime(1,0,0,1,11,70);
  
  Timer1.initialize(1000000); //1 Hz timer
  Timer1.attachInterrupt(display_timeout);
  
}

//---------------------------MAIN PROGRAM LOOP----------------------------
void loop()
{
  wireless_loop();
  sensor_loop();
  
  if((minute() % 5 == 0) && (second() == 0))
  {
    smart_functions();   //inside intelligence
    ppm_val = 0;
  }
   
  if(!data_lockout)  //<-- maybe just do procssing as part of receiving data loop so I can avoid using this flag? 
    process_data;    //(NO!!!! WEIRD DATA STREAM PROBLEM FROM SERVER, THIS IS BEST WORKAROUND)
    
  relay_control();
  lcd_loop();
    
}

void display_timeout()
{
  if((analogRead(A0) == 1023) && (sleep_countdown < 1))
  {
    digitalWrite(backlight, HIGH);
    sleep_countdown = 0;
    //Serial.print("ASLEEP");
  }
  if((analogRead(A0) == 1023)  && (sleep_countdown > 0))
  {
    sleep_countdown--;
    //Serial.print("SLEEP IN: ");
    //Serial.println(sleep_countdown);
  }
  if(analogRead(A0) != 1023)
  {
    digitalWrite(backlight, LOW);
    sleep_countdown = 180;
    //Serial.println("INTERRUPTED");
  }
  
  ppm_val++; //just for fun here
}

