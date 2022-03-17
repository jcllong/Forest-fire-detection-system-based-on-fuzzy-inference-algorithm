
#include "DHT.h"                           //引用dht11库文件，使得下面可以调用相关参数
#include <Wire.h>                                              
#include "LiquidCrystal_I2C.h"
//#include <SoftwareSerial.h>
#include "Timer.h" 
#define DHTPIN 2
#define DHTTYPE DHT11
#define Sensor_DO 4
#define MQ7_DO 5
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//SoftwareSerial mySerial(1, 0); // RX, TX
float h = 0;
float t = 0;
float hm[6];
float tp[6];
 int s = 0;
 int q = 0;
char val;
int Buzzer = 13; // buzzer
int FlamePin = 3; // This is for input pin
boolean Flame; // HIGH when FLAME Exposed
boolean s1,s2;
byte read_dat,jdy_dat;
Timer tt;  

/******************************************/
void setup() {                                                 //设置
	pinMode(DHTPIN,INPUT);
        dht.begin();
        lcd.init();                      // initialize the lcd
	lcd.backlight();
	lcd.print("Forest Guardian");// Print a message to the LCD.
	lcd.setCursor(0,1);
	lcd.print("-----System-----");
        delay(2000);
        pinMode(Buzzer, OUTPUT);
        pinMode(FlamePin, INPUT);
        pinMode(Sensor_DO, INPUT);
        pinMode(MQ7_DO, INPUT);
	Serial.begin(9600);//设置波特率参数 
        Serial.println("Please stand by for Bluetooth");
        h = dht.readHumidity();
	t = dht.readTemperature();
        Serial.println("Bluetooth Ready");
        //while (!Serial) {
  //}
        //mySerial.begin(9600);
}
/******************************************/
void humiture() {   //温湿度函数 
 
  	h = dht.readHumidity();
	t = dht.readTemperature();                //将湿度值赋给hum
	//Serial.print("Tempeature:");                        //打印出Tempeature:
	//Serial.print(t); //打印温度结果
        //Serial.println('C');
	      //将电压值由数字量换成模拟量                                     
	lcd.print("Tep:"); 
        lcd.print(t);
        lcd.print("C");
        lcd.setCursor(0,1); 
        //Flame = digitalRead(FlamePin);
	//Serial.print("Humidity:");                            //打印出Humidity:
	//Serial.print(h);                                     //打印出湿度结果
	//Serial.println("%");                                //打印出
        //h=h*5.0/1023;      //将电压值由数字量换成模拟量     
        lcd.print("Hum:"); 
        lcd.print(h);
        lcd.print("%");
	delay(2000);                                 //延时一段时间（实际设置为1小时一次） 
                                          
}
/******************************************/
void MQ(){
   s=analogRead(A0);
   q=analogRead(A1);
  //float vol1=5.00*(x/1023.00);
  //float vol2=5.00*(y/1023.00);
  lcd.clear();         //清空lcd显示
  lcd.print("MQ2:");
  lcd.print(s);
  //Serial.println(s);
  lcd.print(" ");
  lcd.print("MQ7:");
  lcd.print(q);
  //Serial.println(q);
  lcd.setCursor(0,1);
  //if(x<210){
    //s=0;
  //}
  //else if(x>210){
    //s=1;
  //}
  // if(y<660){
    //q=0;
  //}
  //else if(x>660){
    //q=1;
  //}
  Flame = digitalRead(FlamePin);
  //Serial.println("Flame");
  //Serial.println(Flame);
}
/******************************************/

void Bluetooth(){
  if (Serial.read()>0) {
      val = Serial.read(); 
      //Serial.print(val);
        delay(2);
//        发送1表示查看温度数据
        if(val=='1'){
          //switch(val)
          //case '0':
          Serial.write("Tempeature:");                        //打印出Tempeature:
	  Serial.print(t);                           //打印温度结果
          Serial.write("C");//break;
        }
        //        发送2表示查看湿度
        else if(val=='2'){
           //case '1':
          Serial.write("Humidity:");                            //打印出Humidity:
	  Serial.print(h);                                     //打印出湿度结果
	  Serial.write("%");//break;                                //打印出      
        }
        //        发送其他表示查看错误
        else{
         // default:
          Serial.write("Send error");                            //打印出Humidity:
        }
  }
     
    delay(1000);//     每5秒更新一次
}
/******************************************/
void ht_update(){
  dht.readHumidity();
  dht.readTemperature();
}
/******************************************/
void warning(){
  float x[5]={0.1,0.1,0.2,0.15,0.45};
  float y[5][2];
  float c[2]={0,0};
  int i,j;
  for(i=0;i<6;i++){
    hm[i]=dht.readHumidity();
    //Serial.println(hm[i]);
    tp[i]=dht.readTemperature();
    //Serial.println(tp[i]);
    //delay(100);
    tt.every(100,ht_update);
  }
  float diff_H,diff_T;
  diff_H=abs(hm[0]-hm[1])+abs(hm[2]-hm[3])+abs(hm[4]-hm[5]);
  diff_T=abs(tp[0]-tp[1])+abs(tp[2]-tp[3])+abs(tp[4]-tp[5]);
  //Serial.println(diff_H);
  //Serial.println(diff_H);
  float V_H=60;
  float V_T=40;
  if(diff_H>=V_H){
    y[0][0]=1;
    y[0][1]=0;
  }
  else if(diff_H<V_H){
    y[0][0]=(diff_H*1.00)/(V_H);
    y[0][1]=1-y[0][0];
  }
  if(diff_T>=V_T){
    y[1][0]=1;
    y[1][1]=0;
  }
  else if(diff_T<V_T){
    y[1][0]=(diff_H*1.00)/(V_H);
    y[1][1]=1-y[1][0];
  } 
  if(dht.readHumidity()>50){
    y[0][0]=1;
    y[0][1]=0;
  }
 if(dht.readTemperature()<10){
    y[1][0]=1;
    y[1][1]=0;
 }
  if(s>=250){
    y[2][0]=1;
    y[2][1]=0;
  }
  else if(s<250){
    y[2][0]=(abs(s-250)*1.00/250);
     //Serial.println(abs(s-320));
    y[2][1]=1-y[2][0];
  }
  if(q>=120){
    y[3][0]=1;
    y[3][1]=0;
  }
  else if(q<120){
    y[3][0]=abs(q-120)*1.00/120;
     //Serial.println(abs(q-265));
    y[3][1]=1-y[3][0];
  }
  if(Flame==0){
    y[4][0]=1;
    y[4][1]=0;
  }
  else{
    y[4][0]=0;
    y[4][1]=1;
  }
  //for(i=0;i<2;i++){
    //for(j=0;j<5;j++){
      //Serial.println(y[j][i]);
    //}}
  
  
  for(j=0;j<5;j++){
    c[0]=c[0]+x[j]*y[j][0];
    c[1]=c[1]+x[j]*y[j][1];
  }
  //Serial.println("% fire risk");
  //Serial.println(c[0]);
  //Serial.println(c[1]);
  s1= digitalRead(Sensor_DO);
  s2= digitalRead(MQ7_DO);
  //Serial.println(s1);
  int m=0;
    if(s1==0){
      m=m+1;
    }
    if(Flame==0){
      m=m+1;
    }
    if(t>=80){
      m=m+1;
    }
    if(h<=10){
      m=m+1;
    }
     if(q==0){
      m=m+1;
    }
   if(c[0]>=0.65||m>=4){
      Serial.println("Fire,please dial 119 as soon as possible");
      Serial.println(c[0]*100);
      Serial.print("% fire risk");
      lcd.print("---dangerous---");
      digitalWrite(Buzzer, HIGH);
    }
    else{
      Serial.println("There is no fire");
      Serial.print(c[0]*100);
      Serial.println("% fire risk");
      lcd.print("------safe------");
      digitalWrite(Buzzer, LOW);
    }
    delay(2000);
}
  
/******************************************/

void loop(){                             //主函数
   lcd.clear();
   digitalWrite(Buzzer, LOW);  
   humiture();   //采集温湿度数据
   //delay(1000);         //延时1s
   Bluetooth();
   lcd.clear();         //清空lcd显示
   lcd.print("Forest Guardian");// Print a message to the LCD.
   lcd.setCursor(0,1);
   lcd.print("-----System-----");
   delay(2000);
   MQ();
   warning();
   Bluetooth();
   lcd.clear();         //清空lcd显示
   tt.update();
}

    
    

