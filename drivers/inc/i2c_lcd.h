#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include "stm32f407xx.h"
#include "stm32f407xx_i2c_driver.h"

// Addresses
#define LCD_ADDR          0x27
#define LCD_BACKLIGHT     0x08
#define LCD_EN            0x04
#define LCD_RS            0x01

// Function prototypes
void LCD_Send_Cmd(I2C_Handle_t *pI2CHandle, uint8_t cmd);
void LCD_Send_Data(I2C_Handle_t *pI2CHandle, uint8_t data);
void LCD_Init(I2C_Handle_t *pI2CHandle);
void LCD_Set_Cursor(I2C_Handle_t *pI2CHandle, uint8_t row, uint8_t col);
void LCD_Send_String(I2C_Handle_t *pI2CHandle, char *str);

#endif /* INC_LCD_I2C_H_ */
