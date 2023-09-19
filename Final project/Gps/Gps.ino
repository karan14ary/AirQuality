#include <TinyGPS++.h>
#include <SoftwareSerial.h>
int GPSBaud = 9600;
String AP = "Karan";       // Hotspot Name
String PASS = "123456789"; // Hotspot PASSWORD
String API = "AB6YLMLEYD3NFB1N";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand;
/* Create object named bt of the class SoftwareSerial */
SoftwareSerial esp8266(6,7);
SoftwareSerial GPS_SoftSerial(8, 9);/* (Rx, Tx) */
/* Create an object named gps of the class TinyGPSPlus */
TinyGPSPlus gps;
boolean found = false;
int valSensor = 1;
volatile float minutes, seconds;
volatile int degree, secs, mins;

void setup() {
  Serial.begin(9600); 
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  esp8266.end();
  /* Define baud rate for serial communication */
  /* Define baud rate for software serial communication */
}

void loop() {
  GPS_SoftSerial.begin(9600); 
  
        smartDelay(1000); /* Generate precise delay of 1ms */
        unsigned long start;
        double lat_val, lng_val, alt_m_val;
        uint8_t hr_val, min_val, sec_val;
        bool loc_valid, alt_valid, time_valid;
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
//        alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
//        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
//        hr_val = gps.time.hour(); /* Get hour */
//        min_val = gps.time.minute();  /* Get minutes */
//        sec_val = gps.time.second();  /* Get seconds */
//        time_valid = gps.time.isValid();  /* Check if valid time data is available */
//        if (!loc_valid)
//        {          
//          Serial.print("Latitude : ");
//          Serial.println("*****");
//          Serial.print("Longitude : ");
//          Serial.println("*****");
//        }
//        else
//        {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
//          Serial.print("Latitude in Degrees Minutes Seconds : ");
//          Serial.print(degree);
//          Serial.print("\t");
//          Serial.print(mins);
//          Serial.print("\t");
//          Serial.println(secs);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);
//          Serial.print("Longitude in Degrees Minutes Seconds : ");
//          Serial.print(degree);
//          Serial.print("\t");
//          Serial.print(mins);
//          Serial.print("\t");
//          Serial.println(secs);
//        }
//        if (!alt_valid)
//        {
//          Serial.print("Altitude : ");
//          Serial.println("*****");
//        }
//        else
//        {
//          Serial.print("Altitude : ");
//          Serial.println(alt_m_val, 6);    
//        }
//        if (!time_valid)
//        {
//          Serial.print("Time : ");
//          Serial.println("*****");
//        }
//        else
//        {
//          char time_string[32];
//          sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
//          Serial.print(time_string);    
//        }
GPS_SoftSerial.end();
esp8266.begin(9600);
 String getData = "GET /update?api_key="+ API +"&field1="+lat_val+"&field2="+lng_val;
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 esp8266.end();
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
 
    countTimeCommand++;
  }
 
  if(found == true)
  {
    Serial.println("PASS");
    countTrueCommand++;
    countTimeCommand = 0;
  }
 
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
 
  found = false;
 }


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}
