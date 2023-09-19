#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include<dht.h> 
int GPSBaud = 9600;
String AP = "Karan";       // Hotspot Name
String PASS = "123456789"; // Hotspot PASSWORD
String API = "MGT0FO9TXKGVC8FA";   // Write API KEY
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
int measurePin = A0;
int ledPower = 12;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0.17*calcVoltage-0.1;
 

#include<Timer.h>
Timer t;
#define dht_dpin 2 
dht DHT;
int COsensor=A3;
int CO2sensor=A1;
int SO2sensor=A2;

  int coValue;
  int co2Value;
  int so2Value;

void setup() {
  pinMode(ledPower,OUTPUT);  
  Serial.begin(9600); 
  Serial.println("WEB APPLICATION BASED REALTIME LOCALITE AIR QUALITY MONITORING AND PREDICTION USING ML");
    delay(1000);
  Serial.println("Connecting Wifi....");
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  esp8266.end();
 
    Serial.println("Wifi Connected"); 
  /* Define baud rate for serial communication */
  /* Define baud rate for software serial communication */
}

void loop() {
  CO_MEASUREMENT();
  
  CO2_MEASUREMENT();
  
  SO2_MEASUREMENT();
  
  HUMIDITY_MEASUREMENT();
  delay(1000);
  DUST();
   
  GPS_SoftSerial.begin(9600); 
  
        smartDelay(1000); /* Generate precise delay of 1ms */
        unsigned long start;
        double lat_val, lng_val, alt_m_val;
        uint8_t hr_val, min_val, sec_val;
        bool loc_valid, alt_valid, time_valid;
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);

GPS_SoftSerial.end();
esp8266.begin(9600);
 String getData = "GET /update?api_key="+ API +"&field1="+DHT.humidity+"&field2="+DHT.temperature+"&field3="+coValue+"&field4="+co2Value+"&field5="+so2Value+"&field6="+dustDensity+"&field7="+String(lat_val,6)+"&field8="+String(lng_val,6);
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
void HUMIDITY_MEASUREMENT(void)
{
    DHT.read11(dht_dpin);
    Serial.print("Humidity: ");
    Serial.print(DHT.humidity);   // printing Humidity on LCD
    Serial.print(" %");
    Serial.print("  ");
    
    delay(500);

    Serial.print("Temperature:");
    Serial.print(DHT.temperature);   // Printing temperature on LCD
    Serial.print(" C");
    Serial.println("  ");
    delay(500);
    
 }
void CO_MEASUREMENT()
{
  coValue=analogRead(A3);
  Serial.print("CO: ");
  Serial.println(coValue);
  delay(1000);
}
void CO2_MEASUREMENT()
{
 co2Value=analogRead(A1);
  Serial.print("CO2: ");
  Serial.println(co2Value);
  delay(1000);
  
}
void SO2_MEASUREMENT()
{
  so2Value=analogRead(A2);
  Serial.print("SO2: ");
  Serial.println(so2Value);
  delay(1000);
  
}
void DUST()
{
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  Serial.println("Raw Signal Value (0-1023):");
  Serial.println(voMeasured);

  Serial.println("Voltage:");
  Serial.println(calcVoltage);

  Serial.println("Dust Density:");
  Serial.println(dustDensity);

  delay(1000);

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
