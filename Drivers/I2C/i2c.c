/***
 *  @file i2c.c
 *  @author Liron Shteinfer
 *  @brief I2C1 peripheral initialization for STM32F446RE
 *
 * @details Configures I2C1 in Fast Mode (~200kHz) using PB8 (SCL) and PB9 (SDA).
 * 			All configuration is done via direct register access, without HAL.
 *
 * Pin Mapping:
 * 		PB8-> I2C1_SCL
 * 		PB9-> I2C1_SDA
 *
 * Clock Assumptions:
 * 		APB1 clock = 16MHZ (default HSI)
 *
 */

#include "i2c.h"
#include "stm32f446xx.h"

/***
 * @brief  Initializes I2C1 peripheral in Fast Mode at ~200kHz.
 *
 * @details Performs the following steps:
 * 		1. Enable GPIOB and I2C1 clocks.
 * 		2. Configure PB8, PB9 as Alternate Function.
 * 		3. Set Open-Drain output type (required for I2C).
 * 		4. Configure I2C1 registers for Fast Mode ~200kHz.
 *
 * @note I2C peripheral (PE bit) is disabled before configuration
 * 		 and re-enabled after, as required by the reference manual.
 *
 */
void I2C1_Init(void)
{
	/***
	 * Step 1: Enable peripheral clocks.
	 * GPIOB clock on AHB1 bus.
	 * I2C1  clock on APB1 bus.
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	/***
	 * Step 2: Configure PB8 and PB9 as Alternate Function mode (MODER = 10).
	 * Clear bits first, then set to AF mode.
	 *
	 */
	GPIOB->MODER &= ~(3U << (2 * 8));	/* Clear PB8 mode bits */
	GPIOB->MODER &= ~(3U << (2 * 9));	/* Clear PB9 mode bits */
	GPIOB->MODER |= (2U << (2 * 8));	/* PB8 -> AF mode */
	GPIOB->MODER |= (2U << (2 * 9));	/* PB9 -> AF mode */

	/***
	 * Step 3: Select AF4 (I2C1) in Alternate Function Register.
	 * AFR[1] controls pins 8-15.
	 * PB8 -> AFR[1] bits [3:0].
	 * PB9 -> AFR[1] bits [7:4].
	 *
	 */
	GPIOB->AFR[1] &= ~(0xFU << (4 * 0));	/* Clear AF bits for PB8. */
	GPIOB->AFR[1] &= ~(0xFU << (4 * 1));	/* Clear AF bits for PB9. */
	GPIOB->AFR[1] |= (4U << (4 * 0));		/* PB8 -> AF4 (I2C1_SCL). */
	GPIOB->AFR[1] |= (4U << (4 * 1));		/* PB9 -> AF4 (I2C1_SDA). */

	/***
	 * Step 4: Set Open-Drain output type for PB8 and PB9.
	 * I2C protocol requires open-drain lines with external pull-up resistors.
	 *
	 */
	GPIOB->OTYPER |= (1U << (8));	/* PB8 -> Open-Drain. */
	GPIOB->OTYPER |= (1U << (9));	/* PB9 -> Open-Drain. */

	/***
	 * Step 5: Configure I2C1 registers.
	 * PE (bit 0) must be 0 during configuration.
	 *
	 */
	I2C1->CR1 &= ~(1U << (0));		/* Disable I2C1 (PE = 0). */

	I2C1->CR2 &= ~((1U << 6) - 1);	/* Clear FREQ bits [5:0] */
	I2C1->CR2 |= 16U;				/* FREQ = 16 -> APB1 = 16MHz */

	/***
	 * CCR = 26 for Fast Mode ~200kHz with Duty cycle 2:1 (Bit15=1, Bit14=0).
	 * Formula: CCR = Fpclk1 / (3 * Fscl)
	 * 			CCR = 16MHz  / (3 * 205kHz) = 26
	 *
	 * Note: For 400kHz use CCR = 13
	 * 		 CCR = 16MHz / (3 * 400kHz) = 13
	 *
	 */
	I2C1->CCR = 26U;
	I2C1->CCR |= (1U << 15);		/* Fast mode (Bit15 = 1) */
	/***
	 * TRISE = floor(Trise_max / Tpclk1) + 1
	 * Fast mode Trise_max = 300ns
	 * Tpclk1 = 1/16MHz = 62.5ns
	 * TRISE = floor(300ns / 62.5ns) + 1 = 4 + 1 = 5
	 *
	 */
	I2C1->TRISE = 5U;

	I2C1->CR1 |= (1U << (0));		/* Enable I2C1 (PE = 1) */
}
