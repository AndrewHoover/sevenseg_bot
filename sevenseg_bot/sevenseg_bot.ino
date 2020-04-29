/* Freeform "SevenSeg" Bot Workshop with special guest Mohit Bhoite April 25th, 2020, 1:30pm
 *  Requires libraries:
 *    RTClib by Adafruit
 *    SevSeg by Dean Reading
 *    DHT sensor library by Adafruit
 */

#include "SevSeg.h"
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"

#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define LIGHTPIN 1
#define BUTTONPIN 13

char temp[5];
char humidity[5];
char cltime[5];

int mode=0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const long modeInterval = 5000;
const long lightOnInterval = 10000;

bool lightOn = false;

SevSeg sevseg; //Instantiate a seven segment controller object
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;

void setup() {

  Serial.begin(9600);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  
  byte numDigits = 4;
  byte digitPins[] = {A5, A2, A1, 12};
  byte segmentPins[] = {A4, A0, 10, 7, 5, A3, 11, 9};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  dht.begin();
  rtc.begin();

  digitalWrite(LIGHTPIN, HIGH);
  lightOn = true;
  
}

void loop() {

  if (digitalRead(BUTTONPIN)==false){
    digitalWrite(LIGHTPIN,true);
    lightOn=true;
    previousMillis2 = millis();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis1 >= modeInterval) {
    previousMillis1 = currentMillis;
    if (mode<2){
      mode++;
    }else{
      mode=0;
    }
  }

  if ((currentMillis - previousMillis2 >= lightOnInterval) && (lightOn == true)) {
    previousMillis1 = currentMillis;
    digitalWrite(LIGHTPIN,false);
    lightOn=false;
  }

  switch (mode){
    case 0:
      getTime();
      sevseg.setChars(cltime);
      break;
    case 1:
      getTemp();
      sevseg.setChars(temp);
      break;
    case 2:
      getHumidity();
      sevseg.setChars(humidity);
      break;
  }

  sevseg.refreshDisplay(); // Must run repeatedly
}

void getTemp(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  int intemp = int(f);
  sprintf(temp, "%s%d", ".T.", intemp);
  
  //float h = dht.readHumidity();
  //int inhum = int(h);
  //sprintf(humidity, "%s%d", ".H.", inhum);
  
}

void getHumidity(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);
  //int intemp = int(f);
  //sprintf(temp, "%s%d", ".T.", intemp);
  
  float h = dht.readHumidity();
  int inhum = int(h);
  sprintf(humidity, "%s%d", ".H.", inhum);
  
}

void getTime(){
  DateTime now = rtc.now();
  int hr = 0;
  int mn = 0;
  if (now.hour()<12){
    hr = now.hour();
  }else{
    hr = (now.hour()-12);
  }

  mn = now.minute();
  
  if (hr < 10){
    if (mn < 10){
      sprintf(cltime, ".%d%s%d", hr, ".0", mn);
    }else if(mn == 0){
      sprintf(cltime, ".%d%s", hr, ".00");
    }else{
      sprintf(cltime, ".%d%s%d", hr, ".", mn);
    }
  }else{
    if (mn < 10){
      sprintf(cltime, "%d%s%d", hr, ".0", mn);
    }else if(mn == 0){
      sprintf(cltime, "%d%s", hr, ".00");
    }else{
      sprintf(cltime, "%d%s%d", hr, ".", mn);
    }
  }
}
