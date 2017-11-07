/*
 * Digital SMART Thermostat sketch v3.0a  -- developed by GRANT OCONNOR 
 
	I could not have done this without the included 3rd party libraries either that
	were developed by many people very intelligent and resourceful individuals.
	
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
	
 *	Bottom Line this is open source so please feel free to modify and make changes as you please although, I
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
 * WEBPAGE: oconnors.dyndns-server.com/home ----- hosted myself I know its bare but I am just one person
 *                                                with a job and a mostly normal life so I don't have all
 *                                                day to build it. I don't wish to spend the money to make it
 *												  professional right now either.
 
 
 * Hardware System Description: included in separate document
*/

//libraries
#include <arduino.h>  //main arduino library
#include <WiServer.h> //ya this library at times was the biggest pain to deal with especially since it has
					  //been abandoned by the now defunct AsyncLabs. Its pretty big and uses the largest amount 
					  //of my modules memory. The Arduino WiFi library can not be used since the TCP/IP Stack is
					  //totally different than the units that come as separate shields
#include <LiquidCrystal.h> //LCD library
#include <DFR_Key.h> //library for integrated tact switches on LCD shield
#include <Metro.h> //Timer library for some timed tasks
#include <DHT22.h> 

//--------------------
//--GLOBAL VARIABLES--
//--------------------

//sensor variables
int indoor_temp = 70;
int indoor_humidity = 20;
byte press_change = 0;
String cond = "CLEAR";

//----dust sensor parameters
float ppm=0; 
float dustdensity = 0;
float ppmpercf = 0;
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

//temperature request
byte request_temperature = 68;
byte prev_request;

//select button
boolean select_record = 0;

//fan status: 0-OFF, 1-ON, 2-AUTO
byte fan_status = 0; 

//system status: 0-SYS OFF, 1-MANU, 2-AUTO
byte system_status = 0;

//air conditioner settings
boolean AC = false;
boolean HEAT = false;

//display mode for second line: 0-Temperature & Humidity, 1-PPM/CF(RAW) & Air Quality
//                              2-Inside Temperature & Outside Temperature
//                              3-Barometric Pressure & Weather Forcast
byte display_mode = 0;

//-----------------------END GLOBAL VARIABLES-----------------------------

//-----------------------START LCD SECTION--------------------------------
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

//*NOTE MOST GLOBAL VARIABLES ARE IN MAIN HEADER FILE

//local parameters for Keypad
DFR_Key keypad;
int localKey = 0;

//Metro timer/counter for dimmer (1s resolution)
Metro dimmer_timer = Metro(500);

void setup_lcd() {
  
  //set custom characters  
  //pinMode(10, OUTPUT);
  //digitalWrite(10, HIGH);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Thermostat v3.0a");
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
      switch(select_record)
      {
        case 0:
        {
          if(system_status == 0) //if auto is set for system then auto is set for fan
            fan_status = 0;
          else if(system_status == 2)
            fan_status = 2;
          else
          {
            if(fan_status == 0)
              fan_status = 1;
            else
              fan_status = 0;
           }
        }
          break;
        case 1:
        {
           if(system_status == 2)
             system_status = 0;
           else
             system_status++;
             
            //update system operations as needed here
            if(system_status == 0)
              fan_status = 0;
   
            if(system_status == 2)
              fan_status = 2;
        }
          break;
      }
        //else maybe show system is off or in manual mode
     }
       break;
     case 3: //UP KEY PRESSED (Raise Setpoint)
     {
          if(request_temperature != 85)
            request_temperature++;
     }
        break;
        
      case 4: //DOWN KEY PRESSED (Lower Setpoint)
      {
            if(request_temperature != 55)
              request_temperature--;   
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
  lcd.print("SYS:");
  lcd.setCursor(4, 0);
  switch(system_status)
  {
    case 0:  lcd.print("OFF ");
          break;
    case 1:  lcd.print("MAN");
          break;
    case 2: lcd.print("AUT");
          break;
  }
  lcd.setCursor(9, 0);
  lcd.print("FAN:");
  lcd.setCursor(13, 0);
  
  switch(fan_status)
  {
    case 0:   lcd.print("OFF ");
          break;
    case 1:   lcd.print("ON  ");
           break;
    case 2:    lcd.print("AUT");
           break;
   }
  
   lcd.setCursor(0,1);
  
//display mode for second line: 0-Indoor Temperature & Humidity,
//                              1-Indoor Temperature & Outdoor Temperature
//                              2-PPM/CF(RAW) & Air Quality
//                              3-Barometric Pressure & Weather Forcast
  if(request_temperature == prev_request)
  {
    switch(display_mode) //first 13 columns (0-12) available for use
    {
      case 0:
      {
        lcd.print("IN:");
        lcd.setCursor(3,1);
        lcd.print(indoor_temp);
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
        lcd.print(indoor_temp);
        lcd.setCursor(5,1);
        lcd.print((char)223);
        lcd.setCursor(6,1);
        lcd.print("F ");
        lcd.setCursor(9,1);
        lcd.print("O:");
        lcd.setCursor(11,1);
        lcd.print(metar_table[0].temperature);
        lcd.setCursor(14,1);
        lcd.print((char)223);
        lcd.setCursor(15,1);
        lcd.print("F");
       }
        break;
      case 2:
      {
        lcd.print("PPM:xx ");
        lcd.setCursor(7,1);
        lcd.print("AQI:GOOD  ");
      }
        break;
      case 3:
      {
        lcd.print("P:");
        lcd.setCursor(2,1);
        lcd.print(metar_table[0].pressure);
        lcd.setCursor(6,1);
        lcd.print("mb");
		lcd.setCursor(8,1);
		if(press_change == 0)
			lcd.print("- ");
		else if(press_change == 1)
			lcd.print("+ ");
		else
			lcd.print("  ");
        lcd.setCursor(10,1);
        lcd.print(cond);
      }
        break;
    }
  }
  else
  {
    lcd.print("Temp Set:");
    lcd.setCursor(9,1);
    lcd.print(request_temperature);
    lcd.setCursor(11,1);
    lcd.print((char)223);
    lcd.setCursor(12,1);
    lcd.print("F  ");
    delay(1000);
    prev_request = request_temperature;
  }
}

byte sleep_timer = 120;

void lcd_loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):

  set_display();
  read_button();
   
  if(dimmer_timer.check() == 1)
  {
    if(analogRead(A0) == 1023 && sleep_timer <= 0)
    {
      digitalWrite(10, LOW);
      sleep_timer = 0;
    }
    if(analogRead(A0) == 1023 && sleep_timer > 0)
    {
      sleep_timer--;
    }
    if(analogRead(A0) != 1023)
    {
      digitalWrite(10, HIGH);
      sleep_timer = 60;
    }
  } 
}

//-----------------------END LCD SECTION--------------------------------
//-----------------------START SENSOR SECTION---------------------------

//DHT Sensor on Analog Pin A1
//Dust Sensor on Analog Pin A2

// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)

// Setup a DHT22 instance
DHT22 myDHT22(A1);
DHT22_ERROR_t errorCode;

Metro sensor_poll = Metro(1000*60*10); //timer for polling sensor (10 minutes)

int dustPin=0; 
int delayTime=280; 
int delayTime2=40; 
float offTime=9680; 
int dustVal=0; 

void DHT_read()
{ 
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
	case DHT_ERROR_NONE:
        {
	  indoor_temp = ((9.0/5.0)*(myDHT22.getTemperatureC() + 32.0)-33);
	  indoor_humidity = myDHT22.getHumidity();
	}
          break;
	//default: 
	//Serial.print("An Error Occured");
   }
}

void Dust_read()
{
  digitalWrite(3,LOW); // power on the LED (Active Low)
  delayMicroseconds(delayTime); 
  dustVal = analogRead(A2); // read the dust value 
  ppm = ppm+dustVal; 
  delayMicroseconds(delayTime2); 
  digitalWrite(3,HIGH); // turn the LED off 
  delayMicroseconds(offTime); // if there's incoming data from the net connection. // send it out the serial port. This is for debugging // purposes only: 

  dustdensity = 0.17*(ppm/0.0049)-0.1; 
  ppmpercf = ((ppm/0.0049)-0.0256)*120000; 
  if (ppmpercf < 0) 
    ppmpercf = 0; 
  if (dustdensity < 0 ) 
    dustdensity = 0; 
  if (dustdensity > 0.5)
    dustdensity = 0.5; 
   
  ppm=0;  
}

void sensor_loop()
{
  if(sensor_poll.check() == 1)
  { 
      Dust_read();
      DHT_read();    
  }
}

//-------------------------END SENSOR SECTION---------------------------
//-------------------------START WIRELESS SECTION-----------------------

//defines and configuration for AsyncLabs/Microchip module

// infrastructure - connect to AP
// adhoc - connect to another WiFi device
#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,100};     // IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,254};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"boosplayhouse"};	//max 32 bytes for SSID

unsigned char security_type = 2;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"lightbulb"};	// max 64 characters password

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {}; 
//{ 
//  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
//};

// setup the wireless mode
unsigned char wireless_mode = WIRELESS_MODE_INFRA;
//SSID
unsigned char ssid_len;
//SECURITY KEY
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------

//Gather METAR data from local airfield to identify outside conditions

// IP Address for server
uint8 ip[] = {98,199,14,11};
char hostName[] = "oconnors.dyndns-server.com";

#define ICAO "KMSY"
//USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
char url_KMSY[] = "/home/metar_output/KMSY.txt";
GETrequest KMSY_req(ip, 80, hostName, url_KMSY);

//COUNTERS

Metro req_counter = Metro(1000*60*60); //counter for request of data from server   

//---END OF CONFIGURATION PARAMETERS---

// Function that prints data from the server
void processData(char * data, int len) 
{
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and
  // includes the HTTP header.
  String buffer;
     
  // Process data (and ignore) until the declaration METAR is seen in the message data
  Serial.println("SERVER REPLY");
  while(len-- > 0)
  {
     buffer = "";
     
     while(((*data) != ' ') && ((*data) != '\n') && (len > 0))
     {
       buffer += *data;
       ++data;
       --len; 
     }
     if(buffer.substring(0) == "station:")
     {
        buffer = "";
        do
        {
           buffer += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        buffer.trim();
        if(buffer.substring(0) != ICAO)
          break; 
        
        Serial.print("Station:");
        Serial.println(buffer);       
     }
     if(buffer.substring(0) == "temperature:")
     {
        buffer = "";
        do
        {
           buffer += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && ((*data) != 'C') && (len > 0));
        buffer.trim();
        
	metar_table[1].temperature = metar_table[0].temperature;
        metar_table[0].temperature = buffer.toInt();
        
        Serial.print("Temperature:");
        Serial.println(metar_table[0].temperature);
     }
     if(buffer.substring(0) == "pressure:")
     {
        buffer = "";
        do
        {
           buffer += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') &&  ((*data) != 'm') &&(len > 0));
        
        buffer.trim();
        
	metar_table[1].pressure = metar_table[0].pressure;
        metar_table[0].pressure = buffer.toInt(); 
       
        Serial.print("Pressure:");
        Serial.println(metar_table[0].pressure); 
     }
   
   ++data;
  }
}  

void wireless_setup() {

  //Serial.println("Starting wireless module");
  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(NULL);

  //WiServer to generate log messages (optional)
  //WiServer.enableVerboseMode(false);
  KMSY_req.setReturnFunc(processData);
  
  delay(1000);
  KMSY_req.submit();
  Serial.println("1st INIT REQUEST SENT");
  
  if(!(KMSY_req.isActive()))
  {
    KMSY_req.submit();
    Serial.println("2nd INIT REQUEST SENT");
  }
}

void wireless_loop(){

  if(req_counter.check() == 1 && !(KMSY_req.isActive()))
  {
      KMSY_req.submit();
      //Serial.println("REQUEST SENT");
  }    
  // Run WiServer
  WiServer.server_task(); 
}

//--------------------------------END WIRELESS SECTION-------------------------------

//--------------------------------INTELLIGENCE SECTION-------------------------------
//temperature request
//byte request_temperature = 68;
//byte prev_request;

//sensor variables
//int indoor_temp = 70;
//int indoor_humidity = 20;

//select button
//boolean select_record = 0;

//fan status: 0-ON, 1-OFF, 2-AUTO
//byte fan_status = 1; 

//system status: 0-SYS OFF, 1-MANU, 2-AUTO
//byte system_status = 0;

//air conditioner settings
//boolean AC = false;
//boolean HEAT = false;

void smart_functions()
{
  //MANUAL MODES
  if(system_status == 1)
  {
    //AC ON
    if(request_temperature <= indoor_temp && indoor_temp > 65)
    {
      AC = true;
      HEAT = false;
    }
    //AC OFF
    else if(request_temperature >= indoor_temp && indoor_temp > 65)
    {
      AC = false;
      HEAT = false;
    }
	
	//HEAT ON
	if(request_temperature >= indoor_temp && indoor_temp < 60)
    {
      AC = false;
      HEAT = true;
    }
    //HEAT OFF
    else if(request_temperature <= indoor_temp && indoor_temp < 60)
    {
      AC = false;
      HEAT = false;
    }
    
  }
  else if(system_status == 0)
  {
	AC = false;
	HEAT = false;
  }
  
  
  
   //AUTO MODES
   //if(system_status == 2)
   //{
   //AC ON
   //if(request_temperature <= indoor_temp && indoor_temp > 65 )
   //{
   // AC = true;
   // HEAT = false;
   //}
   //AC OFF
   //else if(request_temperature >= indoor_temp && indoor_temp > 65)
   //{
   //AC = false;
   //HEAT = false;
   //}
   //HEAT ON
   //HEAT OFF
   //}
  
}

void relay_control()
{
    if(AC)
      digitalWrite(12, HIGH);
    else
      digitalWrite(12, LOW);  
    
    if(HEAT)
      digitalWrite(11, HIGH);
    else
      digitalWrite(11, LOW);
    
    
    if(fan_status == 0)
      digitalWrite(13, LOW);  
    else if(fan_status == 1)
      digitalWrite(13, HIGH);
    else if(fan_status == 2)
    {
      if(AC)
        digitalWrite(13, HIGH);
      else
        digitalWrite(13, LOW);  
    
      if(HEAT)
        digitalWrite(13, HIGH);
      else
        digitalWrite(13, LOW); 
    }
}

void process_data()
{
	//int temp_diff = metar_table[0].temperature - metar_table[1].temperature;
	int press_diff = metar_table[0].pressure - metar_table[1].pressure;
	
	//outside temperature calculation
	//if(metar_table[0].temperature > metar_table[1].temperature && abs(temp_diff) > 5) //outside temperature is rising
	//{
	//}
	//else if(metar_table[0].temperature < metar_table[1].temperature && abs(temp_diff) > 5) //outside temperature is dropping
	//{
	//}
	//else //equivalent temperature
	//{
	//}
	
	
	//pressure calculation
	if(metar_table[0].pressure > metar_table[1].pressure  && abs(press_diff) > 5) //pressure is rising
	{
		press_change = 0;
		if(metar_table[0].pressure < 980)
			cond = "STORMY";
		else if(metar_table[0].pressure > 1020)
			cond = "CLEAR";
		else
			cond = "CHANG";
	}
	else if(metar_table[0].pressure < metar_table[1].pressure && abs(press_diff) > 5) //pressure is falling
	{
		press_change = 1;
		if(metar_table[0].pressure < 980)
			cond = "STORMY";
		else if(metar_table[0].pressure > 1020)
			cond = "CLEAR";
		else
			cond = "CHANG";
	}
	else //equivalent pressure
	{
		press_change = 2;
		if(metar_table[0].pressure < 980)
			cond = "STORMY";
		else if(metar_table[0].pressure > 1020)
			cond = "CLEAR";
		else
			cond = "CHANG";
	}
	
}


Metro test_timer = Metro(1000*10); //10 second timer

//SETUP FOR PROGRAM
void setup()
{
    //Debugging only - start serial connection and set bode rate
    Serial.begin(9600);
    setup_lcd();
    Serial.println("LCD Setup");
    //wireless_setup();
    //Serial.println("Wireless Setup");
    
    pinMode(13, OUTPUT); // DUCT FAN RELAY PIN
    pinMode(12, OUTPUT); // AIR CONDITIONER RELAY PIN
    pinMode(11, OUTPUT); // HEATER RELAY PIN
    
}

//PROGRAM LOOP
void loop()
{
    //wireless_loop();
    sensor_loop();
    smart_functions();
    process_data();
    relay_control();
    lcd_loop();
    delay(10);
}
