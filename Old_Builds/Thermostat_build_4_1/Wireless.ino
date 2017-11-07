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
uint8 ip_server[] = {98,199,14,11};
char server_Host_Name[] = "oconnors.dyndns-server.com";

#define ICAO "station: KMSY"
//USED FOR CURRENT METAR INFORMATION AT NEW ORLEANS LOUIS ARMSTRONG
char url_metar_server[] = "/home/metar_output/KMSY.txt";

// PACHUBE FEED
uint8 ip_pachube[] = {216,52,233,122};
char cosm_Host_Name1[] = "api.pachube.com\nX-PachubeApiKey:vcEy4dJBalsqPVjcB0_4VGZsCT-SAKx0QnNUVXFFdVovMD0g\nConnection:close";
char url_cosm_feed1[] = "/v1/feeds/99111.csv?_method=put";
//
GETrequest METAR_req(ip_server, 80, server_Host_Name, url_metar_server);
POSTrequest postCosm(ip_pachube, 80, cosm_Host_Name1, url_cosm_feed1, feed); 


//DATA PROCESSING VARS
uint8 i;
char *tmp;
byte pass = 0;


//---END OF CONFIGURATION PARAMETERS---

// Function for pachube/cosm
void feed()
{
  WiServer.println(indoor_temp);
}


// Function that prints data from COSM server (DEBUGGING)
void processCosmData(char *data, int len){}

// Function that prints data from the METAR server
void METARServerData(char * data, int len) 
{
  //Serial.println("SERVER REPLY");

  if(len != -1)
  {
    if(pass == 0)
    {
       tmp = strstr(data, "\r\n\r\n");
       
       if(tmp != NULL && (strstr(tmp, ICAO) != NULL))
          pass = 1;      
    }
    else if(pass == 1)
    {
       tmp = strstr(data, "temperature:");
       if(tmp != NULL)
       {
         if(tmp[13] == '-')
         {
           if(tmp[15] == '.')
             metar_table[0].temperature = ((tmp[14] - '0') + (tmp[16] - '0')*0.1)*-1.0;        
           else if(tmp[16] == '.')
             metar_table[0].temperature = ((tmp[14] - '0')*10.0 + (tmp[15] - '0') + (tmp[17] - '0')*0.1)*-1.0;
           else
             metar_table[0].temperature = ((tmp[14] - '0')*100.0 + (tmp[15] - '0')*10.0 + (tmp[16] - '0') + (tmp[18] - '0')*0.1)*-1.0;
           
         }
         else
         {
           if(tmp[14] == '.')
             metar_table[0].temperature = (tmp[13] - '0') + (tmp[15] - '0')*0.1;
           if(tmp[15] == '.')
             metar_table[0].temperature = (tmp[13] - '0')*10 + (tmp[14] - '0') + (tmp[16] - '0')*0.1;
           else
             metar_table[0].temperature = (tmp[13] - '0')*100 + (tmp[14] - '0')*10 + (tmp[15] - '0') + (tmp[17] - '0')*0.1;
         }    
       }
       //CONVERSION TO FAHRENHEIT
       metar_table[0].temperature = round(((9.0/5.0)*metar_table[0].temperature) + 32.0);
       //Serial.print("Temperature: ");
       //Serial.print(metar_table[0].temperature);
       //Serial.println(" F");
       
              
       tmp = strstr(data, "pressure:");
       
       if(tmp != NULL)
       {
          if(tmp[13] == ' ' || tmp[13] == '.')
            metar_table[0].pressure = (tmp[10] - '0')*100 + (tmp[11] - '0')*10 + (tmp[12] - '0');
          else
            metar_table[0].pressure = (tmp[10] - '0')*1000 + (tmp[11] - '0')*100 + (tmp[12] - '0')*10 + (tmp[13] - '0');
       }                   
          
       //Serial.print("Pressure: ");   
       //Serial.print(metar_table[0].pressure);
       //Serial.println(" mb");
       
       pass = 2;
       //Serial.println("\n\n\n ORIGINAL MESSAGE:");  
       //Serial.println(tmp);
    }
    //all other passes are just repeated or are garbage
  }    
  data_lockout = false;
}  

// This is our page serving function that generates web pages
boolean sendPage(char* URL) {
 
  if(strcmp(URL, "/sysoff") ==0)
  {
    system_status = 0;
    
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
      
    if(AC)
      WiServer.print("    AC ON");
    if(HEAT)
      WiServer.print("    HEAT ON");
      
   
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
    
    WiServer.print("Indoor Temperature");
    
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[1]));
    WiServer.print(buffer);  //line break

    strcpy_P(buffer, (char*)pgm_read_word(&string_table[12]));
    WiServer.print(buffer);  //indoor temp graph
    
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
  postCosm.setReturnFunc(processCosmData);
  
  delay(1000);
  METAR_req.submit();
  //Serial.println("1st INIT REQUEST SENT");
  
  postCosm.submit();
  //Serial.println("Cosm Updated");

}

void wireless_loop(){

  if(five_min_cnt > 254) //reused 5 minute timer
  {
    postCosm.submit();
    //Serial.println("Cosm Updated");
    //five_min_cnt = 0;  <--- This is reset only after the smart function has completed as well
  }
  if(metar_server_cnt > 3600 && !(METAR_req.isActive()))  //METAR request every 60 minutes
  {
      pass = 0;
      metar_table[1].temperature = metar_table[0].temperature;
      metar_table[1].pressure = metar_table[1].pressure;
      METAR_req.submit();
      metar_server_cnt = 0;
  }    
  // Run WiServer
  WiServer.server_task(); 
}

