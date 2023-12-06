#include "arduino_secrets.h"

const int servoPin = 9;  // Het pinnummer waarop de servo is aangesloten
const int trigPin = 10;
const int echoPin = 11;

float duration, distance;  

void setup() {
    Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Succesvol verbonden met");
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  Serial.begin(9600); // Initialiseren van de seriële communicatie met een baudrate van 9600
  pinMode(servoPin, OUTPUT);  // Zet servoPin als uitgang
  digitalWrite(servoPin, HIGH);  // Zet pin 9 in eerste instantie aan (stuurt het signaal naar de servo)

  	pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  
	Serial.begin(9600);  

}

void loop() {
  
    mqttClient.poll();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Send message to topicEmpty
    mqttClient.beginMessage(topicEmpty);
    mqttClient.print("false ");
    mqttClient.print(count);
    mqttClient.endMessage();

    // Send message to other topics with appropriate values
    mqttClient.beginMessage(topic);
    mqttClient.print(count);
    mqttClient.endMessage();

    mqttClient.beginMessage(topicLastTime);
    mqttClient.print(count);
    mqttClient.endMessage();

    mqttClient.beginMessage(topicCapacity);
    mqttClient.print("capacity_data_here");
    mqttClient.endMessage();

    Serial.println(count);
    count++;
  }


  digitalWrite(trigPin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trigPin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trigPin, LOW);  

  duration = pulseIn(echoPin, HIGH);  

  distance = (duration*.0343)/2;  

	Serial.print("Distance: ");  
	Serial.println(distance);  
	delay(500);  


  // Kijk of er gegevens beschikbaar zijn om te lezen
  if (Serial.available() > 0) {
    // Lees de invoer en geef deze weer
    String input = Serial.readStringUntil('\n'); // Lees de volledige regel tot aan het newline-teken

    // Controleer of de ingevoerde tekst "stop" is
    if (input.equals("stop")) {
      digitalWrite(servoPin, LOW);
      Serial.println("Programma gestopt.");
    }

    if (input.equals("play")) {
      digitalWrite(servoPin, HIGH);
      Serial.println("Programma gestart");
    }
  }
}

