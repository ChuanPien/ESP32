/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Mon, 10 Jul 2023 06:02:27 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */


void setup()
{
  Serial.begin(115200);


  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
}


void loop()
{
  Serial.println((String("光敏感測數值:")+String(analogRead(36))));
  if (analogRead(36) >= 2000) {
    digitalWrite(15, HIGH);
    digitalWrite(16, HIGH);
    digitalWrite(17, HIGH);
  } else {
    if (analogRead(36) >= 1000) {
      digitalWrite(15, HIGH);
      digitalWrite(16, HIGH);
      digitalWrite(17, LOW);
    } else {
      if (analogRead(36) >= 500) {
        digitalWrite(15, HIGH);
        digitalWrite(16, LOW);
        digitalWrite(17, LOW);
      } else {
        digitalWrite(15, LOW);
        digitalWrite(16, LOW);
        digitalWrite(17, LOW);
      }
    }
  }
  delay(1000);
}