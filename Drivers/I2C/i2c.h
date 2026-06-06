/***
 * @file    i2c.h
 * @author  Liron Shteinfer
 * brief   Header file for I2C1 peripheral initialization on STM32F446RE
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

/***
 * @brief  Initializes I2C1 peripheral in Fast Mode at ~200kHz.
 *
 * @details Configures PB8 (SCL) and PB9 (SDA) as AF4,
 * 			Open-Drain. APB1 clock assumed to be 16MHz.
 *
 */
void I2C1_Init(void);

#endif /* I2C_H_ */
