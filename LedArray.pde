#include <LedArray.h>

//Green:  2 Serial
//Yellow: 3 Clock
//Brown:  4 Storage/Latch
//Blue:   5 OutputEnable - active low 

//Sprite Q(Texture2D(5, 5, B11111, B11101, B10001, B10111, B11111), 0, 1);
//Sprite X(Texture2D(5, 5, B11111, B10101, B11011, B10101, B11111), 50, 1);
//Sprite Y(Texture2D(5, 5, B10101, B00100, B11011, B00100, B10101), 45, 1);
//Sprite Z(Texture2D(5, 5, B10001, B10101, B11111, B10101, B10001), 40, 1);

const byte DataPin = 2;
const byte ClockPin = 3;
const byte LatchPin = 4;
const byte OutputEnable = 5;

const byte ScrollSpeedPin = 0;
const byte SecondPot = 1;

LedArray Display(DataPin, ClockPin, LatchPin);

long previousMillis;
long interval =  80;

const char proto[] = "MESSAGE:";

const byte MAX_STRING_LENGTH = 128;

char CurrentText[MAX_STRING_LENGTH] = "The Cake is a Lie";
byte CurrentLenght = 17;
//char CurrentText[MAX_STRING_LENGTH] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz!\"#$%&`()*+'-./0123456789:;<=>?@[]\\^_{|}~";
//byte CurrentLenght = 95;
char IncomingText[MAX_STRING_LENGTH];
byte IncomingLenght = 0;
byte IncomingIndex = 0;
bool NewMessage = false;
bool NewSecond = false;
bool NewThird = false;

int strPos = 72;

// ======== Main ========
void setup()
{
  pinMode(OutputEnable, OUTPUT);
  digitalWrite(OutputEnable, LOW);
  
  Display.Begin();  

  Serial_Wait();
}

void loop()
{  
  //millis() will wrap after ~50 days
  long currentMillis = millis();
  if (currentMillis - previousMillis > interval || currentMillis < previousMillis) {
    previousMillis = millis();
    digitalWrite(OutputEnable, HIGH);
    Update();
    digitalWrite(OutputEnable, LOW);
  }

  Display.Show(); 
 
 //set scroll speed based on Pot
 // map the interval range from 0ms to 200ms
 interval = map(analogRead(ScrollSpeedPin), 0, 1023, 0, 200);
 
}

// ============ Functions ===================


//TODO:
//  update font2 to diff withs
//    create length array
//  update text looping

void Update()
{
  Display.ClearBuffer();
  
  if (Serial.available() > 0) 
    GetSerialData();
  

  strPos--;
  
  // +CurrentLenght for the spaces between the chars 
  // --MINUS Spaces! * 2--
  int CurrentStrLenghtInPixels = CurrentLenght * 5 + CurrentLenght - 1;
  int IncomingStrLenghtInPixels = IncomingLenght * 5 + IncomingLenght - 1;

  // end of first string
  int endOfStr = strPos + CurrentStrLenghtInPixels;

  // the distance between the 2 strings
  int offset = 20;
  
  if (endOfStr < 0)
  {
    strPos = endOfStr + offset;
    if (NewSecond)
    {
      //copy incoming to current
      for(int i = 0; i < IncomingLenght + 1 && i < MAX_STRING_LENGTH; i++)        
          CurrentText[i] = IncomingText[i];  
      CurrentLenght = IncomingLenght;
      
      NewMessage = false;
      NewSecond = false;
      NewThird = false;
    }
    endOfStr = strPos + CurrentLenght * 5 + CurrentLenght - 1;
  }
  
  //I am assuming that only 3 strings would fit on the board at one time
  //Draw First String
  Display.DrawString(CurrentText, CurrentLenght, strPos);
  
  //Draw Second String
  if ((NewMessage && endOfStr + offset > 71) || NewSecond)
    NewSecond = true;
    
  if (NewSecond)
    Display.DrawString(IncomingText, IncomingLenght, endOfStr + offset); 
  else
    Display.DrawString(CurrentText, CurrentLenght, endOfStr + offset);      
    
  //Draw Third String
  if (NewMessage && endOfStr + offset > 71)
    NewThird = true;
    
  if (NewThird)
  {
    if (NewSecond)
      Display.DrawString(IncomingText, IncomingLenght, endOfStr + offset + IncomingStrLenghtInPixels + offset);
    else
      Display.DrawString(IncomingText, IncomingLenght, endOfStr + offset + CurrentStrLenghtInPixels + offset);
  }
  else
    Display.DrawString(CurrentText, CurrentLenght, endOfStr + offset + CurrentStrLenghtInPixels + offset);
}

void Serial_Wait()
{
  delay(5000);
  Serial.begin(38400);
}

void GetSerialData()
{
  if (NewMessage)
  {
    Serial.flush();
    return;
  }
  
  //Display.DrawString("B", 1, 67);
  while(Serial.available() > 0) {	
    IncomingText[IncomingIndex] = Serial.read();
    IncomingText[IncomingIndex + 1] = '\0';
    if (IncomingText[IncomingIndex] == 13 || IncomingIndex == 127 || IncomingText[IncomingIndex] == 10)
    {
      Serial.println("NULL found");
      //Display.DrawString("N", 1, 60);
      
      bool isMessage = true;
      for (int k = 0; k < 8; k++)
      {                   
        if (IncomingText[k] != proto[k])          
          isMessage = false;
      }
      
      if (isMessage)
      {
        Serial.println("Message found");
        Serial.println(IncomingIndex, DEC);
        IncomingLenght = IncomingIndex - 8; //minus 8 for the "MESSAGE:"
        for(int i = 0; i < IncomingIndex + 1 && i < MAX_STRING_LENGTH; i++)        
          IncomingText[i] = IncomingText[i + 8];  
        
        NewMessage = true;
      }
      
      IncomingIndex = 0;
      break;
    }
    IncomingIndex++;
  }    
  Serial.println(IncomingText);
}

