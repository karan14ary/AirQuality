
//#include <LiquidCrystal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<dht.h> 
 
int measurePin = A0;
int ledPower = 12;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
//float dustDensity;
// voMeasured = analogRead(measurePin);
//calcVoltage = voMeasured*(5.0/1024);
float dustDensity = 0.17*calcVoltage-0.1;




#include<Timer.h>
Timer t;
#include <SoftwareSerial.h>
SoftwareSerial Serial1(6, 7);

#define heart 13                              

char *api_key="MGT0FO9TXKGVC8FA";     // Enter your Write API key from ThingSpeak
static char postUrl[150];
void httpGet(String ip, String path, int port=80);


#define dht_dpin 2 
//LiquidCrystal lcd(13, 12, 5, 4, 3, 2);


dht DHT;
//char MOISTURE_buff[20],CO_buff[20];
//int MOISTURE_Value;




int COsensor=A3;
int CO2sensor=A1;
int SO2sensor=A2;

int coValue=analogRead(A3);
int co2Value=analogRead(A1);
int so2Value=analogRead(A2);


//int nValue;
//int pValue;
//int kValue;
//unsigned long int avgValue;  //Store the average value of the sensor feedback
//float b;
int buf[10],temp;
void setup() 
{
    Serial.begin(9600); 
//    lcd.begin(16, 2);
    Serial.println("WEB APPLICATION BASED REALTIME LOCALITE AIR QUALITY MONITORING AND PREDICTION USING ML");
    delay(1000);
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("CROP YIELD");
//    lcd.setCursor(0,1);
//    lcd.print(" PREDICTION...");
//    delay(2000);
    Serial1.begin(9600);
   Serial.begin(9600);
//   lcd.clear();
//   lcd.print("WIFI Connecting");
//   lcd.setCursor(0,1);
//   lcd.print("Please wait....");

  pinMode(ledPower,OUTPUT);

  
   
   Serial.println("Connecting Wifi....");
   connect_wifi("AT",1000);
   connect_wifi("AT+CWMODE=1",1000);
   connect_wifi("AT+CWQAP",1000);  
   connect_wifi("AT+RST",5000);
   connect_wifi("AT+CWJAP=\"smile123\",\"123456789\"",10000);
   Serial.println("Wifi Connected"); 
//   lcd.clear();
//   lcd.print("WIFI Connected.");
   pinMode(heart, OUTPUT);
   delay(2000);
//   t.oscillate(heart, 1000, LOW);
 //  t.every(20000, send2server);

}

void loop()                    
{
 
  CO_MEASUREMENT();
  
  CO2_MEASUREMENT();
  
  SO2_MEASUREMENT();
  
  HUMIDITY_MEASUREMENT();
  delay(1000);
  DUST();
  
 
  
//  lcd.print("N:");
//  lcd.print(nValue);
//  lcd.setCursor(6, 0);
//  lcd.print("P:");
//  lcd.print(pValue);
//  lcd.setCursor(0, 1);
//   lcd.print("K:");
//  lcd.print(kValue);
//  delay(1000);
 
  t.update();
      
}

char Serial_read(void)
{
      char ch;
      while(Serial.available() == 0);
      ch = Serial.read(); 
      return ch;
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
  int coValue=analogRead(A3);
  Serial.print("CO: ");
  Serial.println(coValue);
  delay(1000);
}
void CO2_MEASUREMENT()
{
 int co2Value=analogRead(A1);
  Serial.print("CO2: ");
  Serial.println(co2Value);
  delay(1000);
  
}
void SO2_MEASUREMENT()
{
  int so2Value=analogRead(A2);
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
void send2server()
{
  char humidityStr[8];
  char tempStr[8];
//  char moistureStr[8];
  
  char coValueStr[8];
  char co2ValueStr[8];
  char so2ValueStr[8];
  char dustDensityStr[8];
  
  
  dtostrf(DHT.temperature, 5, 3, tempStr);
  dtostrf(DHT.humidity, 5, 3, humidityStr);
//  dtostrf(MOISTURE_Value, 5, 3, moistureStr);
  dtostrf(coValue, 5, 3, coValueStr);

  dtostrf(co2Value, 5, 3, co2ValueStr);
  dtostrf(so2Value, 5, 3, so2ValueStr);
  dtostrf(dustDensity, 5, 3, dustDensityStr);


  sprintf(postUrl, "update?api_key=%s&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field6=%s",api_key,humidityStr,tempStr,coValueStr,co2ValueStr,so2ValueStr,dustDensityStr);
  httpGet("api.thingspeak.com", postUrl, 80);
}

//GET https://api.thingspeak.com/update?api_key=SIWOYBX26OXQ1WMS&field1=0

void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd = "GET /"+path+" HTTP/1.0\r\n\r\n";
  //AT+CIPSTART="TCP","192.168.20.200",80
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"";
  connect_wifi(atTcpPortConnectCmd,1000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd+=len;
  connect_wifi(atSendCmd,1000);
  connect_wifi(atHttpGetCmd,1000);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
//    lcd.clear();
//    lcd.print(cmd);
    Serial.println(cmd);
    Serial1.println(cmd); 
    while(Serial1.available())
    {
      if(Serial1.find("OK"))

      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
   Serial.println("OK");
//   lcd.clear();
//        lcd.setCursor(0,1);
//      lcd.print("OK");
  }
  else
  {
   Serial.println("Error");
//         lcd.setCursor(0,1);
//      lcd.print("Error");
  }
}
