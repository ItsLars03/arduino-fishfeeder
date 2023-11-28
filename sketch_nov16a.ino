// Definieer de pinnen voor de LED's
const int ledPin1 = 13;
const int ledPin2 = 12;
const int extraPin = 11; // Kies een andere vrije pin voor het extra apparaat

int een = 0;

void setup() {
  // Definieer de pinnen als output
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(extraPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Maak de eerste LED hoog (aan) en de tweede LED laag (uit)
  digitalWrite(ledPin1, HIGH);
  delay(250);  // Wacht 1 seconde

    digitalWrite(ledPin2, HIGH);
  digitalWrite(extraPin, HIGH); // Zet het extra aangesloten apparaat aan
    delay(200);  // Wacht 1 seconde

  // Maak de eerste LED laag (uit) en de tweede LED hoog (aan)
  digitalWrite(ledPin1, LOW); 
  delay(250);

  digitalWrite(ledPin2, LOW);
  digitalWrite(extraPin, LOW); // Zet het extra aangesloten apparaat uit
  delay(200);  // Wacht 1 seconde

  Serial.print("Hello ");
  Serial.println(een++);
  Serial.println("it works :)");
}
