 #include <WiFiClientSecure.h>
#include <PubSubClient.h>                
#include <ArduinoJson.h> 
#include "DHT.h"  
#include <Wire.h>
#include <Adafruit_BMP085.h>             

//SENSOR VARIABLES
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define LDRPIN 34
Adafruit_BMP085 bmp;

//VARIABLES TO MQTT AND WIFI
const char *ssid                              = "WIFI-NETWORK-NAME";                       
const char *password                          = "WIFI-PASSWORD";                                   

const char *ID                                = "final-project-device";                      
const char* BROKER_MQTT                       = "mqtt.tago.io";                          
unsigned int PORT                             = 8883;
const char *TOKEN                             ="DEVICE-TOKEN";

const char *USER                              = "USERNAME";


// TOPICS TO BE PUBLISHED
const char *TopicsToPublish[]                 = { 
                                                "data",
                                                "info"
                                              };

unsigned long previousMillis = 0;

WiFiClientSecure wClient;
PubSubClient MQTTclient(wClient);                           

//CONNECT WIFI

void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
    WiFi.mode(WIFI_STA);                              
    //WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.begin(ssid, password);                       

    int wait_passes = 0;
    while (WiFi.status() != WL_CONNECTED) {           
      delay(500);
      Serial.print(".");
      if (++wait_passes >= 20) { ESP.restart(); }       
    }

  Serial.print("\nWiFi connected");
  Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

}

//VERIFY WIFI CONNECTION

void VerifyWifi() {

  if (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0, 0, 0, 0)){          
      
    Serial.printf("error: WiFi not connected, reconnecting \n");
            
      WiFi.disconnect();
      setup_wifi();             

  } 

}

//MQTT CONNECTION

void MQTTconnect() {

  if(!MQTTclient.connected()) {                               
  
  Serial.println();
  Serial.println("MQTT Client   : [ not connected ]");

  MQTTclient.setServer(BROKER_MQTT, PORT);                    
  MQTTclient.setBufferSize(1024);
                      
    
    Serial.println("MQTT Client   : [ trying connection ]");
    
    if (MQTTclient.connect(ID,USER,TOKEN)) {
      Serial.println("MQTT Client   : [ broker connected ]");

      for(int i=0; i<=((sizeof(TopicsToPublish) / sizeof(TopicsToPublish[0]))-1); i++){

        Serial.print("MQTT Client   : [ publishing to ");
        Serial.print(TopicsToPublish[i]);
        Serial.println(" ]");
        
      }
    } else {
      Serial.print("MQTT Client   : [ failed, rc= ");
      Serial.print(MQTTclient.state());
      Serial.println(" ]");

      delay(5000);
      setup_wifi();
    }
  }
}

//SERIALIZE CONTENT TO PUBLISH

void SerializeAndPublish() {

  if (!MQTTclient.connected())                            
  {    MQTTconnect();      }

  char buffer[1024];                                      
   
  StaticJsonDocument<512> doc;

    JsonObject doc_0 = doc.createNestedObject();
    doc_0["variable"] = "WiFiRSSI";
    doc_0["value"] = WiFi.RSSI();
    doc_0["unit"] = "dB";

    JsonObject doc_1 = doc.createNestedObject();
    doc_1["variable"] = "temperature";
    doc_1["value"] = String(get_temperature(),2);
    doc_1["unit"] = "°C";
  
    JsonObject doc_2 = doc.createNestedObject();
    doc_2["variable"] = "humidity";
    doc_2["value"] = get_humidity();
    doc_2["unit"] = "%";

    JsonObject doc_3 = doc.createNestedObject();
    doc_3["variable"] = "luminosity";
    doc_3["value"] = get_luminosity();
    doc_3["unit"] = "";

    JsonObject doc_4 = doc.createNestedObject();
    doc_4["variable"] = "pressure";
    doc_4["value"] = get_pressure();
    doc_4["unit"] = "hPa";

    serializeJson(doc, buffer);

    Serial.printf("\nJSON Payload:");
      Serial.printf("\n");
    
    serializeJsonPretty(doc, Serial);                         
      Serial.printf("\n");
      Serial.println("MQTT Client   : [ Sending message to MQTT topic ]"); 
      Serial.println("");         

    MQTTclient.publish(TopicsToPublish[0], buffer);
       
}

float get_temperature(){
  return dht.readTemperature();
}

float get_humidity(){
  return dht.readHumidity();
}

uint16_t get_luminosity(){
  return analogRead(LDRPIN);
}

uint16_t get_pressure(){
  return (bmp.readPressure()/100);
}

void setup() {
  Serial.begin(115200);                                                                     
  delay(1000);

  setup_wifi();         

  wClient.setInsecure();

  MQTTconnect();  

  dht.begin();
  bmp.begin();

  Serial.println("");
  Serial.println("Setup         : [ finished ]");
  Serial.println("");

}

void loop() {
  MQTTclient.loop();  

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 30*1000) {
    
    previousMillis = currentMillis;

    SerializeAndPublish();

    Serial.print("Temperature(°C):");
    Serial.println(get_temperature());
    Serial.print("Humidity(%):");
    Serial.println(get_humidity());
    Serial.print("Luminosity:");
    Serial.println(get_luminosity());
    Serial.print("Pressure(hPa):");
    Serial.println(get_pressure());
    delay(2000);

  }
  

}
