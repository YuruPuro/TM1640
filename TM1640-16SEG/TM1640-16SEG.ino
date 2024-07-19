/**
 * LED DRIVER TM1640 TEST
 * 16セグメントLED２個を操作するサンプル
 */
#include <Arduino.h>
#define PIN_DIN 9
#define PIN_SCLK 10
#define PIN_DP1 7
#define PIN_DP2 6

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
   13, //  A1 -17番ピン - GRID14(ADDR13)
   10, //  A2 -14番ピン - GRID11(ADDR10)
   9 , //  B  -13番ピン - GRID10(ADDR9)
   7 , //  C  - 9番ピン - GRID8 (ADDR7)
   2 , //  D1 - 4番ピン - GRID3 (ADDR2)
   5 , //  D2 - 7番ピン - GRID6 (ADDR5)
   1 , //  E  - 3番ピン - GRID2 (ADDR1)
   15, //  F  -19番ピン - GRID16(ADDR15)
   0 , //  G1 - 2番ピン - GRID1 (ADDR0)
   8 , //  G2 -12番ピン - GRID9 (ADDR8)
   14, //  J  -18番ピン - GRID15(ADDR14)
   12, //  K  -16番ピン - GRID13(ADDR12)
   11, //  L  -15番ピン - GRID12(ADDR11)
   6 , //  M  - 8番ピン - GRID7 (ADDR6)
   4 , //  N  - 6番ピン - GRID5 (ADDR4)
   3 , //  P  - 5番ピン - GRID4 (ADDR3)
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
  delayMicroseconds(1);
  for (int n=0;n<dataNum;n++) {
    uint8_t data1 = data[n];
    for (int i = 0; i < 8; i++) {
      digitalWrite(PIN_DIN, (data1 & 0x01) ? HIGH : LOW);
      digitalWrite(PIN_SCLK, HIGH);
      digitalWrite(PIN_SCLK, LOW);
      data1 >>= 1;
    }
  }
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

  pinMode(PIN_DP1,OUTPUT) ;
  pinMode(PIN_DP2,OUTPUT) ;
  digitalWrite(PIN_DP1,LOW) ;
  digitalWrite(PIN_DP2,LOW) ;


  // Brightness
  sendComand(0x44) ;  // fixed address 
  memset(dispBuff,0x00,sizeof(dispBuff)) ;
  disp1GridN(0 , 16 , dispBuff) ;
  setBrightness(7) ;
  delay(1000) ;
}

void loop() {
  // --- Display 1
  for (int i=0;i<3;i++) {
    memset(dispBuff,0xFF,sizeof(dispBuff)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    digitalWrite(PIN_DP1,LOW) ;
    digitalWrite(PIN_DP2,LOW) ;
    delay(200) ;
    memset(dispBuff,0x00,sizeof(dispBuff)) ;
    digitalWrite(PIN_DP1,HIGH) ;
    digitalWrite(PIN_DP2,HIGH) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(200) ;
  }
  
  // --- Display 1
  for (int i=0;i<18;i++) {
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern2[i%6][0] & mask) != 0 ? 0x01 : 0x00 ;
      uint8_t data2 = (dispPattern2[i%6][1] & mask) != 0 ? 0x02 : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;

    delay(200) ;
  }

  // --- Display 2
  for (int i=0;i<24;i++) {
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern3[i%8] & mask) != 0 ? 0x01 : 0x00 ;
      uint8_t data2 = (dispPattern3[i%8] & mask) != 0 ? 0x02 : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(200) ;
  }

  // --- Display 3
  for (int i=0;i<36;i++) {
    memset(dispBuff,0,sizeof(dispBuff)) ;
    uint16_t mask = 0x8000 ;
    for (int j=0;j<16;j++) {
      uint8_t data1 = (dispPattern[i%36] & mask) != 0 ? 0x01 : 0x00 ;
      uint8_t data2 = (dispPattern[(i+1)%36] & mask) != 0 ? 0x02 : 0x00 ;
      dispBuff[dispMap[j]] = data1 | data2 ;
      mask >>= 1 ;    
    }
    digitalWrite(PIN_DP1,(i%2?LOW:HIGH)) ;
    digitalWrite(PIN_DP2,(i%2?HIGH:LOW)) ;
    disp1GridN(0 , 16 , dispBuff) ;
    delay(1000) ;
  } 
}
