#include <DHT22.h>
#include <WiServer.h>
#include <arduino.h>

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

uint8 ip[] = {173,203,98,29};
char serverName[] = "api.cosm.com\nX-PachubeApiKey: eLSP47-fnVgzDD--m33b1TE4jEGSAKxHNE03dVBCdlFuYz0g\nConnection: close";
char url[] = "/api/89797.csv?_method=put";

POSTrequest postPachube(ip, 80, serverName, url, feedData);

// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN 7

// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);
boolean attained = false;
DHT22_ERROR_t errorCode;


void printData(char* data, int len) {
 
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and
  // includes the HTTP header.
  while (len-- > 0) {
    Serial.print(*(data++));
  }
}

void feedData()
{
  while(!attained)
  {
    delay(2000);
    Serial.print("Requesting data...");
    errorCode = myDHT22.readData();
    switch(errorCode)
    {
    case DHT_ERROR_NONE:
      Serial.print("Got Data ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
      attained = true;
      WiServer.print(myDHT22.getTemperatureC());
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
      break;
    }
  }
  attained = false;
}

void setup(void)
{
  WiServer.init(NULL);
  // start serial port
  Serial.begin(9600);
  Serial.println("DHT22 Library Demo");
  WiServer.enableVerboseMode(true);
  postPachube.setReturnFunc(printData);
}


void loop(void)
{
  if((millis() % 1000) == 0 )
    Serial.println("tick...tock");
     
  if((millis() % 60000) == 0)
  {
    Serial.println("Submitting pachube request");
    postPachube.submit();
  } 
  
  WiServer.server_task();
  delay(10);
  
}

