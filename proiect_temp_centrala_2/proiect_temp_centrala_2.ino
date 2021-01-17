#include <ESP8266WiFi.h>
  #ifdef SPARK
    #include "ThingSpeak/ThingSpeak.h"    // You have to download ThingSpeak  library
  #else
    #include "ThingSpeak.h"
  #endif
#include <NTPClient.h>
#include <WiFiUdp.h>

#define myPeriodic 15 //in sec | Thingspeak pub is 15sec
#define RELAY 13 // GPIO4

bool heating = false;
const char* server = "api.thingspeak.com";
unsigned long mySensorsChannelNumber = 424400;            //Replase this code with your thingSpeak channel ID
unsigned long myCommandsChannelNumber = 424424;           //Replase this code with your thingSpeak channel ID

const char * myWriteAPIKey_sensors = "H1WQ9PHH1W0L1BTR";  //Replace with your channel Write Key
const char * myWriteAPIKey_commands = "MOMBYC34QSZE7A1F"; //Replace with your channel Write Key. This key is not used in this example
                                                          //Use this key from android device  
const long utcOffsetInSeconds = 7200; // UTC+2
const char* MY_SSID = "leChateau"; 
const char* MY_PWD = "Ianuarie!2345";
const int manualMode = 0;
const int workdaysMode = 1;
const int dailyMode = 2;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WiFiClient  client;
int sent = 0;
float manTargeTempValue=12.0;
float autoTargeTempValue=12.0;
float currentTargeTempValue=12.0;
float measuredTemperatureValue=12.0;
int operatingMode = 0;
int startHour = 0;
int stopHour = 0;

void setup() {
  pinMode(RELAY, OUTPUT);
  Serial.println("RELAY: "+ String(HIGH)); 
  digitalWrite(RELAY, HIGH);
  Serial.begin(115200);
  ThingSpeak.begin(client);
  connectWifi();
  timeClient.begin();
}

void loop() {   
  connectAndCheckThermo();
  int count = myPeriodic;
  while(count--)
  delay(1000);
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void startHeat()
{
    digitalWrite(RELAY, LOW);
    Serial.println("RELAY: "+ String(LOW));  
    heating = true;
    Serial.println("Start Heat");  
}//end startHeat

void stopHeat()
{
    digitalWrite(RELAY, HIGH);
    Serial.println("RELAY: "+ String(HIGH));  
    heating = false;
    Serial.println("Stop Heat");
}//end stopHeat

bool isOfficeDay() 
{
  return (timeClient.getDay() > 0) && (timeClient.getDay() < 6); // Monday is 0
}

bool isDayBeforeOffice() 
{
  return (timeClient.getDay() < 5); // Monday is 0
}

void operateThermo(float targeTemperature) {
  if ((targeTemperature+0.5)> measuredTemperatureValue) startHeat();
  else stopHeat();  
  Serial.println("heating "+String(heating));
}

void runThermostatManually()
{
  operateThermo(manTargeTempValue);
  currentTargeTempValue = manTargeTempValue;
}

void runThermostatAutomatically()
{
  int currentHour = timeClient.getHours();
  if (isOfficeDay() || operatingMode == dailyMode) 
  {    
    if (currentHour == startHour && !heating)
    {
        operateThermo(autoTargeTempValue);
        currentTargeTempValue = autoTargeTempValue;
    }
  }

  if (currentHour == stopHour && heating); 
  {
    if (isDayBeforeOffice() || operatingMode == dailyMode)
    {
        currentTargeTempValue = autoTargeTempValue - 5;
    }
    else  
    {
        currentTargeTempValue = autoTargeTempValue - 7;
    }
    
    operateThermo(currentTargeTempValue);
  }
}

void operateThermostat()
{
    if (operatingMode == manualMode) runThermostatManually();
      else runThermostatAutomatically();
}

void readThingSpeakData()
{
  manTargeTempValue = ThingSpeak.readFloatField(myCommandsChannelNumber, 1);
  autoTargeTempValue = ThingSpeak.readFloatField(myCommandsChannelNumber, 2);
  operatingMode = ThingSpeak.readFloatField(myCommandsChannelNumber, 3);
  startHour = ThingSpeak.readFloatField(myCommandsChannelNumber, 4);
  stopHour = ThingSpeak.readFloatField(myCommandsChannelNumber, 5);

  measuredTemperatureValue = ThingSpeak.readFloatField(mySensorsChannelNumber, 1);  
}

void writeThingSpeakData()
{
  ThingSpeak.setField(2,currentTargeTempValue);
  ThingSpeak.setField(3,heating);
  ThingSpeak.writeFields(mySensorsChannelNumber, myWriteAPIKey_sensors);  
  Serial.println("Last target value"+ String(currentTargeTempValue));
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(measuredTemperatureValue);
}

void connectAndCheckThermo()
{  
  WiFiClient client;
  
  if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
  Serial.println("WiFi Client connected ");
  readThingSpeakData()
  operateThermostat();
  writeThingSpeakData();

   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
