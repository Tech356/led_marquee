#ifndef LedArray_h
#define LedArray_h

//#define _DRAW_METHOD_1_
#define _DRAW_METHOD_2_

#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "Arduino.h"

#if defined(_DRAW_METHOD_1_)

#define FONT_HEIGHT 7
#define FONT_DATA_SIZE 191

typedef struct {
  uint8_t offset;
  uint8_t len;
} PROGMEM char_entry;

#endif

#if defined(_DRAW_METHOD_2_)

#include "font5x7.h"

#endif

class LedArray
{
  public:
    LedArray(byte DataPin, byte ClockPin, byte LatchPin);
    void Begin();
    void ClearBuffer();
    void LampTest(long ms);
    void Show();
    void DrawString(char string[], byte textLength, int PosX);
  private:
    boolean buffer[72][7];
    byte dataPin;
    byte clockPin;
    byte latchPin;

    //First Method for displaying chars
    #if defined(_DRAW_METHOD_1_)
    void drawString_1(char string[], byte textLength, int PosX);
    uint8_t get_char_len(uint8_t c);
    bool get_char_bit(uint8_t c, uint8_t row, uint8_t column);
    static char_entry font_table[128];
    static prog_uint8_t font_data[FONT_DATA_SIZE];
    #endif

    //Second Method
    #if defined(_DRAW_METHOD_2_)
    void drawString_2(char string[], byte textLength, int PosX);
    #endif
};

#endif
