#include "Arduino.h"
#include "LedArray.h"

// Constructor
LedArray::LedArray(byte DataPin, byte ClockPin, byte LatchPin) {
  dataPin = DataPin;
  clockPin = ClockPin;
  latchPin = LatchPin;
}

void LedArray::Begin() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  
  ClearBuffer();
  Show();    
}

// sets the buffer to 0
void LedArray::ClearBuffer() {
  for(int y = 0; y < 7; y++)
    for(int x = 0; x < 72; x++)
      buffer[x][y] = false;
}

// Draws the buffer to the screen
void LedArray::Show() {
  for(int y = 0; y < 7; y++)
  {
    digitalWrite(latchPin, 0);  
    for(int x = 0; x < 72; x += 8)
    {
      byte column;
      
      for (int offset = 0; offset < 8; offset++)        
        bitWrite(column, offset, !buffer[x + offset][y]);
      
      shiftOut(dataPin, clockPin, LSBFIRST, column);
    }

    //start with 11111111 which turns off all rows
    byte row = 255;
    //write a Zero to the row we want to update
    bitWrite(row, 7 - y, 0);    
    shiftOut(dataPin, clockPin, LSBFIRST, row);
    
    digitalWrite(latchPin, 1);
  }
}

// Draws Sprite to buffer
// void LedArray::Draw(Sprite sprite) {
  // for (int x = sprite.PosX; x < sprite.PosX + sprite.Texture.Width; x++)
    // for (int y = sprite.PosY; y < sprite.PosY + sprite.Texture.Height; y++)
    // {
      // if (x > 71 || y > 6 || x < 0 || y < 0)
        // continue;
        
      // buffer[x][y] = sprite.Texture.read(x - sprite.PosX, y - sprite.PosY);
    // }
// }

// Draws String to buffer
void LedArray::DrawString(char text[], byte textLength, int PosX) {
  #if defined(_DRAW_METHOD_1_)
  drawString_1(text, textLength, PosX);
  #endif
  
  #if defined(_DRAW_METHOD_2_)
  drawString_2(text, textLength, PosX);
  #endif
}

// ==================================================================================
// First Method for Drawing
// ==================================================================================
#if defined(_DRAW_METHOD_1_)

void LedArray::drawString_1(char text[], byte textLength, int PosX) {
  int pos = PosX;

  for (int i = 0; i < textLength; i++)
  {
    uint8_t len = get_char_len(text[i]);
    
    for (int x = 0; x < len; x++)
    {
      if (pos <= 71 || pos >= 0)
      {
        for (byte y = 0; y < 7; y++)
          buffer[pos][y] = get_char_bit(text[i], y, x);
      }
      pos++;
    }
    //add a space between chars
    pos++;
  }
}

char_entry LedArray::font_table[128] = {
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0, 1},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {1, 3},
  {4, 3},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {7, 1},
  {8, 3},
  {11, 5},
  {16, 2},
  {18, 4},
  {22, 4},
  {26, 4},
  {30, 4},
  {34, 4},
  {38, 4},
  {42, 4},
  {46, 4},
  {50, 1},
  {51, 2},
  {0,0},
  {0,0},
  {0,0},
  {53, 4},
  {57, 6},
  {63, 5},
  {68, 4},
  {72, 5},
  {77, 4},
  {81, 4},
  {85, 4},
  {89, 5},
  {94, 5},
  {99, 3},
  {102, 4},
  {106, 5},
  {111, 4},
  {115, 7},
  {122, 6},
  {128, 6},
  {134, 5},
  {139, 5},
  {144, 4},
  {148, 4},
  {152, 5},
  {157, 5},
  {162, 5},
  {167, 7},
  {174, 7},
  {181, 5},
  {186, 5},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},

};
 
prog_uint8_t LedArray::font_data[FONT_DATA_SIZE] = {
0x5f,
0x1c,
0x22,
0x41,
0x41,
0x22,
0x1c,
0x40,
0x60,
0x1c,
0x3,
0x3e,
0x41,
0x41,
0x41,
0x3e,
0x2,
0x7f,
0x62,
0x51,
0x49,
0x46,
0x22,
0x49,
0x49,
0x36,
0xf,
0x8,
0x8,
0x7f,
0x2f,
0x49,
0x49,
0x39,
0x3e,
0x49,
0x49,
0x32,
0x1,
0x61,
0x19,
0x7,
0x36,
0x49,
0x49,
0x36,
0x26,
0x49,
0x49,
0x3e,
0x12,
0x20,
0x12,
0x2,
0x59,
0x9,
0x6,
0x3e,
0x41,
0x5d,
0x55,
0x4e,
0x20,
0x7e,
0x9,
0x9,
0x9,
0x7e,
0x7f,
0x49,
0x49,
0x36,
0x3e,
0x41,
0x41,
0x41,
0x22,
0x7f,
0x41,
0x41,
0x3e,
0x7f,
0x49,
0x49,
0x41,
0x7f,
0x9,
0x9,
0x1,
0x3e,
0x41,
0x41,
0x49,
0x3a,
0x7f,
0x8,
0x8,
0x8,
0x7f,
0x41,
0x7f,
0x41,
0x20,
0x40,
0x40,
0x3f,
0x7f,
0x8,
0x14,
0x23,
0x40,
0x7f,
0x40,
0x40,
0x40,
0x7f,
0x2,
0x4,
0x8,
0x4,
0x2,
0x7f,
0x7f,
0x2,
0x4,
0x8,
0x10,
0x7f,
0x3e,
0x41,
0x41,
0x41,
0x41,
0x3e,
0x7f,
0x9,
0x9,
0x9,
0x6,
0x3e,
0x41,
0x51,
0x21,
0x5e,
0x7f,
0x9,
0x19,
0x66,
0x26,
0x49,
0x49,
0x32,
0x1,
0x1,
0x7f,
0x1,
0x1,
0x3f,
0x40,
0x40,
0x40,
0x3f,
0x1f,
0x20,
0x40,
0x20,
0x1f,
0x7f,
0x20,
0x10,
0x8,
0x10,
0x20,
0x7f,
0x41,
0x22,
0x14,
0x8,
0x14,
0x22,
0x41,
0x7,
0x8,
0x78,
0x8,
0x7,
0x61,
0x51,
0x49,
0x45,
0x43,

};
 
uint8_t LedArray::get_char_len(uint8_t c) {
  return font_table[c].len;
  //return pgm_read_byte(&(font_table[c].len));
}
 
bool LedArray::get_char_bit(uint8_t c, uint8_t row, uint8_t column) {
  uint8_t offset = font_table[c].offset;
  uint8_t col = font_data[offset+column];
  return ((col & _BV(row)) == 0)?false:true;
  // uint8_t offset = pgm_read_byte(&(font_table[c].offset));
  // uint8_t col = pgm_read_byte(&(font_data[offset+column]));
  // return ((col & _BV(row)) == 0)?false:true;
}

#endif

// ==================================================================================
// Second Method for Drawing
// ==================================================================================
#if defined(_DRAW_METHOD_2_)

void LedArray::drawString_2(char text[], byte textLength, int PosX) {
  if (PosX > 71)
    return;
        
  int pos = PosX;

  for (int i = 0; i < textLength; i++)
  {      
    //all characters have a length of 5
    uint8_t len = 5;
    
    for (uint8_t x = 0; x < len; x++)
    {    
      if (pos > 71)
        return;
        
      if (pos <= 71 && pos >= 0)
      {
        // 0x20 is the start of printable characters
        uint8_t c = (pgm_read_byte(&Font5x7[((text[i] - 0x20) * len) + x]));
        for (byte y = 0; y < 7; y++)
          buffer[pos][y] = bitRead(c, y);
      }
      pos++;
    }
    //don't make spaces extra wide
    //but put a space between chars
    if (text[i] == ' ')
      pos--;
    else
      pos++;    
  }
}

#endif
