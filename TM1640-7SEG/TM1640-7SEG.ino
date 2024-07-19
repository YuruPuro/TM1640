/**
 * LED DRIVER TM1640 TEST
 * 7セグメントLED　２個を操作するサンプル
 */
#include <Arduino.h>
#define PIN_DIN 9
#define PIN_SCLK 10

// 7SEG LED Display Pattern
// ※dp-g-f-e-d-c-b-a の順になっています。
static uint8_t dispPattern[] = {B00000110, B11011011, B01001111, B11100110, B01101101 ,
                                B11111101, B00000111, B11111111, B01101111, B10111111} ;

// ----- build TM1640 Command -----
// コマンド送信開始
void sendStart(){
  digitalWrite(PIN_DIN, LOW);
  digitalWrite(PIN_SCLK, LOW);
  delayMicroseconds(1);
}

// コマンド送信終了
void sendStop(){
  digitalWrite(PIN_SCLK, HIGH);
  digitalWrite(PIN_DIN, HIGH);
  delayMicroseconds(1);
}

// 1Byteのデータ送信
void sendData(uint8_t data){
  digitalWrite(PIN_SCLK, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_DIN, (data & 0x01) ? HIGH : LOW);
    digitalWrite(PIN_SCLK, HIGH);
    digitalWrite(PIN_SCLK, LOW);
    data >>= 1;
  }
}

// nByteのデータ送信
void sendDataN(int dataNum,uint8_t data[]){
  digitalWrite(PIN_SCLK, LOW);
  for (int n=0;n<dataNum;n++) {
    uint8_t data1 = data[n];
    for (int i = 0; i < 8; i++) {
      digitalWrite(PIN_DIN, (data1 & 0x01) ? HIGH : LOW);
      digitalWrite(PIN_SCLK, HIGH);
      digitalWrite(PIN_SCLK, LOW);
      data1 >>= 1;
    }
  }
}

// コマンド送信
void sendComand(uint8_t data){
  sendStart() ;
  sendData(data);
  sendStop();
}

// 明るさ設定: brightness=0~7
void setBrightness(uint8_t brightness) {
  brightness = (brightness & 0x07) | 0x88 ;
  sendComand(0x44) ;  // fixed address 
  sendComand(brightness) ;  // Brightness
}

// ７セグメント1個指定表示
// grid=0~15
void disp1Grid(uint8_t grid , uint8_t data) {
  sendComand(0x44) ;  // fixed address 
  sendStart() ;
  sendData((grid & 0x0F) | 0xC0) ;
  sendData(data) ;
  sendStop();
}

// ７セグメントn個指定表示
// grid=0~15
void disp1GridN(uint8_t grid , int dataNum , uint8_t data[]) {
  sendComand(0x40) ;  // Auto address 
  sendStart() ;
  sendData((grid & 0x0F) | 0xC0) ;
  sendDataN(dataNum , data) ;
  sendStop();
}

// ----------
void setup() {
  // PIN MODE Setting
  pinMode(PIN_DIN,OUTPUT) ;
  pinMode(PIN_SCLK,OUTPUT) ;
  digitalWrite(PIN_DIN,HIGH) ;
  digitalWrite(PIN_SCLK,HIGH) ;

  // Brightness
  sendComand(0x40) ;  // Auto address :最初にアドレスモードを設定しないと上手く動かないっぽい。

  for (int i=0;i<8;i++) {
    disp1Grid(i,0xFF) ;
  }
  setBrightness(2) ;
}

void loop() {
  uint8_t data = 0x01 ;
  for (int i=0;i<8;i++) {
    for (int j=0;j<8;j++) {
      disp1Grid(j,data) ;
      data <<= 1 ;
      if (data == 0) {
        data = 0x01 ;
      }
    }
    delay(1000) ;
    data <<= 1 ;
    if (data == 0) {
      data = 0x01 ;
    }
  }
}
