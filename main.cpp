#include <Arduino.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <Ticker.h>
#include <Wire.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include "firebase.h"

#define LED_GREEN 14
#define LED_YELLOW 12
#define LED_RED 13
#define PIN_DHT 19

/*
Google Firebase control and monitoring
* Control led: 
  - cmd/LedGreen
  - cmd/LedRed
  - cmd/LedYellow
* Receive sensor data
  - data/humidity
  - data/temperature
*/

const char* ssid = "R_X3P";
const char* password = "@RudySusanto";
Ticker timerPublish, ledOff;
DHTesp dht;

void WifiConnect();
void onSendSensor();

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  dht.setup(PIN_DHT, DHTesp::DHT22);
  
  WifiConnect();
  Firebase_Init("cmd");
  Serial.println("System ready.");
}

void loop() {
  onSendSensor();
  delay(5000);
}

void onSendSensor()
{
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  if (dht.getStatus()==DHTesp::ERROR_NONE)
  {
    Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n", 
      temperature, humidity);
    Firebase.RTDB.setFloat(&fbdo, "/data/temperature", temperature);
    Firebase.RTDB.setFloat(&fbdo, "/data/humidity", humidity);
  }
  else
  {
    Serial.printf("DHT22 error: %d\n", dht.getStatus());
  };
}

void onFirebaseStream(FirebaseStream data)
{
  Serial.printf("onFirebaseStream: %s %s %s %s\n", data.streamPath().c_str(),
    data.dataPath().c_str(), data.dataType().c_str(), data.stringData().c_str());

  if (data.dataType()=="int")
  {
    String strDev = data.dataPath().substring(1);    
    byte nValue = data.stringData().charAt(0)-'0';
    Serial.printf("Device: %s -> %d\n", strDev.c_str(), nValue);

    if (nValue>=0 && nValue<=1)
    {
      if (strDev=="LedRed")
        digitalWrite(LED_RED, nValue);
      if (strDev=="LedGreen")
        digitalWrite(LED_GREEN, nValue);
      if (strDev=="LedYellow")
        digitalWrite(LED_YELLOW, nValue);
    }
  }
}

void WifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }  
  Serial.print("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}
