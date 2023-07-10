/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Mon, 10 Jul 2023 06:29:57 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */
#include "SimpleDHT.h"
SimpleDHT11 dht11(4);
byte dht11_temperature = 0;
byte dht11_humidity = 0;

void dht11_read() {
  dht11_temperature = 0;
  dht11_humidity = 0;
  dht11.read(&dht11_temperature, &dht11_humidity, NULL);
}

void setup()
{
  Serial.begin(115200);


}


void loop()
{
  dht11_read();
  Serial.println((String("溫度:")+String(dht11_temperature)+String("\'C 濕度:")+String(dht11_humidity)+String("%")));
  delay(3000);
}