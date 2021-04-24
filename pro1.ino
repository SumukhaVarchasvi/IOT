#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "Svk"
#define WIFI_PASSWORD "gottilla"
#define FIREBASE_HOST "https://greenhouse-eb07a-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "JHdyvblws8FCy0X1PMaLAYoCVWkQ9pnTpWJWGqtu"


FirebaseData my_database;       
#include "DHT.h"

#define DHTPIN D3 
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
int soilpin = A0;
int ldrpin = D8;
int smokepin = D6;
int motorpin = D1;
int ledpin =D7;
int buzpin =D4;
int fanpin = D5;


void setup() {
  //firebase
    Serial.begin(230400);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //soil
  pinMode(soilpin,INPUT);
  

  //LDR
  pinMode(ldrpin,INPUT);
  

  //smoke
  pinMode(smokepin,INPUT);

  pinMode(motorpin,OUTPUT);
  pinMode(ledpin,OUTPUT);
  pinMode(fanpin,OUTPUT);
  pinMode(buzpin,OUTPUT);
  


  //DHT
  Serial.println(F("DHTxx test!"));

  dht.begin();

}
void soilsensor() 
{
  int soilread=analogRead(soilpin);
  Serial.print("SOIL SENSOR:  ");
  Serial.println(soilread);
    Serial.println("\n");
    Firebase.set(my_database, "soil" ,soilread);
    if(soilread>=900)
    {
       Firebase.set(my_database, "pump" ,"1");
    }
    else
    {
      Firebase.set(my_database, "pump" ,"0");
    }
  
}
void smokesensor() 
{
  int x;
  x=analogRead(smokepin);
  Serial.print("smoke SENSOR:  ");
  Serial.println(x);
  Serial.println("\n");
  Firebase.set(my_database, "smoke" ,x);
  if(x<500)
  {
    Firebase.set(my_database, "buz" ,"1");
  }
  else{
    Firebase.set(my_database, "buz" ,"0");
  
  }
}
void ldrsensor()
{
   //ldr
 int Y = digitalRead(ldrpin);
 Serial.print("LDR SENSOR:  ");
  Serial.println(Y);
    Serial.println("\n");
  Firebase.set(my_database, "ldr" ,Y);
  if(Y==1)
  {
    Firebase.set(my_database, "led" ,"1");         
  }
  else
  {
    Firebase.set(my_database, "led" ,"0");
  }
}


void dhtsensor()
{
   // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println(F("°F"));
    Serial.println("\n");
  Firebase.set(my_database, "temp" ,t);
  Firebase.set(my_database, "humidity" ,h);
  if(t>=34)
  {
    Firebase.set(my_database, "fan" ,"1");
  }
  else
  {
    Firebase.set(my_database, "fan" ,"0");
  }
}
void loop() 
{
  digitalWrite(motorpin,LOW);
  {
    dhtsensor();
  }
     Firebase.getString(my_database,"fan");             // (objectname, tag name)
   String F = my_database.stringData();
   Serial.print("fan status");
     Serial.println(F);
  if(F=="1")
  {
     digitalWrite(fanpin,LOW);
     delay(5000);
     digitalWrite(fanpin,HIGH);
  }

  {
    soilsensor();
  }
   Firebase.getString(my_database,"pump");             // (objectname, tag name)
   String P = my_database.stringData();
   Serial.print("pump status");
     Serial.println(P);
  if(P=="1")
  {
     digitalWrite(motorpin,LOW);
     delay(1000);
     digitalWrite(motorpin,HIGH);
     Firebase.set(my_database, "pump" ,"0");
  }
  
  {

    smokesensor();
  }
  Firebase.getString(my_database,"buz");             // (objectname, tag name)
   String B = my_database.stringData();
   Serial.print("buz status");
     Serial.println(B);
   if(B=="1")
   {
      digitalWrite(buzpin,HIGH);
   }
   else
   {
      digitalWrite(buzpin,LOW);
   }
  {
    ldrsensor();
  }
     Firebase.getString(my_database,"led");             // (objectname, tag name)
     String L = my_database.stringData();
     Serial.print("led status");
     Serial.println(L);

  if(L=="1")
  {
     digitalWrite(ledpin,LOW);
  }
  else
  {
    digitalWrite(ledpin,HIGH);
  }
 
}
