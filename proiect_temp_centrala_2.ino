#include <ESP8266WiFi.h>
  #ifdef SPARK
    #include "ThingSpeak/ThingSpeak.h"    // You have to download ThingSpeak  library
  #else
    #include "ThingSpeak.h"
  #endif

#define myPeriodic 15 //in sec | Thingspeak pub is 15sec
#define RELAY 13 // GPIO4

float prevTemp = 0;
bool heating = false;
const char* server = "api.thingspeak.com";
unsigned long mySensorsChannelNumber = 424400;            //Replase this code with your thingSpeak channel ID
unsigned long myCommandsChannelNumber = 424424;           //Replase this code with your thingSpeak channel ID

const char * myWriteAPIKey_sensors = "H1WQ9PHH1W0L1BTR";  //Replace with your channel Write Key
const char * myWriteAPIKey_commands = "MOMBYC34QSZE7A1F"; //Replace with your channel Write Key. This key is not used in this example
                                                          //Use this key from android device  
const char* MY_SSID = "leChateau"; 
const char* MY_PWD = "Ianuarie!2345";
WiFiClient  client;
int sent = 0;
float targeTemperaturetValue=12.0;
float measuredTemperaturetValue=12.0;
void setup() {
  pinMode(RELAY, OUTPUT);
  Serial.println("RELAY: "+ String(HIGH)); 
  digitalWrite(RELAY, HIGH);
  Serial.begin(115200);
  ThingSpeak.begin(client);
  connectWifi();
}

void loop() {   
  receiveTeperatureTS();
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

void operateThermostat(float targeTemperaturet, float measuredTemperaturet)
{
    Serial.println("heating "+String(heating));
  if ((targeTemperaturet+0.5)> measuredTemperaturet)startHeat();
  else stopHeat();
  
}//end thermoperation

void receiveTeperatureTS()
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");

  targeTemperaturetValue = ThingSpeak.readFloatField(myCommandsChannelNumber, 1);
  measuredTemperaturetValue = ThingSpeak.readFloatField(mySensorsChannelNumber, 1);  
  operateThermostat(targeTemperaturetValue, measuredTemperaturetValue);
  ThingSpeak.setField(2,targeTemperaturetValue);
  ThingSpeak.setField(3,heating);
  ThingSpeak.writeFields(mySensorsChannelNumber, myWriteAPIKey_sensors);  
  Serial.println("Last target value"+ String(targeTemperaturetValue));
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(measuredTemperaturetValue);

   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
