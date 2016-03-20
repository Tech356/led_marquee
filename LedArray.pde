#include <LedArray.h>

//Green:  2 Serial
//Yellow: 3 Clock
//Brown:  4 Storage/Latch
//Blue:   5 OutputEnable - active low

const byte DataPin = 2;
const byte ClockPin = 3;
const byte LatchPin = 4;
const byte OutputEnable = 5;

const byte ScrollSpeedPinLarge = 0;
const byte ScrollSpeedPinSmall = 1;

LedArray Display(DataPin, ClockPin, LatchPin);

long previousMillis;
long interval =  80;

const char proto[] = "MESSAGE:";

const byte MAX_STRING_LENGTH = 220;

#define __NUM_TEXT_BUFFERS__ 2
char TextBuffers[__NUM_TEXT_BUFFERS__][MAX_STRING_LENGTH] = {"The Cake is a Lie" , "The Cake is a Lie."};
byte BufferLenghts[__NUM_TEXT_BUFFERS__] = {17, 18};
// This buffer index is not actually active,
// it is more just a var to store a ref to a buffer
// the name is probably very misleading
byte ActiveBufferIndex = 0;

byte IncomingIndex = 0;

int firstStrPos = 72;
int newStrPos = 73;
// the distance between the 2 strings
int offset = 20;

void setup() {
  pinMode(OutputEnable, OUTPUT);
  digitalWrite(OutputEnable, LOW);
  pinMode(13, OUTPUT);

  Display.Begin();
  Serial.begin(38400);
}

void loop() {
  //millis() will wrap after ~50 days
  long currentMillis = millis();
  if (currentMillis - previousMillis > interval || currentMillis < previousMillis) {
    previousMillis = currentMillis;
    digitalWrite(OutputEnable, HIGH);
    Update();
    digitalWrite(OutputEnable, LOW);
  }

  Display.Show();

  if (ActiveBufferIndex == 1)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);

 //set scroll speed based on Pot
 // map the interval range from 0ms to 200ms
 interval = map(analogRead(ScrollSpeedPinLarge), 0, 1023, 0, 200);
 interval += map(analogRead(ScrollSpeedPinSmall), 0, 1023, 0, 40);
}

void Update() {
  //Serial.println("UPDATE: --Start-- ");
  Display.ClearBuffer();

  if (Serial.available() > 0)
    GetSerialData();

  firstStrPos--;
  newStrPos--;

  int CurrentStringPosition = firstStrPos;
  //Serial.print("UPDATE: CurrentStringPosition: ");
  //Serial.println(CurrentStringPosition, DEC);
  while (CurrentStringPosition <= 72) {
    //Serial.println("UPDATE: WHILE: --Start-- ");
    //Serial.print("UPDATE: WHILE: CurrentStringPosition: ");
    //Serial.println(CurrentStringPosition, DEC);

    byte bufferIndex = ActiveBufferIndex;
    if (CurrentStringPosition >= newStrPos && BufferLenghts[(ActiveBufferIndex + 1) % __NUM_TEXT_BUFFERS__] != 0)
      bufferIndex = (ActiveBufferIndex + 1) % __NUM_TEXT_BUFFERS__;

    //Serial.print("UPDATE: WHILE: Active Buffer Index ");
    //Serial.println(bufferIndex, DEC);

    int endOfStr = CurrentStringPosition + (BufferLenghts[bufferIndex] * 5 + BufferLenghts[bufferIndex] - 1); //CurrentStringPosition + length of str in pixels
    //Serial.print("UPDATE: WHILE: endOfStr: ");
    //Serial.println(endOfStr, DEC);
    if (endOfStr < 0) {
      // if this string is not on screen, move positions and comtinue
      //Serial.println("UPDATE: WHILE: end of Str < 0");
      CurrentStringPosition = endOfStr + offset;
      firstStrPos = CurrentStringPosition;
    } else {
      //Serial.println("UPDATE: WHILE: Drawing String ");
      Display.DrawString(TextBuffers[bufferIndex], BufferLenghts[bufferIndex], CurrentStringPosition);
      CurrentStringPosition = endOfStr + offset;
    }
    //Serial.println("UPDATE: WHILE: ---End--- ");
  }
  //Serial.println("UPDATE: ---End--- ");
}

void GetSerialData() {
  // if there is not an available buffer, do nothing
  if (newStrPos >= -(BufferLenghts[ActiveBufferIndex] * 5 + BufferLenghts[ActiveBufferIndex] - 1)) {
    Serial.flush();
    return;
  }

  byte availableBufferIndex = ActiveBufferIndex;
  while(Serial.available() > 0) {
    TextBuffers[availableBufferIndex][IncomingIndex] = Serial.read();
    TextBuffers[availableBufferIndex][IncomingIndex + 1] = '\0';
    if (TextBuffers[availableBufferIndex][IncomingIndex] == 13 || IncomingIndex == MAX_STRING_LENGTH - 1 || TextBuffers[availableBufferIndex][IncomingIndex] == 10) {
      //Serial.println("NULL found");

      bool isMessage = true;
      for (int k = 0; k < 8; k++) {
        if (TextBuffers[availableBufferIndex][k] != proto[k])
          isMessage = false;
      }

      if (isMessage) {
        //Serial.println("Message found");
        //Serial.println(IncomingIndex, DEC);
        BufferLenghts[availableBufferIndex] = IncomingIndex - 8; //minus 8 for the "MESSAGE:"
        for(int i = 0; i < IncomingIndex + 1 && i < MAX_STRING_LENGTH; i++)
          TextBuffers[availableBufferIndex][i] = TextBuffers[availableBufferIndex][i + 8];

        newStrPos = 73;
        ActiveBufferIndex = (ActiveBufferIndex + 1) % __NUM_TEXT_BUFFERS__;
      }

      IncomingIndex = 0;
      break;
    }
    IncomingIndex++;
  }
  //Serial.println(TextBuffers[availableBufferIndex]);
}
