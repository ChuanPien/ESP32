/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Mon, 10 Jul 2023 06:09:26 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */


void setup()
{

}


void loop()
{
  for (int i = 255; i >= 0; i -= 5) {
    ledcAttachPin(4, 1);
    ledcSetup(1, 5000, 8);
    ledcWrite(1,i);
    delay(50);
  }
  for (int i = 0; i <= 255; i += 5) {
    ledcAttachPin(4, 1);
    ledcSetup(1, 5000, 8);
    ledcWrite(1,i);
    delay(50);
  }
}