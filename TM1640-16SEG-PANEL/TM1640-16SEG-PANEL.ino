/**
 * LED DRIVER TM1640 TEST
 *　秋月電子の【改造】超大型4桁LED表示キット 表示用サンプル
 */
#include <Arduino.h>
#define PIN_DIN 9
#define PIN_SCLK 10
#define PIN_DOT 8
#define PIN_DP1 7
#define PIN_DP2 6
#define PIN_DP3 5
#define PIN_DP4 4

// LED Display Pattern
static uint16_t dispPattern[] = { 
  0B0011000000000000 , 0B1110111011000000 , // 1 , 2
  0B1111110011000000 , 0B0011000111000000 , // 3 , 4 
  0B1100110110000100 , 0B1101111111000000 , // 5 , 6
  0B1111000000000000 , 0B1111111111000000 , // 7 , 8
  0B1111000111000000 , 0B1111111100001001 , // 9 , 0
  0B1111001111000000 , 0B1111110001010010 , // A , B
  0B1100111100000000 , 0B1111110000010010 , // C , D
  0B1100111110000000 , 0B1100001110000000 , // E , F
  0B1101111101000000 , 0B0011001111000000 , // G , H
  0B1100110000010010 , 0B0011111000000000 , // I , J
  0B0000001110001100 , 0B0000111100000000 , // K , L
  0B0011001100101000 , 0B0011001100100100 , // M , N
  0B1111111100000000 , 0B1110001111000000 , // O , P
  0B1111111100000100 , 0B1110001111000100 , // Q , R
  0B1101110111000000 , 0B1100000000010010 , // S , T
  0B0011111100000000 , 0B0000001100001001 , // U , V
  0B0011001100000101 , 0B0000000000101101 , // W , X
  0B0011110111000000 , 0B1100110000001001 , // Y , Z
  } ;  

static uint16_t dispPattern2[][2] = { 
  0B1000000000000000 , 0B0000010000000000 ,
  0B0100000000000000 , 0B0000100000000000 ,
  0B0000010000000000 , 0B1000000000000000 ,
  0B0000100000000000 , 0B0100000000000000 ,
  0B0000001000000000 , 0B0010000000000000 ,
  0B0000000100000000 , 0B0001000000000000 ,
  } ;  

static uint16_t dispPattern3[] = { 
  0B0000000010000000 ,
  0B0000000000100000 ,
  0B0000000000010000 ,
  0B0000000000001000 ,
  0B0000000001000000 ,
  0B0000000000000100 ,
  0B0000000000000010 ,
  0B0000000000000001 ,
 } ;  

  
// 配線しやすいように繋いだので、GRID(アドレス)とピンの対応変換表を使用する
static uint16_t dispMap[] = {
   5 , //  A1 -17番ピン - GRID1
   2 , //  A2 -14番ピン - GRID2
   1 , //  B  -13番ピン - GRID3
   15, //  C  - 9番ピン - GRID4
   8 , //  D1 - 4番ピン - GRID5
   13, //  D2 - 7番ピン - GRID6
   9 , //  E  - 3番ピン - GRID7
   7 , //  F  -19番ピン - GRID8
   10, //  G1 - 2番ピン - GRID9
   0 , //  G2 -12番ピン - GRID10
   6 , //  J  -18番ピン - GRID11
   4 , //  K  -16番ピン - GRID12
   3 , //  L  -15番ピン - GRID13
   14, //  M  - 8番ピン - GRID14
   12, //  N  - 6番ピン - GRID15
   11, //  P  - 5番ピン - GRID16
   0   //  DP - 10番ピン
  } ;

uint8_t dispBuff[16] ;

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

// ７セグメント1個(16セグメントLEDでは1SEG)指定表示
// grid=0~15
void disp1Grid(uint8_t grid , uint8_t data) {
  sendComand(0x44) ;  // fixed address 
  sendStart() ;
  sendData((grid & 0x0F) | 0xC0) ;
  sendData(data) ;
  sendStop();
}

// ７セグメントn個(16セグメントLEDではnSEG)指定表示
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

  pinMode(PIN_DOT,OUTPUT) ;
  digitalWrite(PIN_DOT,LOW) ;
  pinMode(PIN_DP1,OUTPUT) ;
  pinMode(PIN_DP2,OUTPUT) ;
  pinMode(PIN_DP3,OUTPUT) ;
  pinMode(PIN_DP4,OUTPUT) ;
  digitalWrite(PIN_DP1,LOW) ;
  digitalWrite(PIN_DP2,LOW) ;
  digitalWrite(PIN_DP3,LOW) ;
  digitalWrite(PIN_DP4,LOW) ;

  // Brightness
  sendComand(0x44) ;  // fixed address 
  memset(dispBuff,0x00,sizeof(dispBuff)) ;
  disp1GridN(0 , 16 , dispBuff) ;
  setBrightness(7) ;
  delay(1000) ;
}

void loop() {
  int dot = LOW ;
  // --- Display 0
  for (int i=0;i<3;i++) {
    memset(dispBuff,0xFF,sizeof(dispBuff)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    digitalWrite(PIN_DOT,dot) ; dot = (dot == LOW) ? HIGH : LOW ;
    digitalWrite(PIN_DP1,LOW) ;
    digitalWrite(PIN_DP2,LOW) ;
    digitalWrite(PIN_DP3,LOW) ;
    digitalWrite(PIN_DP4,LOW) ;
    delay(200) ;
    memset(dispBuff,0x00,sizeof(dispBuff)) ;
    digitalWrite(PIN_DOT,dot) ; dot = (dot == LOW) ? HIGH : LOW ;
    digitalWrite(PIN_DP1,HIGH) ;
    digitalWrite(PIN_DP2,HIGH) ;
    digitalWrite(PIN_DP3,HIGH) ;
    digitalWrite(PIN_DP4,HIGH) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(200) ;
  }
  
  // --- Display 1
  for (int i=0;i<18;i++) {
    digitalWrite(PIN_DOT,dot) ; dot = (dot == LOW) ? HIGH : LOW ;
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern2[i%6][0] & mask) != 0 ? 0x05 : 0x00 ;
      uint8_t data2 = (dispPattern2[i%6][1] & mask) != 0 ? 0x0A : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    digitalWrite(PIN_DP3,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP4,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;

    delay(200) ;
  }

  // --- Display 2
  for (int i=0;i<24;i++) {
    digitalWrite(PIN_DOT,dot) ; dot = (dot == LOW) ? HIGH : LOW ;
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern3[i%8] & mask) != 0 ? 0x05 : 0x00 ;
      uint8_t data2 = (dispPattern3[i%8] & mask) != 0 ? 0x0A : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    digitalWrite(PIN_DP3,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP4,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(200) ;
  }

  // --- Display 3
  for (int i=0;i<36;i++) {
    digitalWrite(PIN_DOT,dot) ; dot = (dot == LOW) ? HIGH : LOW ;
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern[i%36] & mask) != 0 ? 0x01 : 0x00 ;
      uint8_t data2 = (dispPattern[(i+1)%36] & mask) != 0 ? 0x02 : 0x00 ;
      uint8_t data3 = (dispPattern[(i+2)%36] & mask) != 0 ? 0x04 : 0x00 ;
      uint8_t data4 = (dispPattern[(i+3)%36] & mask) != 0 ? 0x08 : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 |data3 | data4 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    digitalWrite(PIN_DP3,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP4,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(500) ;
  } 
}
