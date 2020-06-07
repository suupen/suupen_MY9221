#include "suupen_MY9221.h"

// 元のmbed libraryからの移植でつじつまを合わせるためのマクロ
// arduinoでは割り込み禁止許可は不要だったので指示しない
// 割り込み禁止命令
#define DISABLE_IRQ()                 // arduino
#define DISABLE_IRQ() __disable_irq() // mbed

// 割り込み許可命令
#define ENABLE_IRQ()                  // arduino
#define ENABLE_IRQ() __enable_irq()   // mbed

/**
 *
 */
suupen_MY9221::suupen_MY9221(unsigned char di, unsigned char dcki, uint8_t number)
{
  __di = di;
  __dcki = dcki;
  __number = number;

  pinMode(__di, OUTPUT);
  pinMode(__dcki, OUTPUT);

    __command = 0x0000;
    commandRawSet(__command);
}

/** send one word data
 * @param uint16_t data : send data
 * @param return none
 */
void suupen_MY9221::sendWord(uint16_t data)
{
    uint16_t temp = data;
    uint8_t count = 8;
    uint16_t bitData;     // __diに書き込むbit値  範囲: HIGH/LOW

    DISABLE_IRQ(); // 禁止
    digitalWrite(__dcki, LOW);
    delayMicroseconds(1);

    do {

        bitData = ((temp & 0x8000) == 0x8000) ? HIGH : LOW;
        digitalWrite(__di, bitData);  
        temp <<= 1;
    delayMicroseconds(1);

    digitalWrite(__dcki, HIGH);
//    delayMicroseconds(1);

        bitData = ((temp & 0x8000) == 0x8000) ? HIGH : LOW;
        digitalWrite(__di, bitData);  
        temp <<= 1;
    delayMicroseconds(1);

    digitalWrite(__dcki, LOW);
//    delayMicroseconds(1);

    } while(--count > 0);

    digitalWrite(__di, LOW);
    digitalWrite(__dcki, LOW);
    ENABLE_IRQ(); // 許可

}






/** data latch & display
 * @param none
 * @param return none
 */
void suupen_MY9221::latch(void)
{
    DISABLE_IRQ(); // 禁止
  delayMicroseconds(300);

  digitalWrite(__dcki, LOW);
  delayMicroseconds(1);

  digitalWrite(__di, HIGH);
  delayMicroseconds(1);
  digitalWrite(__di, LOW);
  delayMicroseconds(1);

  digitalWrite(__di, HIGH);
  delayMicroseconds(1);
  digitalWrite(__di, LOW);
  delayMicroseconds(1);

  digitalWrite(__di, HIGH);
  delayMicroseconds(1);
  digitalWrite(__di, LOW);
  delayMicroseconds(1);

  digitalWrite(__di, HIGH);
  delayMicroseconds(1);
  digitalWrite(__di, LOW);
  delayMicroseconds(1);


  digitalWrite(__dcki, LOW);
  
    ENABLE_IRQ(); // 許可

}

/** send brock data
 * @param commandData_t *data : suupen_MY9221 Data
 * @param uint8_t number : connection number (min=1)
 * @param none
 */
void suupen_MY9221::sendBrock(commandData_t *data)
{
    for(uint8_t i = 0; i < __number; i++) {

        sendWord((data + i)->CMD);

        sendWord((data + i)->D[OUT3].DA);
        sendWord((data + i)->D[OUT3].DB);
        sendWord((data + i)->D[OUT3].DC);

        sendWord((data + i)->D[OUT2].DA);
        sendWord((data + i)->D[OUT2].DB);
        sendWord((data + i)->D[OUT2].DC);

        sendWord((data + i)->D[OUT1].DA);
        sendWord((data + i)->D[OUT1].DB);
        sendWord((data + i)->D[OUT1].DC);

        sendWord((data + i)->D[OUT0].DA);
        sendWord((data + i)->D[OUT0].DB);
        sendWord((data + i)->D[OUT0].DC);
    }

    latch();
}


void suupen_MY9221::commandRawSet(uint16_t command)
{
    for(int i = 0; i < __number; i++) {
        __MY9221Data[i].CMD = command;
    }
}


void suupen_MY9221::commandSet(hspd_t hspd, bs_t bs, gck_t gck, sep_t sep, osc_t osc, pol_t pol, cntset_t cntset, onest_t onest)
{

    __command = 
        ((hspd   << 11)  & 0x0400) |
        ((bs     << 8)   & 0x0300) | 
        ((gck    << 5)   & 0x0e00) | 
        ((sep    << 4)   & 0x0010) | 
        ((osc    << 3)   & 0x0008) | 
        ((pol    << 2)   & 0x0004) | 
        ((cntset << 1)   & 0x0002) | 
        ((onest)         & 0x0001); 
                    
    for(int i = 0; i < __number; i++) {
        __MY9221Data[i].CMD = __command;
    }
}

void suupen_MY9221::dataRawSet(uint16_t *data, uint8_t number)
{

    uint8_t num = 0;

    if((number > 0) && (number < 11)) {
        num = number - 1;
    }

    __MY9221Data[num].D[OUT0].DA = *(data + 0);
    __MY9221Data[num].D[OUT0].DB = *(data + 1);
    __MY9221Data[num].D[OUT0].DC = *(data + 2);

    __MY9221Data[num].D[OUT1].DA = *(data + 3);
    __MY9221Data[num].D[OUT1].DB = *(data + 4);
    __MY9221Data[num].D[OUT1].DC = *(data + 5);

    __MY9221Data[num].D[OUT2].DA = *(data + 6);
    __MY9221Data[num].D[OUT2].DB = *(data + 7);
    __MY9221Data[num].D[OUT2].DC = *(data + 8);

    __MY9221Data[num].D[OUT3].DA = *(data + 9);
    __MY9221Data[num].D[OUT3].DB = *(data + 10);
    __MY9221Data[num].D[OUT3].DC = *(data + 11);

//   printf("%04x  %04x %04x %04x        ",__MY9221Data[num].CMD, 0,  *(data + 0), __MY9221Data[num].D[OUT0].DA);

}

void suupen_MY9221::dataSet(float *data, uint8_t number)
{
    uint16_t bit = (__command >> 8 ) & 0x0003;
    uint16_t bias; 

    uint8_t num = 0;

    if((number > 0) && (number < 11)) {
        num = number - 1;
    }

    switch(bit){
        case suupen_MY9221::bit8:
        bias = 0x00ff;
        break;
        case suupen_MY9221::bit12:
        bias = 0x0fff;
        break;
        case suupen_MY9221::bit14:
        bias = 0x3fff;
        break;
        case suupen_MY9221::bit16:
        bias = 0xffff;
        break;
        default:
        bias = 0x00ff;
        break;
        }

    __MY9221Data[num].D[OUT0].DA = (uint16_t)(*(data + 0) * bias);
    __MY9221Data[num].D[OUT0].DB = (uint16_t)(*(data + 1) * bias);
    __MY9221Data[num].D[OUT0].DC = (uint16_t)(*(data + 2) * bias);

    __MY9221Data[num].D[OUT1].DA = (uint16_t)(*(data + 3) * bias);
    __MY9221Data[num].D[OUT1].DB = (uint16_t)(*(data + 4) * bias);
    __MY9221Data[num].D[OUT1].DC = (uint16_t)(*(data + 5) * bias);

    __MY9221Data[num].D[OUT2].DA = (uint16_t)(*(data + 6) * bias);
    __MY9221Data[num].D[OUT2].DB = (uint16_t)(*(data + 7) * bias);
    __MY9221Data[num].D[OUT2].DC = (uint16_t)(*(data + 8) * bias);

    __MY9221Data[num].D[OUT3].DA = (uint16_t)(*(data + 9) * bias);
    __MY9221Data[num].D[OUT3].DB = (uint16_t)(*(data + 10) * bias);
    __MY9221Data[num].D[OUT3].DC = (uint16_t)(*(data + 11) * bias);

//   printf("%04x   %04x %f %04x\r\n",__MY9221Data[num].CMD, bias,  *(data + 0), __MY9221Data[num].D[OUT0].DA);

}

void suupen_MY9221::refresh(void){

    sendBrock(__MY9221Data);
}
