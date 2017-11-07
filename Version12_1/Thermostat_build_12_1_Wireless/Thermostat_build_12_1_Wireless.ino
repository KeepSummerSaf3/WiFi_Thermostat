
/*
 * Digital SMART Thermostat sketch v12a  -- developed by GRANT OCONNOR 
 
	I could not have done this without the included 3rd party libraries and the arduino IDE
	developed by many, very intelligent and resourceful individuals.

	  * WiServer Library - developed by the now defunct AsyncLabs but is required for the Microchip Wifi module on this board for 802.11b protocols.
		* DFRKey - developed for the DFRobot LCD with integrated, multiplexed, tact switches 
    * DHT22 - library for handling data from DHT22 humidity and temperature transducer.
	  * LCD_SR - library that extends the functions of the core arduino LCD library for using a shift register to increase pin availability for other uses.
	
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
 *                                                professional right now either.
 
 
Stable Release version changelog:

1.Added DateTime library to the system so this will expand functionality to allow setting timed actions for HVAC
2.Added displayed date and time to the lcd output.
3.Major upgrade to timing system so everything is synced correctly now.
  Time drift can still occur but the RTC will be synced at regular intervals with METAR server.


* Hardware System Description: included in separate document*
*/


/*-----------ORIGINAL IO Map of Linksprite Diamondback------------- ///TO BE DELETED AFTER TESTING//////

//--------------PINOUT DIAGRAM------------------
                      |-------------------|
                      |             AREF  | 
                      |             GND   |
                      |             D13   |  - 
                      | RST         D12   |  - Reserved for Diamondback Wifi LED
                      | 3.3V        D11   |  - LCD Backlight Control
                      | 5V          D10   |  - Reserved for Diamondback Wifi Control
                      | GND         D09   |  - Dust Sensor LED Pin Control
                      | GND         D08   |  - Proximity Sensor
                      | VIN         D07   |  - A/C ON/OFF RELAY Latch Output
                      |             D06   |  - HEAT ON/OFF RELAY Latch Output
     DFR Key Input  - | A0          D05   |  - FAN ON/OFF RELAY Latch Output
DHT22 Sensor Input  - | A1          D04   |  - LCD RCK/ENABLE ---> SR/LCD Enable Pin - (PIN 12)
 Dust Sensor Input  - | A2          D03   |  - LCD DATA ----> SR DATA Pin - (PIN 14)
                      | A3          D02   |  - LCD CLOCK ---> SR CLOOCK Pin - (PIN 11)
                      | A4          TX    |  - Reserved for Serial
                      |_A5__________RX____|  - Reserved for Serial
                  
*/

/*-----------Simplified IO Map of Linksprite Diamondback-------------

//--------------PINOUT DIAGRAM------------------
                       |-------------------|
                       |             AREF  | 
                       |             GND   |
                       |             D13   |  
                       | RST         D12   |  - Reserved for Diamondback Wifi LED
                       | 3.3V        D11   |  - Reserved for Ethernet
                       | 5V          D10   |  - Reserved for Diamondback Wifi Control
                       | GND         D09   |  - LCD Backlight Control 
                       | GND         D08   |  - LCD DATA ----> SR DATA Pin - (Plain White Wire)(PIN 14)
                       | VIN         D07   |  - LCD CLOCK ---> SR CLOCK Pin - (Brown Wire) (PIN 11)
                       |             D06   |  - LCD RCK/ENABLE ---> SR/LCD Enable/Latch Pin - (Brown/White Wire) (PIN 12)
Pressure Transmitter - | A0          D05   |  - RLY DATA ----> SR DATA Pin - (Plain White Wire) (PIN 14)
     Cap Sense EStop - | A1          D04   |  - Reserved for MicroSD Card (SDCS) - NOT IN USE
    Cap Sense Select - | A2          D03   |  - RLY CLOCK ---> SR CLOCK Pin - (Orange Wire) (Pin 11)
   Cap Sense Up/Down - | A3          D02   |  - RLY RCK/ENABLE ---> SR/LCD Enable/Latch Pin - (Orange/White Wire) (Pin 12)
                     - | A4          TX    |  - Reserved for Serial
Volt. Ref. (Internal)- |_A5__________RX____|  - Reserved for Serial
                  
*/
//--------------------
//-----LIBRARIES------
//--------------------

#include <arduino.h>  //main arduino library
#include <WiServer.h> //ya this library at times was the biggest pain to deal with especially since it has
		      //been abandoned by the now defunct AsyncLabs. It uses the lion share of the available 32K flash memory.
                      //The Arduino WiFi library can not be used since the TCP/IP Stack is totally different.
#include <avr/pgmspace.h> //changes for using Flash memory in updated Arduino Libraries
//LCD librarys
#include <LiquidCrystal_SR.h> //LCD library for use with shift register
//#include <DFR_Key.h> //library for integrated tact switches on LCD shield

//Sensor Librarys
#define DHT22_NO_FLOAT
#include <DHT.h> 

//Clock and Timer Libraries
#include <TimeLib.h>
#include <TimedAction.h>

//General Purpose Shift Register library
#include <Shifty.h>

//PID Library
//#include <PID_v1.h>

//--------END LIBRARY LIST---------


//--------------------
//DEFINES
//--------------------
#define AC_RELAY 0 
#define HEAT_RELAY 1 
#define FAN_RELAY 2

//Relay Shift Register
#define RelaySR_Clock_Pin 3
#define RelaySR_Latch_Pin 2
#define RelaySR_Data_Pin 5

//Backlight Control Pin
#define BACKLIGHT_PIN 9

//LCD Control Pins
#define LCD_DATA_PIN  8
#define LCD_CLOCK_PIN  7
#define LCD_ENABLE_PIN  6


#define MAX_RELAY_TIME 30

#define WEATHER_DATA_LEN 7

//--------END DEFINES LIST---------
//---------------------
//FUNCTION DECLARATIONS
//---------------------

//Calculations
void calc_relative_humidity();

//Relay Control
void RelaySR_Setup();
void relay_shutdown();
void relay_on(byte rly);
void relay_off(byte rly);
void relay_main_control();
void ac_control(bool);
void heat_control(bool);
void fan_control(bool);

//Free Ram
int freeRam();

//Sensors
void Dust_read();
void DHT_read();

//Wireless
byte month_conv(char pos1, char pos2, char pos3);
void wireless_setup();
void wireless_loop();
void thingspeak_loop();
void metar_loop();
void METAR_data_feed();
void TS_data_feed();
void processThingspeakData(char *, int );
void processMETARServerData(char * , int );
boolean sendPage(char* URL); 

void process_weather(char *);
void process_humidity(char *);
void process_baropress(char *);
void process_wind(char *);
void process_temperature(char *);
void process_op_data(char *);

//PID Control
//void PIDsetup();
//void PIDLoops();

//LCD and Control Interface
void display_logic();
void set_display_freemem();
void set_display_system_state();
void set_display_fan_state();
void set_display_time();
void set_display_th(byte, byte, bool);
void set_display_aqi();
void set_display_press();
void set_display_setpoint();
void set_display(char *, uint8);
void clear_line(boolean);
void read_button();
void lcd_loop();
void check_freeRam();
void increment_display_mode();

//Reset Function
void software_reset();

//Test Function
void data_readout();

//--------------------
//--GLOBAL VARIABLES--
//--------------------

//TIMED Actions Objects
TimedAction WiFi_ThingspeakQuery = TimedAction(600000, thingspeak_loop);
TimedAction WiFi_MetarQuery = TimedAction(3600000, metar_loop);
TimedAction DHT_Sensor_Read = TimedAction(30000, DHT_read);
TimedAction DisplayUpdate = TimedAction(2000, display_logic);
//TimedAction Dust_Sensor_Read = TimedAction(60000, Dust_read);
TimedAction FreeMem_Display = TimedAction(30000, check_freeRam);
TimedAction ReadOut_Variables = TimedAction(30000, data_readout);

//Shift Register Object
Shifty relaySRSingle;

uint16 i; //reusable counter

//user definable data that will be set in web server
//byte temp_deadband_val = 3; //deadband value (set to 0 if you dont want deadband)
//byte heat_cutoff_val = 70; //(only 0 to 100 accepted)
//byte ac_cutoff_val = 60; //(only 0 to 100 accepted)
//byte humidity_cutoff_val = 60; //(only 0 to 100 accepted)
//byte upper_bound_temperature = 50; //upper boundary temperature - upper limit for action
//byte lower_bound_temperature = 85; //lower boundary temperature - lower limit for action

//DATA FOR VARIABLEES

// "prev" is the previous status, "curr" is for current status

byte curr_setpoint_temp = 75; //curr setpoint temperature (request temperature)
byte prev_setpoint_temp = 75; //prev setpoint temperature

byte indoor_temp = 75; //in deg F
byte indoor_humid = 50; //(relative humidity) in % 
byte outdoor_temp = 75; //in deg F
byte outdoor_humid = 50; //(relative humidity) in %
uint16 outdoor_press = 1001; //in Millibars

boolean vacant = 0;

uint8 wind_speed = 0; //in MPH
char wind_direction[3] = {"XXX"}; //Wind Direction
uint8 ppm = 0; //parts per million (dust)

char weather_data[WEATHER_DATA_LEN] = {"CLEAR"};

bool ac_request = 0; //User request for A/C (no memory required)
bool prev_ac_request = 0;
bool heat_request = 0; //User request for HEAT (no memory required)
bool prev_heat_request = 0;
bool fan_mode_request = 0; //User fan mode request: 0-OFF, 1-ON
bool prev_fan_mode_request = 0;
bool system_state = 0; //system status: 0-SYS OFF, 1-SYS ON

uint16 ac_temp_time; //temporary time register
uint16 heat_temp_time; //tempoerary time register

//uint8_t backlight = 0x80;  //either 0x00 or 0x80   (B1000 0000)

//-----------------------END GLOBAL VARIABLES-----------------------------

