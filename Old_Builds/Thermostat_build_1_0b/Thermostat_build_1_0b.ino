/*
 * Digital SMART Thermostat sketch
 *
 * 1. Digital touch display for control of HVAC system (fan and temperature)
 * 2. Transducers for temperature, humidity, particulates, ozone and airflow
 * 3. Allows for communication with noaa weather forcasts to help improve 
 * system efficiency. 
 */

#include <arduino.h>

//Sensor libraries for temp, humidity, particulates
#include <DHT22.h>

//Touch LCD libraries
//#include <Adafruit_GFX.h>
//#include <TFTLCD.h>
//#include <TouchScreen.h>

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
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
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
char url_KMSY[] = "/home/metar_output/KMSY.txt";
GETrequest KMSY_req(ip, 80, hostName, url_KMSY);

//COUNTERS
uint8_t proc_count = 0; //counter for organization of data in METAR_DATA struct 

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
struct METAR_DATA metar_table[4];

void reader_loop(String *buffer, char **data, int *len)
{
   do
   {
     *buffer += *data;
     ++data;
     --len;
     Serial.println(*buffer);
     delay(1000); 
   }
   while(((**data) != '\n') && ((**data) != '\r') && (len > 0));
}

// Function that prints data from the server
void processData(char * data, int len) 
{
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and
  // includes the HTTP header.
  String buffer;
      
// Process data (and ignore) until the declaration METAR is seen in the message data
  Serial.println("Server Reply...processing...");
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
        Serial.println("Found station");
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
        {
            Serial.println("Station Match");
            metar_table[proc_count].ICAO_str = buffer;
        }
        Serial.println("Station logged:");
        Serial.println(metar_table[proc_count].ICAO_str);        
     }
     if(buffer.substring(0) == "time:")
     { 
        Serial.println("Found time");        
     }
     if(buffer.substring(0) == "temperature:")
     {
        Serial.println("Found temperature");
        buffer = "";
        reader_loop(&buffer, &data, &len);
        buffer.trim();
        
        metar_table[proc_count].temperature = buffer;
            
        Serial.println("Temperature logged:");
        Serial.println(metar_table[proc_count].temperature);
     }
     if(buffer.substring(0) == "point:")
     {
        Serial.println("Found dewpoint");
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
            
        Serial.println("Dew Point logged:");
        Serial.println(metar_table[proc_count].dew_point);
     }

     if(buffer.substring(0) == "pressure:")
     {
        Serial.println("Found pressure");
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
            
        Serial.println("Pressure logged:");
        Serial.println(metar_table[proc_count].pressure) ;
     }
   
   data++;
  }
}  

void setup() {

  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(NULL);

  //start serial connection and set bode rate
  Serial.begin(9600);
  //WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(false);
  Serial.println("Starting...Thermostat_build_1_0b");
  KMSY_req.setReturnFunc(processData);
}

uint16_t main_counter = 0;

void loop(){

  if(main_counter <= 0)
  {
    if(proc_count >= 3)
      proc_count = 0;
    else
      proc_count++;
    main_counter = 60000;
    KMSY_req.submit();
    Serial.println("METAR request submitted");
  }
     
  // Run WiServer
  WiServer.server_task();
  delay(100);
  main_counter--; //counter based on the delay time (may not be accurate at all)
}




