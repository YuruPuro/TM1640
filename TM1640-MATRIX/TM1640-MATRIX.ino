/**
 * LED DRIVER TM1640 TEST
 * 8x8 LEDマトリクス１個を操作するサンプル
 */
#include <Arduino.h>
#define PIN_DIN 9
#define PIN_SCLK 10

// LED Display Pattern　(^^)
static uint8_t dispPattern[] = { 0x3C,0x42,0x81,0xA5,0x81,0x99,0x42,0x3C} ;

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
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_SCLK, LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_DIN, (data & 0x01) ? HIGH : LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_SCLK, HIGH);
    delayMicroseconds(1);
    data >>= 1;
  }
  delayMicroseconds(1);
  digitalWrite(PIN_SCLK, LOW);
  digitalWrite(PIN_DIN, LOW);
  delayMicroseconds(1);
}

// nByteのデータ送信
void sendDataN(int dataNum,uint8_t data[]){
  for (int n=0;n<dataNum;n++) {
    uint8_t data1 = data[n];
    for (int i = 0; i < 8; i++) {
      digitalWrite(PIN_SCLK, LOW);
      delayMicroseconds(1);
      digitalWrite(PIN_DIN, (data1 & 0x01) ? HIGH : LOW);
      delayMicroseconds(1);
      digitalWrite(PIN_SCLK, HIGH);
      delayMicroseconds(1);
      data1 >>= 1;
    }
  }
  delayMicroseconds(1);
  digitalWrite(PIN_SCLK, LOW);
  digitalWrite(PIN_DIN, LOW);
  delayMicroseconds(1);
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

// ７セグメント1個(LEDマトリクスでは1行)指定表示
// grid=0~15
void disp1Grid(uint8_t grid , uint8_t data) {
  sendComand(0x44) ;  // fixed address 
  sendStart() ;
  sendData((grid & 0x0F) | 0xC0) ;
  sendData(data) ;
  sendStop();
}

// ７セグメントn個(LEDマトリクスではn行)指定表示
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
  // --- Display 1
  uint8_t data = 0x01 ;
  for (int i=0;i<25;i++) {
    for (int j=0;j<8;j++) {
      disp1Grid(j,data) ;
      data <<= 1 ;
      if (data == 0) {
        data = 0x01 ;
      }
    }
    delay(300) ;
    data <<= 1 ;
    if (data == 0) {
      data = 0x01 ;
    }
  }
  // --- Display2
  for (int j=0;j<3;j++) {
    for (int i=0;i<8;i++) {
        disp1Grid(i,0x00) ;
    }
    delay(300) ;
    for (int i=0;i<8;i++) {
        disp1Grid(i,dispPattern[i]) ;
    }
    delay(1000) ;
  }
}
