//ETHERNET SECTION

//Ethernet configuration parameters ----------------------------------------

byte unit_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// assign an IP address
byte unit_ip[] = {172,16,1,44};
//byte * gateway = {192,168,1,1};	
//byte * subnet = {255, 255, 255, 0};

//End of Ethernet configuration parameters ---------------------------------

//Gather METAR data from local airfield to identify outside conditions

// Weather Data Server (Yahoo Weather)
uint8_t ip_weather_server[] = {98,199,14,11};
char weather_data[] = "/lighttpd/METARS/";  
#define METAR_LOC "KIAH.txt"

Client weather_client(ip_weather_server, 80);

// Thingspeak Data Server
uint8_t ip_thingspeak_server[] = {184,106,153,149};
//char thingspeak_server[] = "api.thingspeak.com";
char APIKey_write[] = "NZUFCGMOVGHU7TUW";

Client thingspeak_client(ip_thingspeak_server, 80);

//---END OF CONFIGURATION PARAMETERS---

void wired_setup() {

  Ethernet.begin(unit_mac, unit_ip);
  Serial.println("Ethernet connected");
}

char * tmp;

void wired_loop(){

//  if((minute() % 2) == 0 && second() == 0) 
//  {
//      if (thingspeak_client.connect())  //Thingspeak post
//      {
//         thingspeak_client.print(F("POST /update HTTP/1.0\n"));
//         thingspeak_client.print(F("Host: api.thingspeak.com\n"));
//         thingspeak_client.print(F("Connection: close\n"));
//         thingspeak_client.print(F("X-THINGSPEAKAPIKEY:"));
//         thingspeak_client.print(APIKey_write);
//         thingspeak_client.print(F("\nContent-Type: application/x-www-form-urlencoded\n"));
//         thingspeak_client.print(F("Content-Length: "));
//         thingspeak_client.print("");         
//      }
//  }
  if((minute() % 5) == 0 && second() == 0)  //METAR request every 5 minutes
  {
      //retreive METARS
      if(weather_client.connect());
      {
         Serial.println(F("connected to webserver"));
         weather_client.print(F("GET /"));
         weather_client.print(weather_data);
         weather_client.print(METAR_LOC);
         //weather_client.print(F(" HTTP/1.1\r\n\r\n"));
         weather_client.print(F(" HTTP/1.0"));
         
         if(weather_client.available())
         {
           Serial.print(weather_client.read());
         }
      }
  }    
  
//  if(weather_client.available())   //METAR response
//  {
//      sdfile_descrpt = SD.open("/metars/metar.txt", FILE_WRITE);
//      while(weather_client.available())
//      {
//        sdfile_descrpt.write(weather_client.read());
//      }
//      weather_client.stop();
//      sdfile_descrpt.close();
//  }
}

