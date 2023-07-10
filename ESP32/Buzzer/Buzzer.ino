/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Mon, 10 Jul 2023 06:08:01 GMT
 */
/*  部份程式由吉哥積木產生  */
/*  https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697  */


void playBuzzer(int pin, String frequency, String delaytime) {
  int freq = 2000;
  int channel = 10;
  int resolution = 8;
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(pin, channel);
  String f="",d="",split=",";
  int s1=0;
  frequency+=",";
  delaytime+=",";
  for (int i=0;i<frequency.length();i++) {
    if (frequency[i]==split[0]) {
  	   f=frequency.substring(s1,i);
  	   s1=i+1;
  	   for (int j=0;j<delaytime.length();j++) {
  	      if (delaytime[j]==split[0]) {
  		    d=delaytime.substring(0,j);
  		    ledcWriteTone(channel, f.toInt());
  		    delay(d.toInt());
  		    delaytime=delaytime.substring(j+1);
  		    break;
  	      }
  	    }
    }
  }
  ledcWriteTone(channel, 0);
}

void setup()
{

}


void loop()
{
  playBuzzer(25, "392", "500");
  playBuzzer(25, "330", "500");
  playBuzzer(25, "330", "500");
  delay(500);
  playBuzzer(25, "349", "500");
  playBuzzer(25, "294", "500");
  playBuzzer(25, "294", "500");
  delay(500);
  playBuzzer(25, "262", "500");
  playBuzzer(25, "294", "500");
  playBuzzer(25, "330", "500");
  playBuzzer(25, "349", "500");
  playBuzzer(25, "392", "500");
  playBuzzer(25, "392", "500");
  playBuzzer(25, "392", "500");
  delay(2000);
}