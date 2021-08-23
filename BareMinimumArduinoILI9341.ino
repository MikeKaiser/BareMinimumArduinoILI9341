#include "TouchScreen.h"
#include "SdFat.h"

// Touch screen presure threshold
#define MINPRESSURE 40
#define MAXPRESSURE 1000
// Touch screen calibration
const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);



#define LCD_D0  8
#define LCD_D1  9
#define LCD_D2  2
#define LCD_D3  3
#define LCD_D4  4
#define LCD_D5  5
#define LCD_D6  6
#define LCD_D7  7
#define LCD_RST A4
#define LCD_CS  A3
#define LCD_RS  A2
#define LCD_WR  A1
#define LCD_RD  A0
#define SD_SS   10
#define SD_DI   11
#define SD_DO   12
#define SD_SCK  13





SdFs                        sd;
FsFile                      file;
FsFile                      root;

void WaitForSDCardInsert()
{
  while( !sd.begin( SdSpiConfig(SS, DEDICATED_SPI) ) )
  {
      Serial.println( "Insert SD Card" );
      delay(500);   
  }
}

void ListSDCard()
{
  char name[128];
  if (root.open("/"))
  {
    while( file.openNext(&root, O_RDONLY) )
    {
      file.getName( name, 128 );
      Serial.println( name );     
      file.close();
    }
  }
  else
  {
    Serial.println("Failed to open root");
  }  
}

void Wait()
{
  //while( !Serial.available() );
  //Serial.read();
  //delayMicroseconds(10);
}





#define ILI9341_SOFTRESET 0x01
#define ILI9341_SLEEPIN 0x10
#define ILI9341_SLEEPOUT 0x11
#define ILI9341_NORMALDISP 0x13
#define ILI9341_INVERTOFF 0x20
#define ILI9341_INVERTON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPLAYOFF 0x28
#define ILI9341_DISPLAYON 0x29
#define ILI9341_COLADDRSET 0x2A
#define ILI9341_PAGEADDRSET 0x2B
#define ILI9341_MEMORYWRITE 0x2C
#define ILI9341_PIXELFORMAT 0x3A
#define ILI9341_FRAMECONTROL 0xB1
#define ILI9341_DISPLAYFUNC 0xB6
#define ILI9341_ENTRYMODE 0xB7
#define ILI9341_POWERCONTROL1 0xC0
#define ILI9341_POWERCONTROL2 0xC1
#define ILI9341_VCOMCONTROL1 0xC5
#define ILI9341_VCOMCONTROL2 0xC7
#define ILI9341_MEMCONTROL 0x36
#define ILI9341_MADCTL 0x36

#define ILI9341_MADCTL_MY 0x80
#define ILI9341_MADCTL_MX 0x40
#define ILI9341_MADCTL_MV 0x20
#define ILI9341_MADCTL_ML 0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH 0

#define TFT_CS_IDLE     { digitalWrite( LCD_CS, HIGH ); Wait(); }
#define TFT_CS_ACTIVE   { digitalWrite( LCD_CS, LOW  ); Wait(); }

#define TFT_WR_IDLE		  { digitalWrite( LCD_WR, HIGH ); Wait(); }
#define TFT_WR_ACTIVE   { digitalWrite( LCD_WR, LOW  ); Wait(); }
#define TFT_WR_STROBE   { TFT_WR_ACTIVE /*delayMicroseconds(1)*/; TFT_WR_IDLE }

#define TFT_RD_IDLE		  { digitalWrite( LCD_RD, HIGH ); Wait(); }
#define TFT_RD_ACTIVE   { digitalWrite( LCD_RD, LOW  ); Wait(); }
#define TFT_RD_STROBE   { TFT_RD_ACTIVE /*delayMicroseconds(1)*/; TFT_RD_IDLE }

#define TFT_RST_IDLE	  { digitalWrite( LCD_RST, HIGH ); Wait(); }
#define TFT_RST_ACTIVE  { digitalWrite( LCD_RST, LOW  ); Wait(); }

#define TFT_CD_COMMAND	{ digitalWrite( LCD_RS, LOW  ); Wait(); }
#define TFT_CD_DATA		  { digitalWrite( LCD_RS, HIGH ); Wait(); }

#define TFTWIDTH 240
#define TFTHEIGHT 320

uint8_t TFT_Read8()
{
  TFT_SetReadDir();
  TFT_RD_ACTIVE;
  delayMicroseconds(100);
  uint8_t ret = digitalRead( LCD_D0 ) ? 1 : 0;
  ret |= digitalRead( LCD_D1 ) ? 2 : 0;
  ret |= digitalRead( LCD_D2 ) ? 4 : 0;
  ret |= digitalRead( LCD_D3 ) ? 8 : 0;
  ret |= digitalRead( LCD_D4 ) ? 16 : 0;
  ret |= digitalRead( LCD_D5 ) ? 32 : 0;
  ret |= digitalRead( LCD_D6 ) ? 64 : 0;
  ret |= digitalRead( LCD_D7 ) ? 128 : 0;
  Wait();
  TFT_RD_IDLE;
  Wait();
  return ret;
}

void TFT_Write8(uint8_t v)
{
  TFT_SetWriteDir();
  digitalWrite( LCD_D0, (v &   1)?HIGH:LOW );
  digitalWrite( LCD_D1, (v &   2)?HIGH:LOW );
  digitalWrite( LCD_D2, (v &   4)?HIGH:LOW );
  digitalWrite( LCD_D3, (v &   8)?HIGH:LOW );
  digitalWrite( LCD_D4, (v &  16)?HIGH:LOW );
  digitalWrite( LCD_D5, (v &  32)?HIGH:LOW );
  digitalWrite( LCD_D6, (v &  64)?HIGH:LOW );
  digitalWrite( LCD_D7, (v & 128)?HIGH:LOW );
  TFT_WR_STROBE;
}

void TFT_SetReadDir()
{
  digitalWrite( LCD_D0, 0 );
  digitalWrite( LCD_D1, 0 );
  digitalWrite( LCD_D2, 0 );
  digitalWrite( LCD_D3, 0 );
  digitalWrite( LCD_D4, 0 );
  digitalWrite( LCD_D5, 0 );
  digitalWrite( LCD_D6, 0 );
  digitalWrite( LCD_D7, 0 );

  pinMode( LCD_D0, INPUT );
  pinMode( LCD_D1, INPUT );
  pinMode( LCD_D2, INPUT );
  pinMode( LCD_D3, INPUT );
  pinMode( LCD_D4, INPUT );
  pinMode( LCD_D5, INPUT );
  pinMode( LCD_D6, INPUT );
  pinMode( LCD_D7, INPUT );
}

void TFT_SetWriteDir()
{
  pinMode( LCD_D0, OUTPUT );
  pinMode( LCD_D1, OUTPUT );
  pinMode( LCD_D2, OUTPUT );
  pinMode( LCD_D3, OUTPUT );
  pinMode( LCD_D4, OUTPUT );
  pinMode( LCD_D5, OUTPUT );
  pinMode( LCD_D6, OUTPUT );
  pinMode( LCD_D7, OUTPUT );
}

uint32_t TFT_ReadReg(uint8_t r)
{
  uint32_t id0, id1, id2, id3;

  TFT_CS_ACTIVE;
  TFT_CD_COMMAND;
  TFT_Write8(r);
  TFT_CD_DATA;
  delayMicroseconds(50);
  id0 = TFT_Read8();
  id1 = TFT_Read8();
  id2 = TFT_Read8();
  id3 = TFT_Read8();
  TFT_CS_IDLE;

  //Serial.print("Read $"); Serial.print(r, HEX); Serial.print(":\t0x"); Serial.print(id0, HEX); Serial.print(id1, HEX); Serial.print(id2, HEX); Serial.println(id3, HEX);
  return (id0<<24)|(id1<<16)|(id2<<8)|id3;
}



void TFT_WriteRegister8(uint8_t a, uint32_t d)
{
    TFT_CD_COMMAND;
  delayMicroseconds(10);
    TFT_Write8(a);
  delayMicroseconds(10);
    TFT_CD_DATA;
  delayMicroseconds(10);
    TFT_Write8(d);
}

void TFT_WriteRegister16(uint8_t a, uint32_t d)
{
    uint8_t hi, lo;
    hi = (a) >> 8;
    lo = (a);
    TFT_CD_COMMAND;
  delayMicroseconds(10);
    TFT_Write8(hi);
    TFT_Write8(lo);
    hi = (d) >> 8;
    lo = (d);
    TFT_CD_DATA;
  delayMicroseconds(10);
    TFT_Write8(hi);
    TFT_Write8(lo);
}

void TFT_WriteRegister24(uint8_t r, uint32_t d)
{
  TFT_CS_ACTIVE;
  TFT_CD_COMMAND;
  TFT_Write8(r);
  TFT_CD_DATA;
  delayMicroseconds(10);
  TFT_Write8(d >> 16);
  delayMicroseconds(10);
  TFT_Write8(d >> 8);
  delayMicroseconds(10);
  TFT_Write8(d);
  TFT_CS_IDLE;
}

void TFT_WriteRegister32(uint8_t r, uint32_t d)
{
  TFT_CS_ACTIVE;
  TFT_CD_COMMAND;
  TFT_Write8(r);
  TFT_CD_DATA;
  delayMicroseconds(10);
  TFT_Write8(d >> 24);
  delayMicroseconds(10);
  TFT_Write8(d >> 16);
  delayMicroseconds(10);
  TFT_Write8(d >> 8);
  delayMicroseconds(10);
  TFT_Write8(d);
  TFT_CS_IDLE;
}


void TFT_SetAddrWindow( int x1, int y1, int x2, int y2 )
{
    uint32_t t;
    TFT_CS_ACTIVE;

    t = x1;
    t <<= 16;
    t |= x2;
    TFT_WriteRegister32(ILI9341_COLADDRSET, t);
    t = y1;
    t <<= 16;
    t |= y2;
    TFT_WriteRegister32(ILI9341_PAGEADDRSET, t);
    TFT_CS_IDLE;

}


uint16_t TFT_Init()
{
  TFT_CS_IDLE;
  TFT_WR_IDLE;
  TFT_RD_IDLE;
  TFT_RST_ACTIVE;
  delay(20);
  TFT_RST_IDLE;
  TFT_CS_ACTIVE;
  TFT_CD_COMMAND;
  TFT_Write8(0x00);
  for (uint8_t i = 0; i < 3; i++)
    TFT_WR_STROBE; // Three extra 0x00s
  TFT_CS_IDLE;


  //-------------------------
  // Read the LCD ID
  //-------------------------
  uint16_t id;

  for (int i = 0; i < 5; i++)
  {
    id = (uint16_t)TFT_ReadReg(0xD3);
    delayMicroseconds(50);
    if (id == 0x9341)
      return id;
  }

  return 0;
}

void TFT_Flood(uint16_t color, uint32_t len)
{
  uint16_t blocks;
  uint8_t i, hi = color >> 8, lo = color;

  TFT_CS_ACTIVE;
  TFT_CD_COMMAND;
  TFT_Write8(0x2C);

  // Write first pixel normally, decrement counter by 1
  TFT_CD_DATA;
  TFT_Write8(hi);
  TFT_Write8(lo);
  len--;

  blocks = (uint16_t)(len / 64); // 64 pixels/block
  if (hi == lo)
  {
    // High and low bytes are identical.  Leave prior data
    // on the port(s) and just toggle the write strobe.
    while (blocks--)
    {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do
      {
        TFT_WR_STROBE;
        TFT_WR_STROBE;
        TFT_WR_STROBE;
        TFT_WR_STROBE; // 2 bytes/pixel
        TFT_WR_STROBE;
        TFT_WR_STROBE;
        TFT_WR_STROBE;
        TFT_WR_STROBE; // x 4 pixels
      } while (--i);
    }
    // Fill any remaining pixels (1 to 64)
    for (i = (uint8_t)len & 63; i--;) {
      TFT_WR_STROBE;
      TFT_WR_STROBE;
    }
  } else {
    while (blocks--)
    {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do
      {
        TFT_Write8(hi);
        TFT_Write8(lo);
        TFT_Write8(hi);
        TFT_Write8(lo);
        TFT_Write8(hi);
        TFT_Write8(lo);
        TFT_Write8(hi);
        TFT_Write8(lo);
      } while (--i);
    }
    for (i = (uint8_t)len & 63; i--;)
    {
      TFT_Write8(hi);
      TFT_Write8(lo);
    }
  }
  TFT_CS_IDLE;
}


void TFT_FillScreen(uint16_t color)
{
  // For these, there is no settable address pointer, instead the
  // address window must be set for each drawing operation.  However,
  // this display takes rotation into account for the parameters, no
  // need to do extra rotation math here.
  TFT_SetAddrWindow(0, 0, TFTWIDTH - 1, TFTHEIGHT - 1);
  TFT_Flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

void TFT_Begin()
{
    TFT_CS_ACTIVE;
    TFT_WriteRegister8(ILI9341_SOFTRESET, 0);
    delay(50);
    TFT_WriteRegister8(ILI9341_DISPLAYOFF, 0);

    TFT_WriteRegister8(ILI9341_POWERCONTROL1, 0x23);
    TFT_WriteRegister8(ILI9341_POWERCONTROL2, 0x10);
    TFT_WriteRegister16(ILI9341_VCOMCONTROL1, 0x2B2B);
    TFT_WriteRegister8(ILI9341_VCOMCONTROL2, 0xC0);
    TFT_WriteRegister8(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    TFT_WriteRegister8(ILI9341_PIXELFORMAT, 0x55);
    TFT_WriteRegister16(ILI9341_FRAMECONTROL, 0x001B);

    TFT_WriteRegister8(ILI9341_ENTRYMODE, 0x07);
    /* TFT_WriteRegister32(ILI9341_DISPLAYFUNC, 0x0A822700);*/

    TFT_WriteRegister8(ILI9341_SLEEPOUT, 0);
    delay(150);
    TFT_WriteRegister8(ILI9341_DISPLAYON, 0);
    delay(500);
    TFT_SetAddrWindow(0, 0, TFTWIDTH - 1, TFTHEIGHT - 1);
}



//////////////////////////////////////////////////////////////
// ARDUINO SETUP
//////////////////////////////////////////////////////////////

void setup()
{
  pinMode( A5, OUTPUT );
  digitalWrite( A5, 0 );

  Serial.begin(9600);
  Serial.println("CNC Controller Reset");

  
  pinMode( LCD_RST , OUTPUT );
  pinMode( LCD_CS  , OUTPUT );
  pinMode( LCD_RS  , OUTPUT );
  pinMode( LCD_WR  , OUTPUT );
  pinMode( LCD_RD  , OUTPUT );

  digitalWrite( A5, 1 );

  uint16_t id = TFT_Init();
  Serial.print("ID = ");
  Serial.println(id,16);
  TFT_Begin();
  TFT_FillScreen( 0x0FF0 );
  
  //WaitForSDCardInsert();
  //ListSDCard();
 
  touchToStart();
  Serial.println("Screen Touched");
}
//////////////////////////////////////////////////////////////
// ARDUINO LOOP
//////////////////////////////////////////////////////////////

void loop(void)
{
  TSPoint tp = getTouch();
  char buf[10];
  if( tp.z > MINPRESSURE )
  {
    Serial.print("Screen Touched ");
    Serial.print(tp.x);
    Serial.print(" ");
    Serial.print(tp.y);
    Serial.print(" ");
    Serial.println(tp.z);
  }
}




void touchToStart() {
  while (waitForTouch() < 0) {}
}

TSPoint getTouch()
{
  TSPoint tp = ts.getPoint();   //tp.x, tp.y are ADC values

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  return tp;
}

int waitForTouch()
{
  TSPoint tp = getTouch();
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE)
  {
    return 1;
  }
  return -1;
}
