//WIRELESS SECTION

//defines and configuration for AsyncLabs/Microchip module

// infrastructure - connect to AP
// adhoc - connect to another WiFi device
#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,10};     // IP address of WiShield
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

//// IP Address for server
uint8 ip_server[] = {38,102,136,138};
char server_Host_Name[] = "api.wunderground.com/";

#define ICAO "station: KMSY"
//USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
char url_metar_server[] = "api/241f9358bed1065c/conditions/q/LA/New_Orleans.json";

// PACHUBE FEED
uint8 ip_pachube[] = {216,52,233,122};
char cosm_Host_Name[] = "api.pachube.com\nX-PachubeApiKey:vcEy4dJBalsqPVjcB0_4VGZsCT-SAKx0QnNUVXFFdVovMD0g\nConnection:close";
char url_cosm_feed_Itemp[] = "/v1/feeds/99111/datastreams/0.csv?_method=put";
char url_cosm_feed_Ihumid[] = "/v1/feeds/99111/datastreams/1.csv?_method=put";
//
GETrequest METAR_req(ip_server, 80, server_Host_Name, url_metar_server);
POSTrequest postCosmIT(ip_pachube, 80, cosm_Host_Name, url_cosm_feed_Itemp, feed_Itemp); 
POSTrequest postCosmIH(ip_pachube, 80, cosm_Host_Name, url_cosm_feed_Ihumid, feed_Ihumid);

//DATA PROCESSING VARS
//uint8 i;
//char *tmp;
//byte pass = 0;


//---END OF CONFIGURATION PARAMETERS---

// Function for pachube/cosm
// COSM nicely separates the data when it is sent :)
void feed_Itemp()
{
  WiServer.println(indoor_temp);
}
void feed_Ihumid()
{
  WiServer.println(indoor_humidity); 
}

// Function that prints data from COSM server (DEBUGGING)
void processCosmData(char *data, int len){}

// Function that prints data from the METAR server
void METARServerData(char * data, int len) 
{
  while (len-- > 0)
  {
    Serial.print(*(data++));
  }
  
}  

// This is our page serving function that generates web pages
boolean sendPage(char* URL) {
 
  if(strcmp(URL, "/sysoff") ==0)
  {
    system_status = 0;
    fan_status = 0;
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[11]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer);
  }
  else if(strcmp(URL, "/sysauto") == 0)
  {
    system_status = 2;
    fan_status = 2;
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[11]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer);
  }
  else if(strcmp(URL, "/syseff") == 0)
  {
    system_status = 3;
    fan_status = 2;
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[11]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer);
  }
  
  else if(strcmp(URL, "/temp_inc") == 0)
  {
    if(request_temperature < 85)
      request_temperature++;
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[11]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer);
  }
  
  else if(strcmp(URL, "/temp_dec") == 0)
  {
    if(request_temperature > 55)
      request_temperature--;
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[11]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer);
  }
  
  else
  {
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[0]));
    WiServer.print(buffer); //page start
  
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[3]));
    WiServer.print(buffer); //page heading
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[10]));
    WiServer.print(buffer); //page auto refresh
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);
  
    WiServer.print("System Status: ");
    if(system_status == 0)
      WiServer.print("OFF");
    else if(system_status == 1)
      WiServer.print("MANUAL MODE");
    else if(system_status == 2)
      WiServer.print("AUTOMATIC MODE");
    else
      WiServer.print("EFFICIENCY MODE");
      
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    if(AC)
      WiServer.print("AC ON");
    if(HEAT)
      WiServer.print("HEAT ON");
      
   
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[4]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[5]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[4]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[6]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[4]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[7]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    WiServer.print("Current Request Temperature: ");
    WiServer.print(request_temperature);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[4]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[8]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[4]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[9]));
    WiServer.print(buffer);
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break

    strcpy_P(buffer, (char*)pgm_read_word(&string_table[12]));
    WiServer.print(buffer);  //indoor temp graph
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[13]));
    WiServer.print(buffer);  //end src line (from above)
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[30]));
    WiServer.print(buffer);  //indoor humid graph
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[13]));
    WiServer.print(buffer);  //end src line (from above)
        
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[2]));
    WiServer.print(buffer); //page end
  }
}

void wireless_setup() {

  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendPage);
  //WiServer.init(NULL);
  
  //WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(false);
  
  METAR_req.setReturnFunc(METARServerData);
  postCosmIT.setReturnFunc(processCosmData);
  postCosmIH.setReturnFunc(processCosmData);
  
  delay(1000);
  METAR_req.submit();
  //Serial.println("1st INIT REQUEST SENT");
}

void wireless_loop(){

  if(five_min_cnt > 254) //reused 5 minute timer
  {
    postCosmIT.submit();
    postCosmIH.submit();
    //Serial.println("Cosm Updated");
    //five_min_cnt = 0;  <--- This is reset only after the smart function has completed as well
  }
  if(metar_server_cnt > 3600 && !(METAR_req.isActive()))  //METAR request every 60 minutes
  {
      //pass = 0;
      metar_table[1].temperature = metar_table[0].temperature;
      metar_table[1].pressure = metar_table[1].pressure;
      METAR_req.submit();
      metar_server_cnt = 0;
  }    
  // Run WiServer
  WiServer.server_task(); 
}

