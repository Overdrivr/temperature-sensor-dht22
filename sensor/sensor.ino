#include "DHT.h"
#define DHTPIN 7  
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// Logger shield
#include <SD.h>
#include <SPI.h>
File myFile;

// Real-time clock
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
uint32_t before;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht.begin(); 
  RTC.begin();

  before = 0;
    
  if (!SD.begin(10)) {
    return;
  }
  
  myFile = SD.open("salon.txt", FILE_WRITE);
  
  if(!myFile) {
    return;
  }
  
  if (!RTC.isrunning()) {
    myFile.println("RTC issue");
    myFile.close();
    return;
  }

  RTC.adjust(DateTime(__DATE__, __TIME__));

  myFile.println("unixtime(s)\thum(%)\ttemp(c)\theatIndex");
  myFile.close();
}

void loop() { 
  DateTime now = RTC.now();
  uint32_t nowunix = now.unixtime();
  
  // 1 point every minute
  if(nowunix - before > 60)
  {
    before = nowunix;
     
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      myFile = SD.open("salon.txt", FILE_WRITE);
      myFile.print(nowunix);
      myFile.print("\t");
      myFile.println("Reading error");
      myFile.close();
    }
    else
    {
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false); 
      
      myFile = SD.open("salon.txt", FILE_WRITE);
      myFile.print(nowunix);
      myFile.print("\t");
      myFile.print(h);
      myFile.print("\t");
      myFile.print(t);
      myFile.print("\t");
      myFile.println(hic);
      myFile.close();
    }
  }
}
