/***
 * @file    lcd_i2c.h
 * @author  Liron Shteinfer
 * @brief   Header file for LCD 1602 driver over I2C (PCF8574 expander)
 *
 * @details This driver controls an LCD 1602 display connected via I2C,
 * 			using a PCF8574 I/O expander with default address 0x27.
 * 			Communication is in 4-bit mode via direct register access,
 * 			without HAL.
 *
 * Wiring (PCF8574 pin mapping):
 * 		Bit 0 -> RS  (Register Select)
 * 		Bit 1 -> RW  (Read/Write)
 * 		Bit 2 -> EN  (Enable)
 * 		Bit 3 -> BL  (Backlight)
 * 		Bit 4 -> D4
 * 		Bit 5 -> D5
 * 		Bit 6 -> D6
 * 		Bit 7 -> D7
 *
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdint.h>

/***
 * I2C Configuration
 *
 */

/* @brief I2C slave address of the PCF8574 expander (default: 0x27) */
#define LCD_I2C_SLAVE_ADDR 0x27U

/***
 * PCF8574 Control Bits
 */


#define LCD_BIT_RS (1U << 0)		/* @brief Register Select: 0 = Command, 1 = Data */
#define LCD_BIT_RW (1U << 1)		/* @brief Read/Write: 0 = Write, 1 = Read */
#define LCD_BIT_EN (1U << 2)		/* @brief Enable pulse bit */
#define LCD_BIT_BACKLIGHT (1U << 3)	/* @brief Backlight control bit */
#define LCD_MASK_DATA 0xF0U			/* @brief Data bits mask (upper nibble) */

/***
 * LCD Commands
 */
#define LCD_CMD_CLEAR_DISPLAY 0x01U		/* @brief Clear display and return cursor to home */
#define LCD_CMD_RETURN_HOME 0x02U		/* @brief Return cursor to home position */
#define LCD_CMD_ENTRY_MODE_SET 0x04U	/* @brief Set entry mode (cursor direction) */
#define LCD_CMD_DISPLAY_CONTROL 0x08U	/* @brief Set display, cursor, and blink on/off */
#define LCD_CMD_CURSOR_SHIFT 0x10U		/* @brief Shift display or cursor */
#define LCD_CMD_FUNCTION_SET 0x20U		/* @brief Set interface length, lines, and font */
#define LCD_CMD_SET_CGRAM_ADDR 0x40U	/* @brief Set CGRAM address */
#define LCD_CMD_SET_DDRAM_ADDR 0x80U	/* @brief Set DDRAM address (cursor position) */

/***
 * Display Control Flags
 */
#define LCD_DISPLAY_ON 0x04U	/* @brief Turn display on */
#define LCD_CURSOR_ON 0x02U		/* @brief Turn cursor on */
#define LCD_BLINK_ON 0x01U		/* @brief Turn cursor blink on */
/***
 * Function Set Flags
 */
#define LCD_4BIT_MODE 0x00U		/* @brief 4-bit interface mode */
#define LCD_2_LINE 0x08U		/* @brief 2-line display mode */
#define LCD_5x8_DOTS 0x00U		/* @brief 5x8 dots font */

/***
 * Timing Constants (microseconds) - calibrated for 16MHz HSI
 */
#define LCD_DELAY_POWER_ON_US 50000U	/* @brief Delay after power-on before initialization */
#define LCD_DELAY_RESET_FIRST_US 5000U	/* @brief Delay after first reset nibble */
#define LCD_DELAY_RESET_US 100U			/* @brief Delay after second and third reset nibble */
#define LCD_DELAY_4BIT_SWITCH_US 1000U	/* @brief Delay after switching to 4-bit mode */
#define LCD_DELAY_CLEAR_US 2000U		/* @brief Delay after clear display command */
#define LCD_DELAY_EN_PULSE_US 2U		/* @brief EN pulse width */
#define LCD_DELAY_EN_SETTLE_US 50U		/* @brief EN settle time after pulse */

/***
 * Entry Mode Flags
 */
#define LCD_ENTRY_INCREMENT 0x02U		/* @brief Increment cursor after each character (left to right) */


/***
 * Public API
 */

/***
 * @brief  Initializes the LCD display.
 * @details Performs the HD44780 initialization sequence in 4-bit mode,
 * 			sets 2-line display, turns on display, and clears screen.
 */
void LCD_Init(void);

/***
 * @brief  Clears the display and returns cursor to home position.
 */
void LCD_Clear(void);

/***
 * @brief  Sends a null-terminated string to the display.
 * @param  str: Pointer to the string to display
 */
void LCD_SendString(const char *str);

/***
 * @brief  Sets the cursor position on the display.
 * @param  row: Display row (0 = first row, 1 = second row)
 * @param  col: Display column (0-15)
 */
void LCD_SetCursor(uint8_t row, uint8_t col);






#endif /* LCD_I2C_H_ */
