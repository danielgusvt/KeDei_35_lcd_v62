/*
 * kedei62.c
 *
 *  Created on: Jan 18, 2024
 *      Author: Daniel Vasquez
 */

#include "kedei62.h"

/*
 * D7 1 = left, 0 = right
 * D6 1 = Bottom, 0 = Top
 * D5 (Row / Column Exchange) : 1 = Landscape , 0 = portrait
 * D4 Don't care?
 */
const uint8_t lcd_rotations[] = {
	0b10101010, // 0
	0b00001010, // 90
	0b01101010, // 180
	0b11001010,	// 270
};

volatile uint8_t color;
volatile uint16_t lcd_h;
volatile uint16_t lcd_w;

void lcd_cs_tp(GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(TCS_GPIO_Port, TCS_Pin, PinState);
}

int lcd_spi_transmit(uint8_t *buff, size_t buff_size) // TODO spi handler
{
	HAL_StatusTypeDef ret = 0;
	lcd_cs_tp(1);
	HAL_GPIO_WritePin(LCS_GPIO_Port, LCS_Pin, GPIO_PIN_RESET);
	ret = HAL_SPI_Transmit(&hspi1, buff, buff_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCS_GPIO_Port, LCS_Pin, GPIO_PIN_SET);
	lcd_cs_tp(0);

	return ret;
}

int lcd_rst(void) {
	HAL_StatusTypeDef ret = 0;

	uint8_t buff[4];
	buff[0] = 0x00;
	buff[1] = 0x01;
	buff[2] = 0x00;
	buff[3] = 0x00;
	ret = lcd_spi_transmit(&buff[0], sizeof(buff));
	if (ret > 0)
		return ret;
	HAL_Delay(50);

	buff[0] = 0x00;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;
	ret = lcd_spi_transmit(&buff[0], sizeof(buff));
	if (ret > 0)
		return ret;
	HAL_Delay(100);

	buff[0] = 0x00;
	buff[1] = 0x01;
	buff[2] = 0x00;
	buff[3] = 0x00;
	ret = lcd_spi_transmit(&buff[0], sizeof(buff));
	if (ret > 0) {
		return ret;
	} else {
		HAL_Delay(50);
		return 0;
	}
}

int lcd_cmd(uint8_t cmd) {
	uint8_t b1[3];
	b1[0] = 0x11;
	b1[1] = 0x00;
	b1[2] = cmd;
	return lcd_spi_transmit(&b1[0], sizeof(b1));
}

int lcd_data(uint8_t dat) {
	uint8_t b1[3];
	b1[0] = 0x15;
	b1[1] = 0x00;
	b1[2] = dat;
	return lcd_spi_transmit(&b1[0], sizeof(b1));
}

int lcd_color(uint16_t col) {
	uint8_t b1[3];
	b1[0] = 0x15;
	b1[1] = col >> 8;
	b1[2] = col & 0xFF;
	return lcd_spi_transmit(&b1[0], sizeof(b1));
}

// 18bit color mode
int lcd_colorRGB(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t b1[3];

	uint16_t col = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0)
			| ((b >> 3) & 0x001F);

	// 18bit color mode ???
	// 0xF800 R(R5-R1, DB17-DB13)
	// 0x07E0 G(G5-G0, DB11- DB6)
	// 0x001F B(B5-B1, DB5 - DB1)
	// 0x40 = R(R0, DB12), 0x20 = B(B0, DB0)
	b1[0] = 0x15;
	b1[1] = col >> 8;
	b1[2] = col & 0x00FF;
	return lcd_spi_transmit(&b1[0], sizeof(b1));
}

void lcd_setrotation(uint8_t m) //TODO handle ret values
{
	lcd_cmd(0x36);
	lcd_data(lcd_rotations[m]);
	if (m & 1) {
		lcd_h = LCD_WIDTH;
		lcd_w = LCD_HEIGHT;
	} else {
		lcd_h = LCD_HEIGHT;
		lcd_w = LCD_WIDTH;
	}
}

void lcd_init(void) {
	lcd_cs_tp(1);

	// reset display
	lcd_rst();
	// kedei 6.2
	lcd_cmd(0x00);
	HAL_Delay(10);
	lcd_cmd(0xFF);
	lcd_cmd(0xFF);
	HAL_Delay(10);
	lcd_cmd(0xFF);
	lcd_cmd(0xFF);
	lcd_cmd(0xFF);
	lcd_cmd(0xFF);
	HAL_Delay(15);

	// Sleep OUT
	lcd_cmd(0x11);
	HAL_Delay(150);

	// Interface Mode Control
	lcd_cmd(0xB0);
	lcd_data(0x00);

	// Frame Rate control (In Partial Mode/Full Colors)
	lcd_cmd(0xB3);
	lcd_data(0x02); // Division Ratio by 2?
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x00);

	// Command not available on ILI9486L
	lcd_cmd(0xB9);
	lcd_data(0x01);
	lcd_data(0x00);
	lcd_data(0x0F);
	lcd_data(0x0F);

	// Power Control 1
	lcd_cmd(0xC0);
	lcd_data(0x13);
	lcd_data(0x3B);
	lcd_data(0x00);
	lcd_data(0x02);
	lcd_data(0x00);
	lcd_data(0x01);
	lcd_data(0x00);
	lcd_data(0x43);

	lcd_cmd(0xC1);
	lcd_data(0x08);
	lcd_data(0x0F);
	lcd_data(0x08);
	lcd_data(0x08);
	lcd_cmd(0xC4);
	lcd_data(0x11);
	lcd_data(0x07);
	lcd_data(0x03);
	lcd_data(0x04);

	// CABC Control 1
	lcd_cmd(0xC6);
	lcd_data(0x00);

	lcd_cmd(0xC8);
	lcd_data(0x03);
	lcd_data(0x03);
	lcd_data(0x13);
	lcd_data(0x5C);
	lcd_data(0x03);
	lcd_data(0x07);
	lcd_data(0x14);
	lcd_data(0x08);
	lcd_data(0x00);
	lcd_data(0x21);
	lcd_data(0x08);
	lcd_data(0x14);
	lcd_data(0x07);
	lcd_data(0x53);
	lcd_data(0x0C);
	lcd_data(0x13);
	lcd_data(0x03);
	lcd_data(0x03);
	lcd_data(0x21);
	lcd_data(0x00);
	lcd_cmd(0x35);
	lcd_data(0x00);

	// MADCTL (Memory Access Control)
	lcd_cmd(0x36);
//	lcd_data(0x60);
	lcd_data(0xAA);

	lcd_cmd(0x3A);
	lcd_data(0x55);
	lcd_cmd(0x44);
	lcd_data(0x00);
	lcd_data(0x01);
	lcd_cmd(0xD0);
	lcd_data(0x07);
	lcd_data(0x07);
	lcd_data(0x1D);
	lcd_data(0x03);
	lcd_cmd(0xD1);
	lcd_data(0x03);
	lcd_data(0x30);
	lcd_data(0x10);
	lcd_cmd(0xD2);
	lcd_data(0x03);
	lcd_data(0x14);
	lcd_data(0x04);
	lcd_cmd(0x29);

	HAL_Delay(30);

	// Set Start and End to draw all columns
	lcd_cmd(0x2A);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x01);
	lcd_data(0x3F);

	lcd_cmd(0x2B);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x01);
	lcd_data(0xE0);

	// Display inversion - disable
	lcd_cmd(0xB4);
	lcd_data(0x00);

	lcd_cmd(0x2C);

	HAL_Delay(10);
}

void lcd_setframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	/* If we try to draw outside the limits, set drawing dimensions
	 as the maximum available by the desired coordinates */
	if ((x + w) > lcd_w) {
		w = lcd_w - x;
	}
	if ((y + h) > lcd_h) {
		h = lcd_h - y;
	}

	//Start from top left corner
	y = lcd_h - y - h;

	lcd_cmd(0x2A);
	lcd_data(x >> 8);
	lcd_data(x & 0xFF);
	lcd_data(((w + x) - 1) >> 8);
	lcd_data(((w + x) - 1) & 0xFF);

	lcd_cmd(0x2B);
	lcd_data(y >> 8);
	lcd_data(y & 0xFF);
	lcd_data(((h + y) - 1) >> 8);
	lcd_data(((h + y) - 1) & 0xFF);
	lcd_cmd(0x2C);
}


void lcd_setwindow(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y) {
	/*
	 * Start Column[15:0] always must be equal to or less than End Column[15:0]
	 * 1stParameter SC[15:8]
	 * 2ndParameter SC[7:0]
	 * 3rdParameter EC[15:8]
	 * 4thParameter EC[7:0]
	*/
	lcd_cmd(0x2A);
	lcd_data(start_x >> 8);
	lcd_data(start_x & 0xFF);
	lcd_data(end_x >> 8);
	lcd_data(end_x & 0xFF);

	lcd_cmd(0x2B);
	lcd_data(start_y >> 8);
	lcd_data(start_y & 0xFF);
	lcd_data(end_y >> 8);
	lcd_data(end_y & 0xFF);

	//RAMWR
	lcd_cmd(0x2C);
}


// lcd_fillframe
// fills an area of the screen with a single color.
void lcd_fillframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col) {
	int span = h * w;
	int q;

	// First we set the position and area we want to draw on
	lcd_setframe(x, y, w, h);

	for (q = 0; q < span; q++) {
		lcd_color(col);
	}
}

void lcd_fill(uint16_t col) {
	lcd_fillframe(0, 0, lcd_w, lcd_h, col);
}

void lcd_fillframeRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r,
		uint8_t g, uint8_t b) {
	int span = h * w;
	lcd_setframe(x, y, w, h);
	int q;
	for (q = 0; q < span; q++) {
		lcd_colorRGB(r, g, b);
	}
}

void lcd_fillRGB(uint8_t r, uint8_t g, uint8_t b) {
	lcd_fillframeRGB(0, 0, lcd_w, lcd_h, r, g, b);
}
