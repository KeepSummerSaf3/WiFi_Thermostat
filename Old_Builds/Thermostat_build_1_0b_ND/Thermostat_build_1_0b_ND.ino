/*
 * Digital SMART Thermostat sketch
 *
 * 1. Digital touch display for control of HVAC system (fan and temperature)
 * 2. Transducers for temperature, humidity, particulates, ozone and airflow
 * 3. Allows for communication with noaa weather forcasts to help improve 
 * system efficiency. 
 */

#include <arduino.h>

//Multiplexer library
#include <analogmuxdemux.h>

//Sensor libraries for temp, humidity, particulates
#include <DHT22.h>

//Touch LCD libraries
#include <Adafruit_GFX.h>
#include <TFTLCD.h>
#include <TouchScreen.h>

//WiServer library
#include <WiServer.h>

//defines
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
prog_uchar wep_keys[] PROGMEM = { 
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------

//Gather METAR data from local airfield to identify outside conditions

// IP Address for server
uint8 ip[] = {98,199,14,11};
char hostName[] = "oconnors.dyndns-server.com";

#define ICAO "KMSY"
//USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
char * url_KMSY = "/home/metar_output/KMSY.txt";
GETrequest KMSY_req(ip, 80, hostName, url_KMSY);

//COUNTERS
byte proc_count = 0; //counter for organization of data in METAR_DATA struct 

//DATA STORAGE FOR METAR DATA
struct METAR_DATA
{
  int issue_day;
  int issue_month;
  int issue_time;
    
  String ICAO_str;
  String temperature;
  String dew_point;
  String pressure;
};
struct METAR_DATA metar_table[3];

//END METAR PARAMETERS---------------------------------------------

//Multiplexer I/O Defines
#define READPIN A5
#define NO_PINS 4
// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
//#define DHT22_PIN 7

uint8 indoor_count;

struct TEMP_HUMID_TABLE
{
  byte indoor_temp;
  byte indoor_humid;
};
struct TEMP_HUMID_TABLE tht[4];

//setup multiplexer object with I/O
AnalogMux amux(4,3,2, READPIN); 
// Setup a DHT22 instance
DHT22 myDHT22(READPIN);

//END Multiplexer, Temperature, Humidity and Dust Sensor I/O


// Function that prints data from the server
void processData(char * data, int len) 
{
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and
  // includes the HTTP header.
  String buffer;
      
// Process data (and ignore) until the declaration METAR is seen in the message data
  while(len-- > 0)
  {
     buffer = "";
     
     while(((*data) != ' ') && ((*data) != '\n') && (len > 0))
     {
       buffer += *data;
       ++data;
       --len; 
     }
     //Serial.println(tmp);
          
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
        if(buffer.substring(0) == ICAO)
          metar_table[proc_count].ICAO_str = buffer;
                     
     }
     
     if(buffer.substring(0) == "temperature:")
     {
        Serial.println("Found temperature");
        buffer = "";
        
        do
        {
           buffer += (*data);
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        buffer.trim();
        
        metar_table[proc_count].temperature = buffer;
        
        Serial.println("Temperature logged:");
        Serial.println(metar_table[proc_count].temperature);
     }
     if(buffer.substring(0) == "point:")
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
                
        metar_table[proc_count].dew_point = buffer;
             
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
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        
        buffer.trim();
               
        metar_table[proc_count].pressure = buffer;
            
     }
   
   data++;
  }
}  



void DHT_read()
{ 
  DHT22_ERROR_t errorCode;

  int errcount = 0;
  do
  {
    if(errcount > 10)
      return;
    
    delay(4000);
    errorCode = myDHT22.readData();
  }
  while(errorCode != DHT_ERROR_NONE);
  
  tht[indoor_count].indoor_temp = myDHT22.getTemperatureC();
  tht[indoor_count].indoor_humid = myDHT22.getHumidity();
  
}

void setup() {

  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(NULL);

  KMSY_req.setReturnFunc(processData);
}

uint8 main_counter = 0;
uint8 secondary_counter = 0;

void loop()
{

  if(!secondary_counter)
  {
    if(indoor_count == 3)
      indoor_count = 0;      
    
    for (int pinno=0; pinno < NO_PINS; pinno++)
    {
        delay(5000);
        amux.SelectPin(pinno); // choose the pin you want to send signal to off the DeMux
        
        
        if(pinno == 1)
          DHT_read();
        else
        {
          uint16_t reading = amux.AnalogRead();
        }
    }
  }
  
  if(!main_counter)
  {
    if(proc_count == 3)
      proc_count = 0;
    else
    {
      proc_count++;
      main_counter = 600000;
      KMSY_req.submit();
    }
  }
  
  
  
     
  // Run WiServer
  WiServer.server_task();
  delay(10);
  
  //counter based on the delay time (may not be accurate at all)
  if(main_counter > 0)
    main_counter--; 
  if(secondary_counter > 0)
    secondary_counter--;
  
}




