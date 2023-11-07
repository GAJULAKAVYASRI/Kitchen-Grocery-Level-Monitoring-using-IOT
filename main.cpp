/*
AWS Iot Core
This example needs https://github.com/esp8266/arduino-esp8266fs-plugin
It connects to AWS IoT server then:
- publishes "hello world" to the topic "outTopic" every two seconds
- subscribes to the topic "inTopic", printing out any messages
*/
#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <time.h>
#include <NewPing.h>

#define trigPin1 D1
#define echoPin1 D2
#define trigPin2 D3
#define echoPin2 D4
#define trigPin3 D7
#define echoPin3 D8
#define MAXDISTANCE 400

NewPing ultrasonic1(trigPin1, echoPin1, MAXDISTANCE);
NewPing ultrasonic2(trigPin2, echoPin2, MAXDISTANCE);
NewPing ultrasonic3(trigPin3, echoPin3, MAXDISTANCE);


//define variable
long duration1;
int distance1;
int maxlength1 = 20;
long duration2;
int distance2;
int maxlength2 = 20;
long duration3;
int distance3;
int maxlength3 = 20;

// Update these with values suitable for your network.
const char* ssid = "Sandeep";
const char* password = "12345678";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const char* AWS_endpoint = "a1v0azg8apkbgp-ats.iot.ap-south-1.amazonaws.com"; //MQTT broker ip
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();
}
WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;
void setup_wifi() {
delay(10);
// We start by connecting to a WiFi network
espClient.setBufferSizes(512, 512);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
timeClient.begin();
while(!timeClient.update()){
timeClient.forceUpdate();
}
espClient.setX509Time(timeClient.getEpochTime());
}
void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESPthing")) {
Serial.println("connected");
// Once connected, publish an announcement...
client.publish("outTopic", "hello world");
// ... and resubscribe
client.subscribe("inTopic");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
char buf[256];
espClient.getLastSSLError(buf,256);
Serial.print("WiFiClientSecure SSL error: ");
Serial.println(buf);
// Wait 5 seconds before retrying
delay(5000);
}
}
}


void setup() {
Serial.begin(115200);
Serial.println("Beginning Program");
Serial.setDebugOutput(true);
// initialize digital pin LED_BUILTIN as an output.
setup_wifi();
delay(1000);
if (!SPIFFS.begin()) {
Serial.println("Failed to mount file system");
return;
}
Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
// Load certificate file
File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
if (!cert) {
Serial.println("Failed to open cert file");
}
else
Serial.println("Success to open cert file");
delay(1000);
if (espClient.loadCertificate(cert))
Serial.println("cert loaded");
else
Serial.println("cert not loaded");
// Load private key file
File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
if (!private_key) {
Serial.println("Failed to open private cert file");
}
else
Serial.println("Success to open private cert file");
delay(1000);
if (espClient.loadPrivateKey(private_key))
Serial.println("private key loaded");
else
Serial.println("private key not loaded");
// Load CA file
File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
if (!ca) {
Serial.println("Failed to open ca ");
}
else
Serial.println("Success to open ca");
delay(1000);
if(espClient.loadCACert(ca))
Serial.println("ca loaded");
else
Serial.println("ca failed");
Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

}


void loop() {
if (!client.connected()) {
reconnect();
}
client.loop();

distance1 = ultrasonic1.ping_cm();
distance2 = ultrasonic2.ping_cm();
distance3 = ultrasonic3.ping_cm();

Serial.print("Free space in the container 1 is: ");
Serial.print(distance1);Serial.println("cm");
Serial.print("Free space in the container 2 is: ");
Serial.print(distance2);Serial.println("cm");
Serial.print("Free space in the container 3 is: ");
Serial.print(distance3);Serial.println("cm");
delay(1000);


StaticJsonDocument<400> doc;
doc["distance1"] = distance1;
doc["distance2"] = distance2;
doc["distance3"] = distance3;
doc["maxlength1"] = maxlength1;
doc["maxlength2"] = maxlength2;
doc["maxlength3"] = maxlength3;
char jsonBuffer[512];
serializeJson(doc, jsonBuffer); // print to client
 
client.publish("outTopic", jsonBuffer);
Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
delay(1000);


}
