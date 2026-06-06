/***
 * @file    lcd_i2c.c
 * @author  Liron Shteinfer
 * @brief   LCD 1602 driver over I2C (PCF8574 expander) for STM32F446RE
 *
 * @details Implements HD44780 protocol in 4-bit mode over I2C,
 * 			via PCF8574 I/O expander. All register access is done
 * 			without HAL.
 *
 */

#include <lcd_i2c.h>
#include <stdint.h>
#include "stm32f446xx.h"

/***
 * Private Functions
 */

/***
 * @brief  Blocking microsecond delay.
 * @note   Calibrated for 16MHz HSI clock (no PLL).
 * 		   Not cycle-accurate - use only for LCD timing.
 * 		   This loop may be optimized away with -O2/-O3.
 * 		   Consider using DWT cycle counter for production use.
 * @param  us: Number of microseconds to wait
 */
static void delay_us(uint32_t us)
{
	uint32_t count = us * 3; /* Approximate delay */
	while(count--)
	{
		__NOP();
	}
}

/***
 * @brief  Transmits a single byte to the PCF8574 expander over I2C.
 * @details Performs a complete I2C transaction:
 * 			START -> Address -> Data -> STOP
 * @param  data: Byte to transmit
 */
static void I2C_Transmit_Raw(uint8_t data)
{
	/* Generate START condition */
	I2C1->CR1 |= I2C_CR1_START;
	while(!(I2C1->SR1 & I2C_SR1_SB));

	/* Send slave address with write bit (bit0 = 0) */
	I2C1->DR = (LCD_I2C_SLAVE_ADDR << 1);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Clear ADDR flag by reading SR1 then SR2 */
	uint32_t temp = I2C1->SR1;
	temp = I2C1->SR2;
	(void)temp;

	/* Send data byte */
	I2C1->DR = data;
	while(!(I2C1->SR1 & I2C_SR1_TXE));

	/* Generate STOP condition */
	I2C1->CR1 |= I2C_CR1_STOP;
}

/***
 * @brief  Generates an EN (Enable) pulse to latch data into the LCD.
 * @details Sends data with EN high, waits, then sends with EN low.
 * 			Backlight bit is always set.
 * @param  data: Nibble with control bits (RS, RW) already set
 */
static void LCD_Pulse_Enable(uint8_t data)
{
	I2C_Transmit_Raw(data | LCD_BIT_EN | LCD_BIT_BACKLIGHT);	/* EN high */
	delay_us(LCD_DELAY_EN_PULSE_US);
	I2C_Transmit_Raw((data & ~LCD_BIT_EN) | LCD_BIT_BACKLIGHT);	/* EN low */
	delay_us(LCD_DELAY_EN_SETTLE_US);
}

/***
 * @brief  Sends a full byte to the LCD in two 4-bit nibbles.
 * @details Splits the byte into high and low nibbles and sends
 * 			each with an EN pulse, as required by 4-bit mode.
 * @param  value: Byte to send (command or data)
 * @param  mode:  0 for command, LCD_BIT_RS for data
 */
static void LCD_Send(uint8_t value, uint8_t mode)
{
	uint8_t high_nibble = (value & 0xF0);
	high_nibble |= mode;
	LCD_Pulse_Enable(high_nibble);
	uint8_t low_nibble = ((value << 4) & 0xF0);
	low_nibble |= mode;
	LCD_Pulse_Enable(low_nibble);
}

/***
 * @brief  Sends a command byte to the LCD (RS = 0).
 * @param  cmd: HD44780 command byte.
 */
static void LCD_Send_Command(uint8_t cmd)
{
	LCD_Send(cmd, 0);
}

/***
 * @brief  Sends a data byte to the LCD (RS = 1).
 * @param  data: ASCII character to display.
 */
static void LCD_Send_Data(uint8_t data)
{
	LCD_Send(data, LCD_BIT_RS);
}

/***
 * brief  Sends a single nibble to the LCD during initialization sequence.
 * @details Used only during the HD44780 reset sequence before
 * 			4-bit mode is fully configured.
 * @param  nibble: Upper 4 bits contain the nibble to send
 * @param  mode:   Control bits (RS, RW)
 */
static void LCD_Send_Nibble(uint8_t nibble, uint8_t mode)
{
	uint8_t data = ((nibble & 0xF0) | mode | LCD_BIT_BACKLIGHT);
	LCD_Pulse_Enable(data);
}

/***
 * Public Functions
 */

/***
 * @brief  Initializes the LCD display.
 * @details Follows the HD44780 initialization sequence for 4-bit mode:
 * 		1. Wait >40ms after power-on
 * 		2. Send 0x30 three times (8-bit mode reset sequence)
 * 		3. Switch to 4-bit mode (0x20)
 * 		4. Configure: 2 lines, 5x8 font
 * 		5. Display ON, cursor OFF, blink OFF
 * 		6. Clear display
 * 		7. Entry mode: increment cursor, no shift
 */
void LCD_Init(void)
{
	delay_us(LCD_DELAY_POWER_ON_US);	/* Wait >40ms after power-on */

	/* HD44780 reset sequence (8-bit mode) */
	LCD_Send_Nibble(0x30, 0);
	delay_us(LCD_DELAY_RESET_FIRST_US);
	LCD_Send_Nibble(0x30, 0);
	delay_us(LCD_DELAY_RESET_US);
	LCD_Send_Nibble(0x30, 0);
	delay_us(LCD_DELAY_RESET_US);

	/* Switch to 4-bit mode */
	LCD_Send_Nibble(0x20, 0);
	delay_us(LCD_DELAY_4BIT_SWITCH_US);

	/* Function Set: 4-bit, 2 lines, 5x8 font */
	LCD_Send_Command(LCD_CMD_FUNCTION_SET | LCD_2_LINE | LCD_5x8_DOTS);

	/* Display ON, cursor OFF, blink OFF */
	LCD_Send_Command(LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_ON);

	/* Clear display */
	LCD_Clear();

	/* Entry mode: increment cursor, no display shift */
	LCD_Send_Command(LCD_CMD_ENTRY_MODE_SET | LCD_ENTRY_INCREMENT);
}

/***
 * @brief  Clears the display and returns cursor to home position.
 * @note   Requires 2ms delay after execution.
 */
void LCD_Clear(void)
{
	LCD_Send_Command(LCD_CMD_CLEAR_DISPLAY);
	delay_us(LCD_DELAY_CLEAR_US);
}

/***
 * @brief  Sets the cursor position on the display.
 * @param  row: Display row (0 = first row, 1 = second row)
 * @param  col: Display column (0-15)
 */
void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t address = col;
	if(row == 1)
	{
		address += 0x40;	/* Second row starts at DDRAM address 0x40 */
	}
	LCD_Send_Command(LCD_CMD_SET_DDRAM_ADDR | address);
}

/***
 * @brief  Sends a null-terminated string to the display.
 * @param  str: Pointer to the string to display
 */
void LCD_SendString(const char*str)
{
	while(*str)
	{
		LCD_Send_Data((uint8_t)*str);
		++str;
	}
}
