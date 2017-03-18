#include <RTClib.h>

#include "LedArray.h"
#include <Wire.h>

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

const byte sw1Pin = 8;
const byte sw2Pin = 9;
const byte swPowerPin = 10;
const byte sw3Pin = 11;


LedArray Display(DataPin, ClockPin, LatchPin);

RTC_DS3231 rtc;

bool DisplayTime = true;

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

  pinMode(sw1Pin, INPUT);
  pinMode(sw2Pin, INPUT);
  pinMode(sw3Pin, INPUT);
  pinMode(swPowerPin, OUTPUT);
  digitalWrite(swPowerPin, HIGH);

  Display.Begin();
  Serial.begin(38400);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("Setup Complete");
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
  if (DisplayTime) {
    interval = 500;
  } else {
    interval = map(analogRead(ScrollSpeedPinLarge), 0, 1023, 0, 200);
    interval += map(analogRead(ScrollSpeedPinSmall), 0, 1023, 0, 40);
  }

  if (digitalRead(sw1Pin)  == HIGH)
    DisplayTime = true;
  else
    DisplayTime = false;
}

char* NumberToChar(byte num) {
  switch (num)
  {
    case 0:
      return "0";
    case 1:
      return "1";
    case 2:
      return "2";
    case 3:
      return "3";
    case 4:
      return "4";
    case 5:
      return "5";
    case 6:
      return "6";
    case 7:
      return "7";
    case 8:
      return "8";
    case 9:
      return "9";
  }
}

DateTime first_sunday_on_or_after(DateTime dt) {
    // Shift day indexes Sunday 0 -> 6, Monday 1 -> 0, ... Saturday 6 -> 5
    uint8_t day_index = (dt.dayOfTheWeek() + 6) % 7;
    uint8_t days_to_go = 6 - day_index;
    if (days_to_go > 0)
      dt = dt + TimeSpan(days_to_go, 0, 0, 0);
    return dt;
}

void Update() {
  Display.ClearBuffer();

  if (DisplayTime) {
    DateTime now = rtc.now();

    // Check Daylight Savings Time
    DateTime dst_start = first_sunday_on_or_after(DateTime(now.year(), 3, 8, 2));
    DateTime dst_end = first_sunday_on_or_after(DateTime(now.year(), 11, 1, 1));  // minus 1 hour for the DST difference

    if (now.secondstime() >= dst_start.secondstime() && now.secondstime() < dst_end.secondstime())
          now = now + TimeSpan(0, 1, 0, 0);

    byte currentPos = 4;
    char timeString[8];

    byte hours12 = now.hour() % 12;
    if (hours12 == 0)
      hours12 = 12;

    if (hours12 < 10) {
      Display.DrawString("0", 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(hours12), 1, currentPos);
      currentPos += 6;
    }
    else {
      Display.DrawString(NumberToChar(hours12 / 10), 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(hours12 % 10), 1, currentPos);
      currentPos += 6;
    }

    Display.DrawString(":", 1, currentPos);
    currentPos += 6;

    if (now.minute() < 10) {
      Display.DrawString("0", 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(now.minute()), 1, currentPos);
      currentPos += 6;
    }
    else {
      Display.DrawString(NumberToChar(now.minute() / 10), 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(now.minute() % 10), 1, currentPos);
      currentPos += 6;
    }

    Display.DrawString(":", 1, currentPos);
    currentPos += 6;

    if (now.second() < 10) {
      Display.DrawString("0", 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(now.second()), 1, currentPos);
      currentPos += 6;
    }
    else {
      Display.DrawString(NumberToChar(now.second() / 10), 1, currentPos);
      currentPos += 6;
      Display.DrawString(NumberToChar(now.second() % 10), 1, currentPos);
      currentPos += 6;
    }

    if (now.hour() < 12)
      Display.DrawString(" AM", 3, currentPos);
    else
      Display.DrawString(" PM", 3, currentPos);
  }
  else {
    if (Serial.available() > 0)
      GetSerialData();

    firstStrPos--;
    newStrPos--;

    int CurrentStringPosition = firstStrPos;
    while (CurrentStringPosition <= 72) {

      byte bufferIndex = ActiveBufferIndex;
      if (CurrentStringPosition >= newStrPos && BufferLenghts[(ActiveBufferIndex + 1) % __NUM_TEXT_BUFFERS__] != 0)
        bufferIndex = (ActiveBufferIndex + 1) % __NUM_TEXT_BUFFERS__;

      int endOfStr = CurrentStringPosition + (BufferLenghts[bufferIndex] * 5 + BufferLenghts[bufferIndex] - 1); //CurrentStringPosition + length of str in pixels

      if (endOfStr < 0) {
        // if this string is not on screen, move positions and comtinue
        CurrentStringPosition = endOfStr + offset;
        firstStrPos = CurrentStringPosition;
      } else {
        Display.DrawString(TextBuffers[bufferIndex], BufferLenghts[bufferIndex], CurrentStringPosition);
        CurrentStringPosition = endOfStr + offset;
      }
    }
  }
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
