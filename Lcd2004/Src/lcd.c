/*
 * lcd.c
 *
 *  Created on: 6 окт. 2018 г.
 *      Author: dima
 */

#include "stm32f1xx_hal.h"
#include "lcd.h"

uint8_t buf[1] = {0};
extern I2C_HandleTypeDef hi2c1;
uint8_t control = 0; // команда управления

void LCD_WriteByteI2CLCD(uint8_t bt)
{
	buf[0] = bt;
	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)LCD_ADDR, buf, 1, 1000);
}

void sendhalfbyte(uint8_t c)
{
	c <<= 4;
	E_high(); // установка линии Е в 1
	HAL_Delay(1);
	LCD_WriteByteI2CLCD(control | c);
	E_low(); // установка линии Е в 0
	HAL_Delay(1);
}

void sendbyte(uint8_t c, uint8_t RS)
{
	if(RS == 0) RS_low();
	else RS_high();
	uint8_t hc = 0;
	hc = c >> 4;
	sendhalfbyte(hc);
	sendhalfbyte(c);
}

void LCD_Clear(void)
{
	sendbyte(0x01, 0);
	HAL_Delay(2);
}

void LCD_SendChar(char ch)
{
	sendbyte(ch, 1);
}

void LCD_String(char* st)
{
	uint8_t i = 0;
	while(st[i] != 0)
	{
		sendbyte(st[i], 1);
		i++;
	}
}

void LCD_SetPos(uint8_t x, uint8_t y)
{
	switch(y)
	{
		case 0:
			sendbyte(x|0x80, 0);
			HAL_Delay(1);
			break;
		case 1:
			sendbyte((0x40+x)|0x80, 0);
			HAL_Delay(1);
			break;
		case 2:
			sendbyte((0x14+x)|0x80, 0);
			HAL_Delay(1);
			break;
		case 3:
			sendbyte((0x54+x)|0x80, 0);
			HAL_Delay(1);
			break;
	}
}

void LCD_ini(void)
{
	HAL_Delay(15);
	sendhalfbyte(0x03);
	HAL_Delay(4);
	sendhalfbyte(0x03);
	HAL_Delay(1);
	sendhalfbyte(0x03);
	HAL_Delay(1);
	sendhalfbyte(0x02);
	HAL_Delay(1);
	sendbyte(0x28, 0);//режим 4 бит, 2 линии (для нашего большого дисплея это 4 линии, шрифт 5х8
	HAL_Delay(1);
	sendbyte(0x0C,0);//дисплей включаем (D=1), курсоры никакие не нужны
	HAL_Delay(1);
	sendbyte(0x01,0);//уберем мусор
	HAL_Delay(2);
	sendbyte(0x06,0);//пишем влево
	HAL_Delay(1);
	led_on();//подсветка
	HAL_Delay(1);
	RW_low(); // запись в дисплей
}

