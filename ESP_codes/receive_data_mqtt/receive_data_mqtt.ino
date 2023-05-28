#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>  
#include <ArduinoJson.h>

//LEDS PINS
#define AC            2
#define HEATER        4
#define HUMIDIFIER    0
#define LIGHTS        5


//VARIABLES TO MQTT AND WIFI
const char *ssid                              = "WIFI-NETWORK-NAME";                       
const char *password                          = "WIFI-PASSWORD";                              

const char *ID                                = "final-project-device";                      
const char* BROKER_MQTT                       = "mqtt.tago.io";                          
unsigned int PORT                             = 8883;
const char *TOKEN                             ="DEVICE-TOKEN";

const char *USER                              = "USERNAME";

unsigned long previousMillis = 0;

BearSSL::WiFiClientSecure wClient;
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
    
  Serial.println("MQTT Client   : [ trying connection ]");
    
    if (MQTTclient.connect(ID,USER,TOKEN)) {
      Serial.println("MQTT Client   : [ broker connected ]");

    } else {
      Serial.print("MQTT Client   : [ failed, rc= ");
      Serial.print(MQTTclient.state());
      Serial.println(" ]");

      delay(5000);
      setup_wifi();
    }
  }
}

// CALLBACK RECEIVE MESSAGE

void callback(char* topic, byte* payload, unsigned int length) {

  StaticJsonDocument<64> doc;

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    int ac_state = doc["ac"];
    int humidifier_state = doc["humidifier"];
    int heater_state = doc["heater"];
    int lights_state = doc["lights"];

    Serial.println("----------------");
    Serial.print("AC:");
    Serial.println(ac_state);
    Serial.print("Humidifier:");
    Serial.println(humidifier_state);
    Serial.print("Heater:");
    Serial.println(heater_state);
    Serial.print("Lights:");
    Serial.println(lights_state);
    Serial.println("----------------");

    digitalWrite(AC, ac_state); 
    digitalWrite(HEATER, heater_state); 
    digitalWrite(HUMIDIFIER, humidifier_state); 
    digitalWrite(LIGHTS, lights_state); 
  }
 
}

void setup() {
  Serial.begin(115200);                                                                     
  delay(1000);

  setup_wifi();         

  wClient.setInsecure();

  MQTTconnect();  

  MQTTclient.setCallback(callback);

  MQTTclient.subscribe("device/states");

  Serial.println("");
  Serial.println("Setup         : [ finished ]");
  Serial.println("");

  pinMode(AC, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(LIGHTS, OUTPUT);

  digitalWrite(AC, HIGH); 
  digitalWrite(HEATER, HIGH); 
  digitalWrite(HUMIDIFIER, HIGH); 
  digitalWrite(LIGHTS, HIGH); 
  delay(1000);
  digitalWrite(AC, LOW); 
  digitalWrite(HEATER, LOW); 
  digitalWrite(HUMIDIFIER, LOW); 
  digitalWrite(LIGHTS, LOW); 

}

void loop() {
  MQTTclient.loop();
  
}
