#include "arduino_secrets.h"

// Definieer variabelen
// Sensor pinnen
int trigPin = 10;
int echoPin = 11;
// Error LED pin
int errorLed = 8;
// Knop pin
int feederButton = 13;
// Pulsduur en afstand van sensor
float duration, distance;  
// Overige variabelen met een waarde true of false
bool isEmpty, isStarted, feedButton, fault, activated;
String message, currentTime;

// Pak de pinnen waarop de RTC is aangesloren
ThreeWire myWire(4,5,3); // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);
// Defineer servo
Servo myServo;

void setup() {
  // Zet de de pinnen op de goede mode
  pinMode(feederButton, INPUT_PULLUP);
  pinMode(errorLed, OUTPUT);

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  // Controlleer de RTC op fouten
  if (!Rtc.IsDateTimeValid()) 
    {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("[ERROR] RTC verloor verbinding!");
      Rtc.SetDateTime(compiled);

      fault = true;
    } else {
      fault = false;

      Serial.println("[OK] Succesvol verbinding gemaakt met de RTC!");
    }

  if (Rtc.GetIsWriteProtected())
    {
      Serial.println("[!] RTC was schrijfbeschermd, schrijven inschakkelen...");
      Rtc.SetIsWriteProtected(false);
    }

  if (!Rtc.GetIsRunning())
    {
      Serial.println("[!] RTC was niet aan het draaien, start nu...");
      Rtc.SetIsRunning(true);
    }

    // Pak de tijd en datum van de RTC
    RtcDateTime now = Rtc.GetDateTime();
    
  if (now < compiled) 
    {
      Serial.println("[!] RTC is ouder dan compiled tijd!  (DateTime aan het updaten...)");
      Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) {
      Serial.println("[!] RTC is nieuwer dan compiled tijd. (Dit was verwacht)");
    }
    else if (now == compiled) 
    {
      Serial.println("[!] RTC is hetzelfde als de compiled tijd! (Niet verwacht, maar alles is OK.)");
    }


  while (!Serial);
  
  Serial.print("Proberen verbinding te maken met SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Succesvol verbonden met");
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.print("Proberen om verbinding te maken met de MQTT-broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT-verbinding mislukt! Foutcode = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("Je bent verbonden met de MQTT-broker!");
  Serial.println();

  // Initialiseren van de seriële communicatie met een baudrate van 9600
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  

  // Set de message receive callback voor topic
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Abboneren op topic: ");
  Serial.println(topicStart);

  // Op topic abboneren
  mqttClient.subscribe(topicStart);
}



void loop() {
  // Haal de nieuwste wijziging op
  mqttClient.poll();
  
  // Haal string uitelkaar om het aantal seconde toe te voegen
  int uren = origineleTijd.substring(0, 2).toInt();
  int minuten = origineleTijd.substring(3, 5).toInt();
  int seconden = origineleTijd.substring(6).toInt() + feedingDuration;
  
  // Sla de bijgewerkte tijd op in een nieuwe variabele
  String bijgewerkteTijd = "";
  if (uren < 10) bijgewerkteTijd += "0";
  bijgewerkteTijd += String(uren) + ":";
  if (minuten < 10) bijgewerkteTijd += "0";
  bijgewerkteTijd += String(minuten) + ":";
  if (seconden < 10) bijgewerkteTijd += "0";
  bijgewerkteTijd += String(seconden);

  // Zet de tijden in de console
  // Tijd dat de moter zal starten
  Serial.print("[Time] Starttijd: ");
  Serial.println(origineleTijd);
  // Tijd dat de moter zal stoppen
  Serial.print("[Time] Stoptijd: ");
  Serial.println(bijgewerkteTijd);
  // Tijd die het op dit moment is volgens de RTC-module
  Serial.print("[Time] Huidige Tijd: ");
  Serial.println(currentTime);

  // Haal de huidige datum en tijd op vanuit een RTC-object.
  RtcDateTime now = Rtc.GetDateTime();

  // Print de verkregen datum en tijd af met een aangepaste functie.
  printDateTime(now);

  // Check of de RTC nog beschikbaar is
  if (!now.IsValid())
    {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("[ERROR] RTC heeft de verbinding verloren!");

      mqttClient.beginMessage(topicMessage);
      mqttClient.print("[!] RTC heeft de verbinding verloren!");
      mqttClient.endMessage();

      // Laad de fault led branden
      fault = true;
    } else {
      // Zet de fault led uit
      fault = false;
    }

  if (!isEmpty){
    if (currentTime == origineleTijd) {
      if (!myServo.attached()) {
        myServo.attach(9);
      }
      
      // Zet in de console dat de moter is gestart
      Serial.println("[Servo] Moter gestart");

      // Start de moter
      myServo.write(90);  // Move the servo to the 90-degree position
      myServo.write(0);   // Move the servo to the 0-degree position
      
      // Zet de isStarted waarde naar true om de moter weer te laten stoppen
      isStarted = true;
    }
  }
  
  // Check of de moter is gestart
  if (isStarted){
    // Stop de moter wanneer de eindtijd is bereikt
    if (currentTime == bijgewerkteTijd){
      // Zet in de console dat de moter is gestopt
      Serial.println("[Servo] Moter gestopt");
      // Zet de moter weer uit 
      myServo.detach();
      // Zet de isStarted waarde weer naar false
      isStarted = false;
    } 
  }

  // Wanneer fault true is voer deze led code uit
  if (fault){
    // Zet de error led voor 150ms aan
    digitalWrite(errorLed, HIGH);
    delay(150); // Houd de LED aan gedurende 250 ms (1/4 van een seconde)
    
    // Zet de error led voor 150ms uit
    digitalWrite(errorLed, LOW);
    delay(150); // Wacht 250 ms (1/4 van een seconde) terwijl de LED is uitgeschakeld
  }

  // MQTT-CLIENT
  // Send message to topicEmpty
    mqttClient.beginMessage(topicEmpty);
    if (isEmpty){
      mqttClient.print("LEEG");
    } else{
      mqttClient.print("VOL");
    }
    mqttClient.endMessage();




}

// Callbackfunctie voor topic
void onMqttMessage(int messageSize) {
  // Lees de tekst rechtstreeks van de MQTT-client
  message = mqttClient.readString();
  if (message == "true"){
    activated = true;
  } else {
    activated = false;
  }
}

// Callbackfunctie voor de tijd
void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  // Pak alleen de seconden, minuten en uren uit de RTC
  snprintf_P(datestring,
              sizeof(datestring),
              PSTR("%02u:%02u:%02u"),
              dt.Hour(),
              dt.Minute(),
              dt.Second());

  // Zet de tijd in een variable
  currentTime = datestring;
}

