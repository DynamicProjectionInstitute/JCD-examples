#include <Arduino.h>
#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
#include "../lib/arduino-mqtt/src/MQTTClient.h" // to make linter happy ....
#include <string.h>
#include <EEPROM.h>

#define DEBUG true

Ultrasonic ultrasonic1(14,12);

const char ssid[] = "JCD-stock";
const char pass[] = "jcd-stock";
const char broker[]= "192.168.1.1";
static char st[8];
static bool ledStatus = false;

WiFiClient wificlient;
MQTTClient mqttclient;
void messageReceived(String &, String &);

void wificonnect();
void mqttconnect();
void lighttoggle();

void lighttoggle() {
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN,ledStatus);
  #ifdef DEBUG
  Serial.println(".");
  #endif
  delay(500);
}


void wificonnect() {
  #ifdef DEBUG
  Serial.println("wificonnect");
  #endif
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED) {
    lighttoggle();
  }
  #ifdef DEBUG
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  #endif
  digitalWrite(LED_BUILTIN,true);

}

void mqttconnect() {

  #ifdef DEBUG
  Serial.print("mqttconnect ");
  Serial.println(st);
  #endif
    mqttclient.begin(broker,wificlient);
  while(!mqttclient.connect(st)) {
    lighttoggle();

  }
  mqttclient.onMessage(messageReceived);
  #ifdef DEBUG
  Serial.println("mqtt connected");
  #endif
  digitalWrite(LED_BUILTIN,true);
  mqttclient.subscribe("dips/+/cmd");
}


void setup() {
  Serial.begin(9600);
EEPROM.begin(512);
  sprintf(st,"%x",ESP.getChipId());
  wificonnect();
  mqttconnect();
  Serial.print("EEPROM:");
  Serial.println(EEPROM.read(0));

}



void loop() {
  mqttclient.loop();
  delay(10);
if(WiFi.status() != WL_CONNECTED) {
#ifdef DEBUG
  Serial.println("wifi broke down....");
#endif
wificonnect();
}

  if (!mqttclient.connected()) {
    #ifdef DEBUG
      Serial.println("mqtt broke down....");
    #endif

    mqttconnect();
  }
std::string y;
std::string x="dips/";
x+=st;
y=x+"/eeprom";
mqttclient.publish(x.c_str(),String(ultrasonic1.distanceRead()));
mqttclient.publish(y.c_str(),String(EEPROM.read(0)));

  Serial.print("Sensor 01: ");
  Serial.print(ultrasonic1.distanceRead()); // Prints the distance on the default unit (centimeters)
  Serial.println("cm");
  delay(1000);
}


void messageReceived(String &topic, String &payload) {

    #ifdef DEBUG
    Serial.println("incoming: " + topic + " - " + payload);
    EEPROM.write(0, payload.toInt());
    EEPROM.commit();

#endif
}
