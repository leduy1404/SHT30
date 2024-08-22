#include <PubSubClient.h> 
#include <ESP8266WiFi.h> 
#include <Wire.h> 
#include <ArduinoJson.h> 
#include "SHT3x.h"


#define Addr 0x45
#define WIFI_SSID "LOTODA" //Enter your WIFI SSID 
#define WIFI_PASSWORD "0388187172" //Enter your WiFi password 
#define BUILTIN_LED 2 

char LotodaServer[] = "app.lotoda.vn"; //app.lotoda.vn is the default servername 
int LotodaPort = 1883; //1883 is the default port 
char deviceId[] = "LotodaEsp8266-0001"; //every device should have a different name 
char topic[] = "Y9L4MytvWM/SHT30"; //Enter <User_ID_Key> from LOTODA system, the topic should be different for each device as well 
//Please create this new topic on LOTODA mobile app, in this case is "lamp/power" 
char User_ID_Key[] = "Y9L4MytvWM"; //Enter <User_ID_Key> from LOTODA system 
char Password_ID_Key[] = "xY1SI02Y9Oi2USoD25Fb"; //Enter <Password_ID_Key> from LOTODA system 
WiFiClient wifiClient; 
SHT3x Sensor;
PubSubClient client(wifiClient); 
long lastMsg = 0; 
char msg[50]; 
int value = 0;


void callback(char* topic, byte* payload, unsigned int length) { 
 // handle message arrived 
  Serial.print("Message arrived ["); 
  Serial.print(topic); 
  Serial.print("] "); 
  for (int i = 0; i < length; i++) { 
    Serial.print((char)payload[i]); 
  } 
  Serial.println(); 
} 

void connect() { 
  while (!client.connected()) { 
  if ( WiFi.status() != WL_CONNECTED) { 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
    while (WiFi.status() != WL_CONNECTED) { 
     delay(500); 
      Serial.print("."); 
    } 
    Serial.print("Connected to "); 
    Serial.println(WIFI_SSID); 
  } 
  client.setServer(LotodaServer, LotodaPort); 
  client.setCallback(callback); 
  if (client.connect(deviceId, User_ID_Key, Password_ID_Key)) { 
    client.subscribe(topic); 
    Serial.println("Connected to LOTODA's Server"); 
  } else { 
      Serial.print("[FAILED] [ rc = "); 
      Serial.print(client.state() ); 
      Serial.println(" : retrying in 5 seconds]"); 
      delay(5000); 
    } 
  }   
}

void setup() {
   // Initialise I2C communication as MASTER
  Sensor.Begin();
  delay(300);
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output led 
  Serial.begin(115200); 
  delay(20); 
  Serial.println("Let' start now"); 
} 
void loop() { 
  if ( !client.connected() ) { 
    connect(); 
  } 
  client.loop(); 
  Sensor.UpdateData();
  Serial.print("Temperature: ");
  Serial.print(Sensor.GetTemperature());
  Serial.write("\xC2\xB0"); //The Degree symbol
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(Sensor.GetRelHumidity());
  Serial.println("%");
  delay(500);

  float c = Sensor.GetTemperature();
  float h = Sensor.GetRelHumidity();

    
  //if (isnan(h) || isnan(c)) { 
      //Serial.println("Failed to read from SHT sensor!");
      //return; 
    //} 
  // Convert the value to a char array 
  char tempString[8]; 
  // float to string 
  dtostrf(c, 1, 2, tempString); 
  Serial.print("Temperature: "); 
  Serial.println(tempString);
char humString[8]; 
  dtostrf(h, 1, 2, humString); 
  Serial.print("Humidity: "); 
  Serial.println(humString); 
  String key1 = "{\"temperature\":"; 
  String key2 = ",\"humidity\":"; 
  String endkey = "}"; 
  String str = key1 + tempString + key2 + humString + endkey; // json message 
  char payloaddata[80]; 
  str.toCharArray(payloaddata, (str.length() + 1)); 
  Serial.println(str); 
  client.publish(topic, payloaddata); // publish message to server 
  
}