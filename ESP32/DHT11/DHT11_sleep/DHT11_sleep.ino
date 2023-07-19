//twgo.io/rltlb
#include <SimpleDHT.h>
#include <Wire.h>
#include "MatrixInt.h" //安裝方式：https://www.nmking.io/index.php/2022/12/15/701/
#include "driver/rtc_io.h" //RTC腳位上拉下拉設定
// ------ 以下修改成你MQTT設定 ------
long WNB303Timeout = 10000; //返回0=Timeout
int pinDHT11 = 4;
int LED = 18;
SimpleDHT11 dht11(pinDHT11);
//使用小霸王Matrix板
int WNB303ResetPIN = 14;       //  設定 WNB303 重置腳位為輸出腳位
int WNB303PowerPIN = 15;      //  設定 WNB303 電源控制腳位為輸出腳位

// ------ 以下修改成你MQTT設定 ------
String MQTTServer = "mqttgo.io";//免註冊MQTT伺服器
String MQTTPort = "1883";//MQTT Port
String MQTTUser = "";//不須帳密
String MQTTPassword = "";//不須帳密
String MQTTPubTopic = "cp/hH";//推播主題:推播溫度與濕度(記得改成你自己的Topic)
String MQTTSubTopic = "cp/led"; //訂閱主題:改變燈號(IO4)
#define TIME_TO_SLEEP  60 //秒
//使用RTC記憶體紀錄喚醒次數
RTC_DATA_ATTR int RTCBootCount = 0;


void setup() {
  Serial.begin(115200);       //  設定序列埠監控視窗的鮑率
  Serial2.begin(115200);      //  設定 WNB303 的鮑率
  Wire.begin(26, 27);         //  Matrix I2C
  for (int i = 0; i <= 7; ++i) MatrixInt(i, 0);//關閉所有燈號
  pinMode(WNB303ResetPIN, OUTPUT);        //  設定 WNB303 電源控制腳位為輸出腳位
  pinMode(WNB303PowerPIN, OUTPUT);        //  設定 WNB303 重置腳位為輸出腳位
  delay(1000);
  pinMode(LED, OUTPUT);
  Serial.println("\r\n\r\n======================\r\nSystem Starting....");
  //印出喚醒次數
  Serial.println("RTCBootCount : " + String(RTCBootCount++));
  //設定喚醒時間
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000 * 1000);
  rtc_gpio_hold_dis((gpio_num_t) WNB303PowerPIN);//解除睡眠保留設定

  String result = "";
  //  開啟 Modem 的電源，開啟關閉之間須間隔10秒(延時電路)
  WNB303Restart(10);
  //檢查網路註冊狀態，5分鐘內無法註冊，則重新啟動WNB303
  int tryCount = 0;
  while (1) {
    boolean resultb = WNB303CheckReg();
    Serial.print("網路註冊結果="); Serial.println(resultb);
    if (resultb == false) {
      Serial.println("時間內無法註冊網路(" + String(tryCount) + ")");
      delay(10000);
    }
    else break;
    if (tryCount++ >= 30) { //無法註冊已經超過5分鐘
      tryCount = 0;
      WNB303Restart(30);
    }
  }
  //檢查網路訊號品質
  result = WNB303CheckRSSI();
  Serial.print("訊號品質="); Serial.println(result);
  delay(3000);
  //連線mqtt
  result = mqttConnect(MQTTServer, MQTTPort, MQTTUser, MQTTPassword);
  if (result == "OK") Serial.println("MQTT Connected");
  //訂閱主題，並接收資訊
  result = mqttSubscribe(MQTTSubTopic, "0");
  if (result == "OK") Serial.println("訂閱成功:" + MQTTSubTopic + " Subscribed");
  //檢查訂閱訊息
  String SubData = WNB303MSG("+EMQPUB:", 5000); //等候訂閱資訊
  //處理訂閱
  if (!(SubData == "")) {
    String Topic = mqttGetSubTopic(SubData); //Serial.println("Topic=" + Topic);
    String Data = mqttGetSubData(SubData); //Serial.println("Data=" + Data);
    if (Topic == MQTTSubTopic) {
      rtc_gpio_hold_dis((gpio_num_t) LED);//解除睡眠保留設定
      digitalWrite(LED, Data.toInt());
    }
  }

  //檢查時間，傳輸步驟：讀取溫濕度、MQTT連線、推播
  MatrixInt(7, 1);

  //讀取溫濕度
  byte temperature = 0; byte humidity = 0;
  ReadDHT(&temperature, &humidity);
  String payload = "{\"T\":" + String(temperature) + ",\"H\":" + String(humidity) + "}";

  //推播訊息
  result = mqttPublish(MQTTPubTopic, "0", "0", "0", payload);
  if (result == "OK") Serial.println("Data:\"" + payload + "\" Published to " + MQTTPubTopic);
  //關閉mqtt
  result = mqttDisconnect();
  if (result == "OK") Serial.println("MQTT Disconnected");
  MatrixInt(7, 0);
  delay(1000);
  //關閉相關裝置
  AT2WNB303("POWEROFF", ""); //關閉NB303電源
  gpio_hold_en((gpio_num_t) WNB303PowerPIN);//開啟睡眠保留設定
  gpio_hold_en((gpio_num_t) LED);//開啟睡眠保留設定
  for (int i = 0; i <= 7; ++i) MatrixInt(i, 0);//關閉所有燈號
  //休眠
  Serial.println("進入休眠");
  esp_deep_sleep_start();
}

void loop() {
  //do nothing..
}

//取得主題
String mqttGetSubTopic(String SubData) {
  String Topic = split(SubData, ',', 1);
  Topic.trim();
  Topic.replace("\"", "");
  return Topic;
}

//取得內容
String mqttGetSubData(String SubData) {
  String Data = split(SubData, ',', 6);
  Data.trim();
  Data.replace("\"", "");
  Data = Hex2Str(Data);
  return Data;
}

//訂閱主題
String mqttSubscribe(String MQTTSubTopic, String Qos) {
  String result = AT2WNB303("AT+EMQSUB=0,\"" + MQTTSubTopic + "\"," + Qos, "OK");
  return result;
}

//連線mqtt
String mqttConnect(String Host, String Port, String MQTTUser, String MQTTPassword) {
  //1.取得IP
  String result = "" ;
  String IP = AT2WNB303("AT+EDNS=\"" + Host + "\"", "+EDNS:");
  IP.trim(); IP = split( IP, ':', 1);
  if (IP.length() <= 7) return "Error: Can't Get IP";
  //2.建立連線
  result = AT2WNB303("AT+EMQNEW=\"" + IP + "\",\"" + Port + "\"" + "," + 60000  + "," + 1024, "+EMQNEW:");
  String mqttid = split( result, ':', 1);
  //Serial.print("MQTTCreate result="); Serial.println(mqttid);
  //以亂數為ClientID
  String  MQTTClientid = "NBIoT-" + String(random(1000000, 9999999));
  result = AT2WNB303("AT+EMQCON=0,3,\"" + MQTTClientid + "\"" + "," + 60000  + ",0,0,\"" + MQTTUser + "\",\"" + MQTTPassword + "\"", "OK");
  //Serial.print("MQTTConnect result="); Serial.println(result);
  return result;
}

//推播訊息
String mqttPublish(String Topic, String QoS, String retained, String dup , String Payload) {
  String result = "" ;
  String hexPayload = Str2Hex(Payload);
  String lenHexPayload = String(hexPayload.length());
  result = AT2WNB303("AT+EMQPUB=0," + Topic + "," + QoS + "," + retained + "," + dup + "," + lenHexPayload + "," + hexPayload, "OK");
  return result;
}

//關閉mqtt連線
String mqttDisconnect() {
  String result = "" ;
  result = AT2WNB303("AT+EMQDISCON=0", "OK");
  return result;
}

//檢查網路註冊狀態 return true or false
boolean WNB303CheckReg() {
  String result = "";
  boolean CEREG = false;
  result = AT2WNB303("AT+CEREG?", "+CEREG:");
  if (result.indexOf("0,1") >= 0 || result.indexOf("1,1") >= 0) { //註冊成功
    MatrixInt(5, 0);//亮紅燈
    MatrixInt(6, 1);//亮綠燈
    CEREG = true;
  }
  else {
    MatrixInt(5, 1);//亮紅燈
    MatrixInt(6, 0);//亮綠燈
    CEREG = false;
  }
  return CEREG;
}

//檢查網路訊號品質 return RSSI，當RSSI=0或99代表沒訊號
int WNB303CheckRSSI() {
  String result = "";
  int RSSI = 0;
  result = AT2WNB303("AT+CESQ", "+CESQ:");
  //處理+CESQ:
  if (!(result == "-1")) {
    int CESQ = split(split(result, ':', 1), ',', 0).toInt();
    RSSI = CESQ - 111;
  }
  if (RSSI == -111 || RSSI == 210) RSSI = 0;
  MatrixLEDrssi(RSSI);
  return RSSI;
}

//重新啟動WNB303(單位為秒)
void WNB303Restart(int dalayTime) {
  AT2WNB303("POWEROFF", "");
  delay(5000);
  AT2WNB303("POWERON", "");
  delay(dalayTime * 1000) ;
}

//HTTP GET
String HTTPGET(String Protocol, String Host, String port, String Url) {
  //例如 http://x.x.x.x/update?api_key=xxxxx&field1=60
  //拆解成Protocol="http" host="x.x.x.x" port="80" Url="/update?api_key=CxxxxxxJ&field1=60"
  //1.轉換網址IP
  String result = "" ;
  String IP = AT2WNB303("AT+EDNS=\"" + Host + "\"", "+EDNS:");
  IP.trim(); IP = split( IP, ':', 1);
  if (IP.length() <= 7) return "Error: Can't Get IP";
  //2.建立連線
  String PIP = Protocol + "://" + IP + ":" + port + "/";
  int PIPlen = PIP.length();
  //Serial.println("PIP=" + PIP + ",len=" + String(PIPlen));
  result = AT2WNB303("AT+EHTTPCREATE=0," + String(PIPlen) + "," + String(PIPlen) + ",\"" + PIP + "\"", "+EHTTPCREAT");
  result.trim(); String clientid = split(result, ':', 1);
  if (result == "") return "Error: Can't Create Connection";
  else {
    //3.開啟連線
    result = AT2WNB303("AT+EHTTPCON=" + clientid, "OK");
    result.trim();
    if (!(result == "OK")) result = "Error: Can't Connect to Server";
    //4.組成網址並傳送
    int LenUrl = Url.length();
    Url = clientid + ",0," + String(LenUrl) + "," + Url + ",0,,0,,0,";
    Url = "0," + String(Url.length()) + "," + String(Url.length()) + "," + Url;
    result = AT2WNB303("AT+EHTTPSEND=" + Url, "OK");
    if (!(result == "OK")) result = "Error: Can't Send to Server";
  }
  delay(1000);
  //關閉連線
  AT2WNB303("AT+EHTTPDISCON=" + clientid, "OK");
  //Serial.println(result);
  delay(1000);
  AT2WNB303("AT+EHTTPDESTROY=" + clientid, "OK");
  //Serial.println(result);
  delay(1000);
  return result;
}

//HTTP POST
String HTTPPOST(String Protocol, String Host, String port, String Url , String contType , String Data) {
  //1.轉換網址IP
  String result = "" ;
  String IP = AT2WNB303("AT+EDNS=\"" + Host + "\"", "+EDNS:");
  IP.trim(); IP = split( IP, ':', 1);
  if (IP.length() <= 7) return "Error: Can't Get IP";
  //2.建立連線
  String PIP = Protocol + "://" + IP + ":" + port + "/";
  int PIPlen = PIP.length();
  //Serial.println("PIP=" + PIP + ",len=" + String(PIPlen));
  result = AT2WNB303("AT+EHTTPCREATE=0," + String(PIPlen) + "," + String(PIPlen) + ",\"" + PIP + "\"", "+EHTTPCREAT");
  result.trim(); String clientid = split(result, ':', 1);
  if (result == "") return "Error: Can't Create Connection";
  else {
    //3.開啟連線
    result = AT2WNB303("AT+EHTTPCON=" + clientid, "OK");
    result.trim();
    if (!(result == "OK")) result = "Error: Can't Connect to Server";
    //4.組成網址並傳送
    int i = 0;
    String PostCommand[10];
    PostCommand[i++] = clientid + ",1," + Url.length() + "," + Url + ",0,," + contType.length() + "," + contType + ",";
    //Serial.println("command1=" + PostCommand[i - 1]);
    String hexData = Str2Hex(Data);
    int lenHexData = hexData.length();
    PostCommand[i++] = String(lenHexData) + ",";
    //Serial.println("command2=" + PostCommand[i - 1]);
    int num = 30;
    for (int n = 0; n < lenHexData; n = n + num) {
      String DataSend = "";
      if (n + num < lenHexData) {
        //切割字串
        DataSend = hexData.substring(n, n + num);
      } else if (lenHexData % num > 0) {
        int remainder = lenHexData % num;
        //切割字串
        DataSend = hexData.substring(n, n + remainder);
      }
      PostCommand[i++] = DataSend;
      //Serial.println("command2=" + PostCommand[i - 1]);
    }
    //求出每條命令長度
    int totalLenHexData = 0;
    for (int j = 0; j <= i - 1; j++) {
      totalLenHexData = totalLenHexData + PostCommand[j].length();
      PostCommand[j] = String(PostCommand[j].length()) + "," + PostCommand[j];
    }
    //完成命令組合
    for (int j = 0; j <= i - 1; j++) {
      if (j == 1) PostCommand[j] = "AT+EHTTPSEND=1," + String(totalLenHexData) + "," + PostCommand[j];
      else if (j == i - 1) PostCommand[j] = "AT+EHTTPSEND=0," + String(totalLenHexData) + "," + PostCommand[j];
      else PostCommand[j] = "AT+EHTTPSEND=1," + String(totalLenHexData) + "," + PostCommand[j];
      result = AT2WNB303(PostCommand[j], "OK");
      //Serial.println("POST" + String(j) + + ":" + result);
    }

  }
  delay(1000);
  //關閉連線
  AT2WNB303("AT+EHTTPDISCON=" + clientid, "OK");
  //Serial.println(result);
  delay(1000);
  AT2WNB303("AT+EHTTPDESTROY=" + clientid, "OK");
  //Serial.println(result);
  delay(1000);
  return result;
}

//檢查WNB303訊息
String WNB303MSG(String StringWith, int delay) {
  long StartTime = millis();
  String result = "";
  while (1) {
    while (Serial2.available()) { //WNB303有資料回傳
      char c = Serial2.read();    //從WNB303讀取一個位元組
      result += c;                //將讀到的字元 c 加進字串 Xfer
      if (c == '\n') break;
    }
    result.trim();
    if (result.startsWith(StringWith)) break; //結尾OK返回
    if ((millis() - StartTime) >= delay ) { //Timeout返回
      break;
    }
  }
  return result;
  //if (result.indexOf(StringWith) >= 0) Serial.println("result=" + result);
}

//將訊息傳到WNB303，並讀取回傳訊息 0代表timeout
String AT2WNB303(String ATdata, String StartWith) {
  //Serial2.flush();
  Serial.println("你的命令是:" + ATdata);
  if (ATdata.length() > 0) { //送出AT命令
    ATdata.trim();
    String command = ATdata;
    command.toUpperCase();

    if (command == "RESET") {  //重置 WNB303
      digitalWrite(WNB303ResetPIN, HIGH);
      delay(10000);
      digitalWrite(WNB303ResetPIN, LOW);
      return "RESET OK";
    }
    else if (command == "POWERON") {  //開啟 WNB303 的電源
      digitalWrite(WNB303PowerPIN, HIGH);
      return "POWERON OK";
    }
    else if (command == "POWEROFF") {  //關閉 WNB303 的電源
      digitalWrite(WNB303PowerPIN, LOW);
      return "POWEROFF OK";
    }
    else  {  //送出AT命令
      Serial2.println(ATdata);
    }
  }
  else return "";
  String result = "";
  //等候回應資料
  long StartTime = millis();
  while (1) {
    result = "";
    while (Serial2.available()) { //WNB303有資料回傳
      char c = Serial2.read();    //從WNB303讀取一個位元組
      result += c;                //將讀到的字元 c 加進字串 Xfer
      if (c == '\n') break;
    }
    result.trim();
    if (result.startsWith(StartWith)) break; //結尾OK返回
    if ((millis() - StartTime) >= WNB303Timeout ) { //Timeout返回
      result = "0";
      break;
    }
  }
  //Serial.println("ATMsg=" + result);
  return result;
}

//讀取DHT11溫濕度
void ReadDHT(byte *temperature, byte *humidity) {
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Error="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  //Serial.print((int)*temperature); Serial.print(" *C, ");
  //Serial.print((int)*humidity); Serial.println(" H");
}

//字串轉HEX
String Str2Hex(String msg) {
  String a = "";
  for (int i = 0; i < msg.length(); i++)  {
    a = a + String(msg.charAt(i), HEX);
  }
  return a;
}

//HEX轉字串
String Hex2Str(String msg) {
  char input[msg.length() + 1];
  msg.toCharArray(input, msg.length() + 1);
  char c[sizeof(input)];
  String a = "";
  for (int i = 0; i < sizeof(input) - 1; i += 2) {
    char temp[3];
    temp[0] = input[i];
    temp[1] = input[i + 1];
    int val = ASCIIHexToInt(temp[0]) * 16 + ASCIIHexToInt(temp[1]);
    c[i] = toascii(val);
    a = a + String(c[i]);
  }
  return a;
}

//ASC轉INT
int ASCIIHexToInt(char c) {
  int ret = 0;
  if ((c >= '0') && (c <= '9')) ret = (ret << 4) + c - '0';
  else ret = (ret << 4) + toupper(c) - 'A' + 10;
  return ret;
}

//split拆解，範例：String a1=split(“aa,bb,cc”,’,’,0);
String split(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
