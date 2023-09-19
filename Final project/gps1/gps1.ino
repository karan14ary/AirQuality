#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define RXPin 8
#define TXPin 9

SoftwareSerial gpsSerial(RXPin, TXPin);

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      delay(1000);
    }
  }
}
