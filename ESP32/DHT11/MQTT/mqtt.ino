/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Tue, 11 Jul 2023 06:45:58 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */
#include <WiFi.h>
#include "SimpleDHT.h"
SimpleDHT11 dht11(4);
byte dht11_temperature = 0;
byte dht11_humidity = 0;
#include <PubSubClient.h>
#define MQTT_SERVER_IP "mqttgo.io"
#define MQTT_SERVER_PORT 1883
#define MQTT_ID ""
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

char _lwifi_ssid[] = "UH";
char _lwifi_pass[] = "0910505205";
void initWiFi() {
  WiFi.mode(WIFI_AP_STA);

  for (int i=0;i<2;i++) {
    WiFi.begin(_lwifi_ssid, _lwifi_pass);

    delay(1000);
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(_lwifi_ssid);

    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if ((StartTime+5000) < millis()) break;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("STAIP address: ");
      Serial.println(WiFi.localIP());
      Serial.println("");

      break;
    }
  }
}

void dht11_read() {
  dht11_temperature = 0;
  dht11_humidity = 0;
  dht11.read(&dht11_temperature, &dht11_humidity, NULL);
}

String receivedTopic="";
String receivedMsg="";
bool waitForE=true;
bool ended=true;
bool pubCtrl=false;

WiFiClient mqttClient;
PubSubClient myClient(mqttClient);

void connectMQTT(){
  while (!myClient.connected()){
    if (!myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD))
    {
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  receivedTopic=String(topic);
  receivedMsg="";
  for (unsigned int myIndex = 0; myIndex < length; myIndex++)
  {
      receivedMsg += (char)payload[myIndex];
  }
  receivedMsg.trim();
  if (receivedTopic == "cp/m") {
    Serial.println(receivedMsg);
  }

}

void setup()
{
  Serial.begin(115200);

  initWiFi();
  myClient.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  myClient.setCallback(mqttCallback);

  connectMQTT();
  myClient.subscribe(String("cp/m").c_str());
}


void loop()
{
  myClient.loop();
  dht11_read();
  Serial.println((String("T : ")+String(dht11_temperature)+String("\'C | H : ")+String(dht11_humidity)+String("%")));
  myClient.publish(String("cp/t").c_str(),String((dht11_temperature)).c_str());
  myClient.publish(String("cp/h").c_str(),String((dht11_humidity)).c_str());
  delay(2000);
}