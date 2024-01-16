
// Include libraries
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <Servo.h>

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Wifigegevens
// char ssid[] = "IoTatelierF2144";
// char pass[] = "IoTatelier";
char ssid[] = "Ziggo17CF4";
char pass[] = "PQH7fyf7zREm";
// const char broker[] = "192.168.144.1";
// int port = 1883;

// MQTT-gegevens
const char broker[] = "test.mosquitto.org";
int port = 1883;

// Leeg/vol notificatie
const char topicEmpty[] = "larsderover/isempty";
// Starttopic
const char topicStart[] = "larsderover/start";
// Notificatietopic
const char topicMessage[] = "larsderover/message";

// Voergegevens
// Aantal seconde dat de moter moet draaien
int feedingDuration = 7;
// Tijd dat de moter moet draaien IN  ARDUINO SCRETS
String origineleTijd = "23:02:00";


