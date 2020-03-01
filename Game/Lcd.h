#ifndef LCD_H_
#define LCD_H_

void lcdInit();
void drawImage(uint8_t pageStart, uint8_t segStart, uint8_t sizeX, uint8_t sizeY, const unsigned char* img);
void clearRegion(uint8_t page, uint8_t seg, uint8_t pageEnd, uint8_t segEnd);
void clearScreen();

#endif /* LCD_H_ */