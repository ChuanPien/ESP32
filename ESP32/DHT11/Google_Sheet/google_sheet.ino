/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Tue, 11 Jul 2023 07:57:38 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */
#include <WiFi.h>
#include "SimpleDHT.h"
SimpleDHT11 dht11(4);
byte dht11_temperature = 0;
byte dht11_humidity = 0;
#include <WiFiClientSecure.h>

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

const char* asId="AKfycbyR-Yp-uu4nIvnjvnkILaQ5AX8yFxp-UpBO-Sqs0su3ai1N_BvQsz_Q";
String sheetId="";
String sheetTag="";

String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";
  while (*msg!='\0'){
      if( ('a' <= *msg && *msg <= 'z')
              || ('A' <= *msg && *msg <= 'Z')
              || ('0' <= *msg && *msg <= '9') ) {
          encodedMsg += *msg;
      } else {
          encodedMsg += '%';
          encodedMsg += hex[*msg >> 4];
          encodedMsg += hex[*msg & 15];
      }
      msg++;
  }
  return encodedMsg;
}

void  sendToGoogleSheets(const String& dateInclude,const String& data)
{
  static WiFiClientSecure sheetClient;
  const char* host="script.google.com";
  if (sheetClient.connect(host, 443)) {
      const String url = String() +"https://"+host+"/macros/s/"+asId+"/exec?type=insert&dateInclude="+dateInclude+"&sheetId="+sheetId+"&sheetTag="+sheetTag+"&data="+data;
      sheetClient.println("GET " + url + " HTTP/1.1");
      sheetClient.println(String()+"Host: "+host);
      sheetClient.println("Accept: */*");
      sheetClient.println("Connection: close");
      sheetClient.println();
      sheetClient.println();
      sheetClient.stop();
  }
}

void setup()
{
  Serial.begin(115200);

  initWiFi();
  Serial.println("WiFi Connect");
  sheetId="15avDmHyvMPi8Hla5rOqE7b9-XamerNcwYGeLyNPeFjw";
  sheetTag=URLEncode("Data");
}


void loop()
{
  dht11_read();
  Serial.println((String("T : ")+String(dht11_temperature)+String("\'C | H : ")+String(dht11_humidity)+String("%")));
  sendToGoogleSheets("1",URLEncode((String() + (dht11_temperature) + "," + (dht11_humidity) + "," + analogRead(36)).c_str()));
  delay(2000);
}