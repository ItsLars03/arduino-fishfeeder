
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>

char ssid[] = "Ziggo17CF4";
char pass[] = "PQH7fyf7zREm";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "xa679108.ala.us-east-1.emqxsl.com";
int port = 8883;

const char topic[] = "larsderover/activated";
const char topicEmpty[] = "larsderover/isempty";
const char topicLastTime[] = "larsderover/lasttimefeeded";
const char topicCapacity[] = "larsderover/capacity";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;



