#include <Arduino.h>
#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
#include "../lib/arduino-mqtt/src/MQTTClient.h" // to make linter happy ....
#include <string.h>
#include <EEPROM.h>

//#define DEBUG true //switched in  platformio.ini

Ultrasonic ultrasonic1(14,12);

// const char ssid[16] = "JCD-stock";
// const char pass[16] = "jcd-stock";
const char ssid[16] = "JCD-79fa5412";
const char pass[16] = "5353da83";

const char broker[16]= "192.168.1.1";
static char st[8];
static bool ledStatus = false;
static bool state=false;

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

void lightflash() {
  digitalWrite(LED_BUILTIN,true);
  for(int i=0;i<10;i++) {
    ledStatus = !ledStatus;
    digitalWrite(LED_BUILTIN,ledStatus);
    delay(25);
  }
  digitalWrite(LED_BUILTIN,true);

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
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,false);

  sprintf(st,"%x",ESP.getChipId());
  wificonnect();
  mqttconnect();
  Serial.print("EEPROM:");
  Serial.println(EEPROM.read(0));

}


float measure() {
  int x=0;
  for(int i=0;i<=5;i++) {
    x+=ultrasonic1.distanceRead();
    delay(10);
  }
  return x/5.0;
}

void loop() {

  int measurement;
  int eepromdata;
  lightflash();
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
std::string z;
  std::string x="dips/";
  x+=st;
  y=x+"/eeprom";
  z=x+"/event";
  measurement=measure();
  eepromdata=EEPROM.read(0);

  mqttclient.publish(x.c_str(),String(measurement));
  mqttclient.publish(y.c_str(),String(eepromdata));

  if(measurement<eepromdata) {
    mqttclient.publish(z.c_str(),"true");
    if(state==false) {
    mqttclient.publish("jcd/global/jcd/cmd","SHON");
    state=true;
    }
  } else {
    mqttclient.publish(z.c_str(),"false");
    if(state==true) {
    mqttclient.publish("jcd/global/jcd/cmd","SHOFF");
    state=false;
}
}



  //Serial.print("Sensor 01: ");
  //Serial.print(ultrasonic1.distanceRead()); // Prints the distance on the default unit (centimeters)
  //Serial.println("cm");
  delay(1000);
}


void messageReceived(String &topic, String &payload) {

  #ifdef DEBUG
  Serial.println("incoming: " + topic + " - " + payload);
  EEPROM.write(0, payload.toInt());
  EEPROM.commit();

  #endif
}
