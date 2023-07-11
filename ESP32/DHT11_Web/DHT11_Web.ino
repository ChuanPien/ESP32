/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Tue, 11 Jul 2023 03:09:51 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */
#include <DHT.h>
#include <WiFi.h>

DHT dht11_p0(0, DHT11);
char _lwifi_ssid[] = "";
char _lwifi_pass[] = "";
WiFiServer WebServer(80);

String webPara="";
String tempPara="";

void setup()
{
  dht11_p0.begin();

  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_lwifi_ssid, _lwifi_pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  delay(300);
  WebServer.begin();
}


void loop()
{
  checkWebClient();
  Serial.println(String((String("IP:")+String(WiFi.localIP().toString()))));
  Serial.println(String((String("溫度:")+String(dht11_p0.readTemperature())+String("\'C 濕度:")+String(dht11_p0.readHumidity())+String("%"))));
  delay(3000);
}

void checkWebClient(){
  WiFiClient WebClient = WebServer.available();
  if (WebClient) {
    webPara="";
    boolean currentLineIsBlank = true;
    while (WebClient.connected()) {
      if (WebClient.available()) {
        char c = WebClient.read();
        if (c == '\n' && currentLineIsBlank) {
          WebClient.println("HTTP/1.1 200 OK");
          WebClient.println("Content-Type: text/html");
          WebClient.println("Connection: close");
          WebClient.println("Refresh: 5");
          WebClient.println();
          WebClient.println("<!DOCTYPE HTML>");
          WebClient.println("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>ESP32網路伺服器</title></head><body bgcolor='#66cccc'>");
          WebClient.println("<p align=left><font size='5' color='#ff0000'>");
          WebClient.println("<b>"+String((String("溫度:")+String(dht11_p0.readTemperature())+String("\'C 濕度:")+String(dht11_p0.readHumidity())+String("%")))+"</b>");
          WebClient.println("</font></p>");
          WebClient.println("<p align=left><font size='5' color='#ffcc33'>");
          WebClient.println("<b>"+String((String("IP:")+String(WiFi.localIP().toString())))+"</b>");
          if (webPara.indexOf(String("GET /digital/")+5+String("/HIGH")) >= 0){
            pinMode(5,OUTPUT);
            digitalWrite(5,HIGH);
          }
          if (webPara.indexOf(String("GET /digital/")+5+String("/LOW")) >= 0){
            pinMode(5,OUTPUT);
            digitalWrite(5,LOW);
          }
          WebClient.println(String("<input type='button' value=開 onclick=\"location.href='/digital/")+5+String("/HIGH';return true;\"")+(digitalRead(5)==1?" disabled":"")+">"+"&nbsp;&nbsp;"+String("<input type='button' value=關 onclick=\"location.href='/digital/")+5+String("/LOW';return true;\"")+(digitalRead(5)==0?" disabled":"")+">");
          WebClient.println("</font></p>");
          WebClient.println("</body></html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          webPara=webPara+c;
         currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    WebClient.stop();
  }
}