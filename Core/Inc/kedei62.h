/*
 * kedei62.h
 *
 *  Created on: Jan 18, 2024
 *      Author: Lenovo
 */

#ifndef KEDEI62_H_
#define KEDEI62_H_

#include <stdint.h>
#include <stddef.h>
#include "main.h"

#define LCD_WIDTH 480
#define LCD_HEIGHT 320

extern SPI_HandleTypeDef hspi1;

//#define BLACK   0b0000000000000000  /* 000000 */
//#define NAVY    0b0000000000010000  /* 000080 */
//#define BLUE    0b0000000000011111  /* 0000ff */
//#define GREEN   0b0000010011000000  /* 009900 */
//#define TEAL    0b0000010011010011  /* 009999 */
//#define LIME    0b0000011111100000  /* 00ff00 */
//#define AQUA    0b0000011111111111  /* 00ffff */
//#define MAROON  0b1000000000000000  /* 800000 */
//#define PURPLE  0b1000000000010000  /* 800080 */
//#define OLIVE   0b1001110011000000  /* 999900 */
//#define GRAY    0b1000010000010000  /* 808080 */
//#define SILVER  0b1100111001111001  /* cccccc */
#define RED     0b1111100000000000  /* ff0000 */
//#define FUCHSIA 0b1111100000011111  /* ff00ff */
//#define YELLOW  0b1111111111100000  /* ffff00 */
#define WHITE   0b1111111111111111  /* ffffff */

void lcd_cs_tp(GPIO_PinState PinState);
int lcd_spi_transmit(uint8_t *buff, size_t buff_size);
int lcd_rst(void);
int lcd_cmd(uint8_t cmd);
int lcd_data(uint8_t dat);
int lcd_color(uint16_t col);
int lcd_colorRGB(uint8_t r, uint8_t g, uint8_t b);
void lcd_setrotation(uint8_t m);
void lcd_init(void);
void lcd_setframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void lcd_setwindow(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);
void lcd_fill(uint16_t col);
void lcd_fillframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col);
void lcd_fillframeRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b);
void lcd_fillRGB(uint8_t r, uint8_t g, uint8_t b);

#endif /* KEDEI62_H_ */
