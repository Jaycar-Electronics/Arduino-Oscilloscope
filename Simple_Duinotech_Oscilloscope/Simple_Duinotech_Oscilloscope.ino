// Simple Duinotech Oscilloscope
// using Leonardo and XC4616 display
// max sample rate of 726 columns/sec (column takes 1377 us) or about 8.6 frames/second (84 columns/screen)
// generates a reference tone on D13 for timebase checking

// plug the 8 pin header straight into d0-d7, LED on pin D0, VCCon pin D7
// VCC  GND  SCE  RST  DC   MOSI SCLK LED
//  0    1    2    3    4    5    6    7
// LED must be off, as it needs 3.3V not 5V

#define PIN_VCC 0
#define PIN_GND 1
#define PIN_SCE   2
#define PIN_RESET 3
#define PIN_DC    4
#define PIN_SDIN  5
#define PIN_SCLK  6

//analog input pin
#define PIN_INPUT A0

//tone output reference pin and frequency
#define PIN_TONE 13
#define TONE_FREQ 50

#define LCD_C     LOW
#define LCD_D     HIGH

byte bitdata[8]={128,64,32,16,8,4,2,1};   //array to speed up conversion of value to pixel position;

void setup(void)
{
  delay(100);
  LcdInitialise();
  tone(PIN_TONE,TONE_FREQ);
}

void loop(void)
{
  //LcdSety(0);
  int data;
  byte dispdata[6]={0,0,0,0,0,0};   //pixels to display per row (could change this to something like (2,0,0,1,0,128) to give rudimentary gridlines
  data =analogRead(PIN_INPUT)/22;    //read input and convert from 0-1023 to 0-47
  int bt;
  bt=5-data/8;
//  dispdata[bt]=dispdata[bt]|bitdata[data&7];   //set pixel (use this version if pixels are non zero)
  dispdata[bt]=bitdata[data&7];   //set pixel
  digitalWrite(PIN_DC, LCD_D);      //unroll display function to speed it up
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[0]);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[1]);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[2]);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[3]);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[4]);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, dispdata[5]);
  digitalWrite(PIN_SCE, HIGH);
  delayMicroseconds(51);     //change timebase by changing delay between cycles- 51=> 700Hz sample rate, gives a stable display of 50Hz wave
}

void LcdClear(void)
{
  for (int index = 0; index < 504; index++){LcdWrite(LCD_D, 0x00);}
}

void LcdInitialise(void)
{
  pinMode(PIN_VCC, OUTPUT);    //VCC
  digitalWrite(PIN_VCC,HIGH);

  pinMode(PIN_GND, OUTPUT);    //GND
  digitalWrite(PIN_GND,LOW);
  
  //Leave LED floating (off)

  pinMode(PIN_SCE,   OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC,    OUTPUT);
  pinMode(PIN_SDIN,  OUTPUT);
  pinMode(PIN_SCLK,  OUTPUT);

  digitalWrite(PIN_RESET, LOW);
  delay(100);
  digitalWrite(PIN_RESET, HIGH);

  LcdWrite( LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite( LCD_C, 0xB0 );  // Set LCD Vop (Contrast). //A8-B1 TRY ADJUSTING THIS IF IT'S NOT WORKING 80-FF are valid
  LcdWrite( LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite( LCD_C, 0x14 );  // LCD bias mode 1:40. //0x14
  LcdWrite(LCD_C, 0x20);
  LcdWrite(LCD_C, 0x22);    //display on, vertical scan (0x20 is horizontal scan)
  LcdWrite(LCD_C, 0x0C); //display on and normal
//  LcdWrite(LCD_C, 0x0D); //display on and inverted
  LcdClear();
  LcdSetx(0);
  LcdSety(0);
}

void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void LcdSetx(byte x)
{
  if(x<84)
    {LcdWrite(LCD_C,0x80 | x);
    }
}

void LcdSety(byte y)
{
  if(y<6)
    {LcdWrite(LCD_C,0x40 | y);
    }
}
