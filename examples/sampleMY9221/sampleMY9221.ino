/* arduino MY9221 sample program
 *  http://akizukidenshi.com/catalog/g/gI-09678/
 *  http://akizukidenshi.com/download/ds/mysemi/MY9221_DS1_0.pdf
 *  
 *  test boad: seeed XAIO
 *  
 *  回路図
 *  -----------
 *  | XAIO  5V|------------------------------------   
 *  |         |   --------------        | A        |　A
 *  | XAIO    |   | MY9221      |      LED1 ...  LED12
 *  |         |   |             |       | K        |  K
 *  |       D1| - |DI     OUA[0]|-------|          |
 *  |       D0| - |DCLK    .    |                  |
 *  |         |   |        .    |                  |
 *  |      3V3| - |VDD     .    |                  |
 *  |      GND| - |GND   OUTC[3]|-------------------
 *  |         |   |             |
 *  |         |   |      REXTA  |--- R1 (1.0 to 3.0 k)
 *  |         |   |      REXTB  |--- R2 (1.0 to 3.0 k)
 *  |         |   |      REXTC  |--- R3 (1.0 to 3.0 k)
 *  -----------   ---------------
 */
 
#include "suupen_MY9221.h"


#define ketaSuu (1) // 連結しているMY9221の数を設定する(設定可能範囲: n = 1 ～ 10) MY9221を10個まで連結できる
                    // マイコンへの接続と数字の関係:  arduino - MY9221(1) - MY9221(2) - ... - MY9221(n - 1), - MY9221(n))

suupen_MY9221 my9221(1,0,ketaSuu); // DI pinNo, DCLK pinNo, MY9221 connect No

float D_gray[ketaSuu * 12] = {0.0}; // MY922から出力するLEDの輝度データを設定する.
// 配列意味づけ: [0]: OUTA[0], [1]: OUTB[0], [2]: OUTC[0], ... [11]: OUTC[3]
// 範囲: 0.0: led OFF, .... , 1.0: led 100%ON




/** 表示バッファクリア
 *    
 */
void clearSeg(void)
{
    for(int segNo = 0; segNo < (ketaSuu * 12); segNo++) {
        D_gray[segNo] = 0;
    }
}


void setup() {
  
  my9221.begin();

  // MY9221 初期設定(設定内容はdatasheet "16-bit Command Data Description (CMD[15:0]) = D[207:192])"を参照
  my9221.commandSet(suupen_MY9221::fast, suupen_MY9221::bit8, suupen_MY9221::freq1, suupen_MY9221::waveApdm, suupen_MY9221::internal, suupen_MY9221::workLed, suupen_MY9221::freeMode, suupen_MY9221::repeatMode);

}

void loop() {
  //-----------------------------------
  // MY9221 OUTA[0] gray control　
  // MY9221 OUTA[0]に対して、徐々に明るくして、そのあと暗くする
  //-----------------------------------
  // light up
  for(uint16_t i = 0; i < 1000; i++){
    D_gray[0] += 0.001;            
        my9221.dataSet(&D_gray[0], 1);     // gray data set
        my9221.refresh();                  // send gray data to MY9221
  }

  // light down
  for(uint16_t i = 0; i < 1000; i++){
    D_gray[0] -= 0.001;            
        my9221.dataSet(&D_gray[0], 1);  // 表示データをlibrary内のバッファに設定
                                        // 第一引数: 表示バッファの先頭アドレス, 第二引数: 表示バッファのデータを設定するMY9221を指定( arduino - MY9221(n) - MY9221(n-1) - ... - MY9221(1)
        my9221.refresh();               // my9221.dataSet()で設定された表示データをMY9221に転送・表示する
  }

  //------------------------------------
  // light toss (light power=100%) 
  // 表示順序: OUTA[0] -> OUTB[0] -> OUTC[0] -> OUTA[1] -> ... -> OUTC[3]
  // OUTA[0]からOUTC[3]まで１つづつLEDを点灯させる
  //------------------------------------
  for(uint16_t i = 0; i < 12; i++){
    clearSeg();   // すべてのLED出力指示をOFFにする
    D_gray[i] = 1.0;  // 点灯対象のLEDのみ"1.0"(100%点灯)を設定する
    
    my9221.dataSet(&D_gray[0], 1);
    my9221.refresh();
    
    delay(100);        
  }
}
  
