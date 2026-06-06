/**
 ******************************************************************************
 * @file    main.c
 * @author  Liron Shteinfer
 * @brief   Main program - LCD 1602 I2C demo on STM32F446RE
 *
 * @details Demonstrates driving an LCD 1602 display over I2C
 * 			using a PCF8574 expander, via bare-metal register access.
 *
 * Hardware:
 * 		MCU  : STM32F446RE (Nucleo-64)
 * 		LCD  : 1602 with PCF8574 I2C expander (address 0x27)
 * 		SCL  : PB8
 * 		SDA  : PB9
 *
 ******************************************************************************
 */
#include "stm32f446xx.h"
#include "i2c.h"
#include "lcd_i2c.h"


int main(void)
{
	/* Initialize I2C1 peripheral (PB8=SCL, PB9=SDA, ~200kHz) */
	I2C1_Init();

	/* Initialize LCD and clear screen */
	LCD_Init();
	LCD_Clear();

	/* Display text */
	LCD_SetCursor(0, 0);
	LCD_SendString("Embedded C");

	LCD_SetCursor(1, 2);
	LCD_SendString("STM32F446RE");

    /* Loop forever */
	for(;;)
	{

	}
}
