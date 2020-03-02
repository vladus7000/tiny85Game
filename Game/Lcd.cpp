#define F_CPU 10000000
#include <avr/io.h>
#include <avr/delay.h>
#include "TinyWireM.h"

#define BLACK                          0 ///< Draw 'off' pixels
#define WHITE                          1 ///< Draw 'on' pixels
#define INVERSE                        2 ///< Invert pixels

#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range

#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ADDR 0x3C
void ssd1306_command1(uint8_t c)
{
	TinyWireM.beginTransmission(ADDR);
	TinyWireM.send(0);
	TinyWireM.send(c);
	TinyWireM.endTransmission();
}


void ssd1306_commandList(const uint8_t *c, uint8_t n) {
	
	TinyWireM.beginTransmission(ADDR);
	TinyWireM.send(0);
	//WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
	uint8_t bytesOut = 1;
	while(n--) {
		//if(bytesOut >= WIRE_MAX) {
		//	wire->endTransmission();
		//	wire->beginTransmission(i2caddr);
		//	WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
		//	bytesOut = 1;
		//}
		//WIRE_WRITE(pgm_read_byte(c++));
		TinyWireM.send(*c++);
		bytesOut++;
	}
	TinyWireM.endTransmission();
	
}

void lcdInit()
{
// Init sequence
static const uint8_t  init1[] = {
	SSD1306_DISPLAYOFF,                   // 0xAE
	SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
	0x80,                                 // the suggested ratio 0x80
SSD1306_SETMULTIPLEX };               // 0xA8
ssd1306_commandList(init1, sizeof(init1));
ssd1306_command1(SCREEN_HEIGHT - 1);

static const uint8_t init2[] = {
	SSD1306_SETDISPLAYOFFSET,             // 0xD3
	0x0,                                  // no offset
	SSD1306_SETSTARTLINE | 0x0,           // line #0
SSD1306_CHARGEPUMP };                 // 0x8D
ssd1306_commandList(init2, sizeof(init2));

ssd1306_command1( 0x14);

static const uint8_t  init3[] = {
	SSD1306_MEMORYMODE,                   // 0x20
	0x00,                                 // 0x0 act like ks0108
	SSD1306_SEGREMAP | 0x1,
SSD1306_COMSCANDEC };
ssd1306_commandList(init3, sizeof(init3));

static const uint8_t init4b[] = {
	SSD1306_SETCOMPINS,                 // 0xDA
	0x12,
SSD1306_SETCONTRAST };              // 0x81
ssd1306_commandList(init4b, sizeof(init4b));

ssd1306_command1( 0xCF);

ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9

ssd1306_command1( 0xF1);

static const uint8_t  init5[] = {
	SSD1306_SETVCOMDETECT,               // 0xDB
	0x40,
	SSD1306_DISPLAYALLON_RESUME,         // 0xA4
	SSD1306_NORMALDISPLAY,               // 0xA6
	SSD1306_DEACTIVATE_SCROLL,
SSD1306_DISPLAYON };                 // Main screen turn on

ssd1306_commandList(init5, sizeof(init5));
}

void drawImage(uint8_t pageStart, uint8_t segStart, uint8_t sizeX, uint8_t sizeY, const unsigned char* img)
{
	const uint8_t cmd[] = {SSD1306_PAGEADDR, pageStart, (uint8_t)(pageStart+(sizeY+7)/8 -1) , SSD1306_COLUMNADDR, segStart, (uint8_t)(segStart+sizeX-1) };
	uint8_t bytesOut = 1;
	ssd1306_commandList(cmd, sizeof(cmd));
	TinyWireM.beginTransmission(ADDR);
	TinyWireM.send((uint8_t)0x40);
	
	for (uint8_t page = 0; page < (sizeY+7)/8; page++)
	{
		for (uint8_t seg = 0; seg < sizeX; seg++)
		{
			if(bytesOut >= 16)
			{
				TinyWireM.endTransmission();
				TinyWireM.beginTransmission(ADDR);
				TinyWireM.send((uint8_t)0x40);
				bytesOut = 1;
			}
			bytesOut++;
			TinyWireM.send(*img++);
		}
	}
	TinyWireM.endTransmission();
}

void clearRegion(uint8_t page, uint8_t seg, uint8_t pageEnd, uint8_t segEnd)
{
	const uint8_t egg_cmd[] = {SSD1306_PAGEADDR, page, (uint8_t)(pageEnd), SSD1306_COLUMNADDR, seg, (uint8_t)(segEnd) };
	uint8_t bytesOut = 1;
	ssd1306_commandList(egg_cmd, sizeof(egg_cmd));

	TinyWireM.beginTransmission(ADDR);
	TinyWireM.send((uint8_t)0x40);
	
	for (uint8_t page = 0; page < pageEnd+1; page++)
	{
		for (uint8_t seg = 0; seg < segEnd+1; seg++)
		{
			if(bytesOut >= 16)
			{
				TinyWireM.endTransmission();
				TinyWireM.beginTransmission(ADDR);
				TinyWireM.send((uint8_t)0x40);
				bytesOut = 1;
			}
			bytesOut++;
			char data = 0x0;
			TinyWireM.send(data);
		}
	}
	TinyWireM.endTransmission();
}

void clearScreen()
{
	clearRegion(0, 0, 7, 127);
}