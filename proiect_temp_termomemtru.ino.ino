#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
  #ifdef SPARK
    #include "ThingSpeak/ThingSpeak.h"    // You have to download ThingSpeak  library
  #else
    #include "ThingSpeak.h"
  #endif

#define myPeriodic 15 //in sec | Thingspeak pub is 15sec
#define ONE_WIRE_BUS 2  // DS18B20 on arduino pin2 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float prevTemp = 0;
const char* server = "api.thingspeak.com";
//String apiKey ="H1WQ9PHH1W0L1BTR";
unsigned long mySensorsChannelNumber = 424400;            //Replase this code with your thingSpeak channel ID
unsigned long myCommandsChannelNumber = 424424;           //Replase this code with your thingSpeak channel ID

const char * myWriteAPIKey_sensors = "H1WQ9PHH1W0L1BTR";  //Replace with your channel Write Key
const char * myWriteAPIKey_commands = "MOMBYC34QSZE7A1F"; //Replace with your channel Write Key. This key is not used in this example
                                                          //Use this key from android device  
const char* MY_SSID = "Chromosome Studio"; 
const char* MY_PWD = "icu2icu2";
WiFiClient  client;
int sent = 0;
float targetValue=12.0;
void setup() {
  Serial.begin(115200);
  ThingSpeak.begin(client);
  connectWifi();
}

void loop() {
  float temp;
  //char buffer[10];
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  //String tempC = dtostrf(temp, 4, 1, buffer);//handled in sendTemp()
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temp);
  
  
  sendReceiveTeperatureTS(temp);
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

void sendReceiveTeperatureTS(float temp)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");

          targetValue = ThingSpeak.readFloatField(myCommandsChannelNumber, 1);
          Serial.println("Send Value"+ String(targetValue));
          
          ThingSpeak.setField(1,temp);
          ThingSpeak.writeFields(mySensorsChannelNumber, myWriteAPIKey_sensors);                  // write two values

   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
