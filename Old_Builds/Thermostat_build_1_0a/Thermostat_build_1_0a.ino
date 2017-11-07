/*
 * Digital SMART Thermostat sketch
 *
 * Digital touch display for control of HVAC system (fan and temperature)
 * Transducers for temperature, humidity, particulates, ozone and airflow
 * Allows for communication with noaa weather forcasts to predict when to operate air conditioner and heater 
 */

#include <arduino.h>

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

//Gather METAR data from KMSY and 4 surrounding airfields to identify frontal boundaries
//yes this is a very, very backwards and rather inaccurate method of forcasting but it 
//should theoretically work in most instances.
// IP Address for server
uint8 ip[] = {98,199,14,11};
char hostName[] = "oconnors.dyndns-server.com";

//USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
char url_KMSY[] = "/home/metar_output/KMSY.txt";
GETrequest KMSY_req(ip, 80, hostName, url_KMSY);

//USED FOR CURRENT METAR INFORMATION AT DALLAS LOVE FIELD
char url_KDAL[] = "/home/metar_output/KDAL.txt";
GETrequest KDAL_req(ip, 80, hostName, url_KDAL);

//USED FOR CURRENT METAR INFORMATION AT LITTLE ROCK AIR FORCE BASE
char url_KLRF[] = "/home/metar_output/KLRF.txt";
GETrequest KLRF_req(ip, 80, hostName, url_KLRF);

//USED FOR CURRENT METAR INFORMATION AT HOUSTON INTERCONTINENTAL
char url_KIAH[] = "/home/metar_output/KIAH.txt";
GETrequest KIAH_req(ip, 80, hostName, url_KIAH);

//COUNTERS
int proc_count = 0; //counter for organization of data in arrays 

//LIST OF ICAOS and the number (since sizeof does not seem to work properly)
String ICAOS[] = {
  "KMSY", "KIAH", "KLRF", "KDAL"};
#define NUM_ICAOS 4

//DATA STORAGE FOR METAR DATA
struct METAR_DATA
{
  int issue_day;
  int issue_month;
  int issue_time;
    
  String ICAO;
  String temperature;
  String dew_point;
  String pressure;
//  int wind_velocity;
//  int wind_heading;
};
struct METAR_DATA metar_table[4];

// Function that prints data from the server
void processData(char * data, int len) 
{
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and
  // includes the HTTP header.
//  Serial.println("START TRANSMISSION");
  //DEBUG
//  while(len > 0)
//  {
//   if((*data) == ' ')
//   {
//     Serial.print("*");
//   }
//   else if((*data) == '/n')
//   {
//     Serial.print("\n");
//   } 
//    Serial.print(*(data++));
//    len--;
//  }
//  Serial.println("END TRANSMISSION");

  String tmp;
  char * tmp_array;
    
// Process data (and ignore) until the declaration METAR is seen in the message data
  Serial.println("Server Reply...processing...");
  while(len-- > 0)
  {
     tmp = "";
     
     while(((*data) != ' ') && ((*data) != '\n') && (len > 0))
     {
       tmp += *data;
       ++data;
       --len; 
     }
     //Serial.println(tmp);
          
     if(tmp.substring(0) == "station:")
     {
        Serial.println("Found station");
        tmp = "";
        do
        {
           tmp += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        tmp.trim();
        for(int i=0; i < 4; i++)
        {
          if(tmp.substring(0) == ICAOS[i])
          {
            Serial.println("Station Match");
            metar_table[proc_count].ICAO = tmp;
            break;
          }
        }
        Serial.println("Station logged:");
        Serial.println(metar_table[proc_count].ICAO);        
     }
     if(tmp.substring(0) == "time:")
     { 
        Serial.println("Found time");        
     }
     if(tmp.substring(0) == "temperature:")
     {
        Serial.println("Found temperature");
        tmp = "";
        do
        {
           tmp += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
      
        tmp.trim();
              
        metar_table[proc_count].temperature = tmp;
           
        Serial.println("Temperature logged:");
        Serial.println(metar_table[proc_count].temperature) ;
     }
     if(tmp.substring(0) == "point:")
     {
        Serial.println("Found dewpoint");
        tmp = "";
        do
        {
           tmp += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        
        tmp.trim();
                
        metar_table[proc_count].dew_point = tmp;
            
        Serial.println("Dew Point logged:");
        Serial.println(metar_table[proc_count].dew_point) ;
     }
//     if(tmp.substring(0) == "wind:")
//     {
//        Serial.println("Found wind");
//        delay(10000);
//     }
     if(tmp.substring(0) == "pressure:")
     {
        Serial.println("Found pressure");
        tmp = "";
        do
        {
           tmp += *data;
           ++data;
           --len; 
        }
        while(((*data) != '\n') && ((*data) != '\r') && (len > 0));
        
        tmp.trim();
               
        metar_table[proc_count].pressure = tmp;
            
        Serial.println("Pressure logged:");
        Serial.println(metar_table[proc_count].pressure) ;
     }
   
   data++;
  }
}  
   
//void displaytest()
//{
//  Serial.println("METAR DATA OUTPUT");
//  Serial.println(metar_table[proc_count].ICAO);
//  Serial.print("DAY: ");
//  Serial.print(time_table[proc_count].issue_day);
//  Serial.print(" TIME: ");
//  Serial.print(time_table[proc_count].issue_time);
//  Serial.println(" GMT");
//  Serial.print("Temperature: ");
//  Serial.print(metar_table[proc_count].temperature);
//  Serial.println(" deg F");
//  Serial.print("Pressure: ");
//  Serial.print(metar_table[proc_count].pressure);
//  Serial.println(" In. Hg");
//}

void setup() {

  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(NULL);

  //start serial connection and set bode rate
  Serial.begin(57600);
  //WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(false);
  Serial.println("Starting...Thermostat_build_0_2b");
  KMSY_req.setReturnFunc(processData);
  KDAL_req.setReturnFunc(processData);
  KLRF_req.setReturnFunc(processData);
  KIAH_req.setReturnFunc(processData);
}

long main_counter = 0;
int handle = 0;

void loop(){

  if(main_counter <= 0)
  {
    switch(handle)
    {
     Serial.println("Metars requested");
     case 0:{
            proc_count = 0;
            main_counter = 50000;
            handle = 1;
            KIAH_req.submit();
     }
     case 1:{
            proc_count = 1;
            main_counter = 50000;
            handle = 2;
            KMSY_req.submit();
     }
     case 2:{
            proc_count = 2;
            main_counter = 50000;
            handle = 3;
            KDAL_req.submit();
     }
     case 3:{
            proc_count = 3;
            main_counter = 50000;
            handle = 0;
            KLRF_req.submit();
     }
    }
  }
    
  // Run WiServer
  WiServer.server_task();
  delay(10);
  main_counter--; //counter based on the delay time (may not be accurate at all)
}




