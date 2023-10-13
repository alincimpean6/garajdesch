#define BLYNK_TEMPLATE_ID "TMPLJCTVqIP5"
#define BLYNK_DEVICE_NAME "Usi"
#define BLYNK_AUTH_TOKEN "HEt11QqQLrVXKTVeQiPlUh6nJ-TqvGUg"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <TimeLib.h>

char auth[] = BLYNK_AUTH_TOKEN;

#define W5100_CS  10
#define SDCARD_CS 4

#define START_VAL "HIGH"
#define END_VAL "LOW"

const int REL_GARAJ = 5;
const int REL_POARTA = 8;
const int REL_USA = 7;
const int REL_HEATER = 9;

boolean heaterState = false;
unsigned long previousMillis = 0;
const unsigned long heaterDuration = 1200000;  
unsigned int heatingMinutes = 0;
unsigned int timeArray[5]; 

void setup() {

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH);

  setSyncInterval(3600);

  Blynk.begin(auth);
  
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V5,0);  
  
  pinMode(REL_GARAJ, OUTPUT);
  digitalWrite(REL_GARAJ, HIGH);
  pinMode(REL_POARTA, OUTPUT);
  digitalWrite(REL_POARTA, HIGH);
  pinMode(REL_USA, OUTPUT);
  digitalWrite(REL_USA, HIGH);
  pinMode(REL_HEATER, OUTPUT);
  digitalWrite(REL_HEATER, HIGH);
}


BLYNK_WRITE(V5)
{
  if(param.asInt() == 1)
  {
    digitalWrite(REL_GARAJ, LOW);
    Blynk.virtualWrite(V5,1); 
    delay(300);
    digitalWrite(REL_GARAJ, HIGH);
    Blynk.virtualWrite(V5,0);
  }
}

BLYNK_WRITE(V8)
{
  if(param.asInt() == 1)
  {
    digitalWrite(REL_POARTA, LOW);
    Blynk.virtualWrite(V8,1); 
    delay(300);
    digitalWrite(REL_POARTA, HIGH);
    Blynk.virtualWrite(V8,0); 
  }
}

BLYNK_WRITE(V7)
{
  if(param.asInt() == 1)
  {
    digitalWrite(REL_USA, LOW);
    Blynk.virtualWrite(V7,1); 
    delay(300);
    digitalWrite(REL_USA, HIGH);
    Blynk.virtualWrite(V7,0); 
  }
}

BLYNK_WRITE(V9)
{
  if(param.asInt() == 1)
  {
    Blynk.virtualWrite(V9,1);
    heaterState = true;
    } else {
    Blynk.virtualWrite(V9,0); 
    heaterState = false;
  }
}

BLYNK_WRITE(V0)
{
  heatingMinutes = param.asInt();
}

BLYNK_WRITE(V6) {
  TimeInputParam t(param);

  if (t.hasStartTime())
  {
    String startHour = t.getStartHour();
    timeArray[0] = startHour.toInt(); 

    String startMin = t.getStartMinute();
    timeArray[1] = startMin.toInt(); 
  }

  if (t.hasStopTime())
  {
    String stopHour = t.getStopHour();
    timeArray[2] = stopHour.toInt(); 

  }
}



void loop() {
  if (!Blynk.connected()) {
    Blynk.connect();
    Blynk.logEvent("connected");
  }
  Blynk.run();

  unsigned long currentTime = now();
  //restartArduino(currentTime);
  heaterLogic(currentTime);
}

BLYNK_CONNECTED() { 
  Blynk.syncVirtual(V0); 
  Blynk.syncVirtual(V6); 
}

void heaterLogic(unsigned long currentTime) {
  if (heaterState) {
    int currentHour = hour(currentTime);
    int currentMinute = minute(currentTime);

    if ((currentMinute == timeArray[1] || currentMinute == heatingMinutes) && currentHour >= timeArray[0] && currentHour <= timeArray[2]) {
      digitalWrite(REL_HEATER, (currentMinute == timeArray[1]) ? HIGH : LOW);
      Blynk.logEvent("heater_sts");
    }
  }
}

void restartArduino(unsigned long currentTime) {
  if (hour(currentTime) == 4 && minute(currentTime) == 30) {
  Blynk.logEvent("restart");
  ESP.restart();
  }
}
