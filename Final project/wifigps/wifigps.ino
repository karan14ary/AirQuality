#include <SoftwareSerial.h>
#include <TinyGPS++.h>
int GPSBaud = 9600;
String AP = "Karan";       // Hotspot Name
String PASS = "123456789"; // Hotspot PASSWORD
String API = "AB6YLMLEYD3NFB1N";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand;
String Latitude;
String Longitude;
TinyGPSPlus gps;
boolean found = false;
int valSensor = 1;
SoftwareSerial gpsSerial(8,9);
SoftwareSerial esp8266(6,7);
 
void setup() {
  Serial.begin(9600);
 
 esp8266.begin(9600);
   sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  esp8266.end();
}

void loop() {


 gpsSerial.begin(9600);


   Latitude = String(gps.location.lat(), 5);
   Longitude = String(gps.location.lng(),5);

    Serial.print("Latitude: ");
    Serial.println(Latitude);
    Serial.print("Longitude: ");
    Serial.println(Longitude);
gpsSerial.end();
 esp8266.begin(9600);
 String getData = "GET /update?api_key="+ API +"&field1="+Latitude+"&field2="+Longitude;
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
