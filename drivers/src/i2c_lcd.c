#include "i2c_lcd.h"

static void delay_ms(uint32_t ms)
{
    for(volatile uint32_t i = 0; i < ms * 4000; i++)
    {
        __asm volatile("nop");
    }
}

static void LCD_Send_Cmd_Half(I2C_Handle_t *pI2CHandle, uint8_t cmd)
{
    uint8_t data_u = (cmd & 0xF0);
    uint8_t data_t[2];

    data_t[0] = data_u | LCD_BACKLIGHT | LCD_EN;
    data_t[1] = data_u | LCD_BACKLIGHT;

    I2C_MasterSendData(pI2CHandle, data_t, 2, LCD_ADDR, I2C_DISABLE_SR);
}

void LCD_Send_Cmd(I2C_Handle_t *pI2CHandle, uint8_t cmd)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);

    data_t[0] = data_u | LCD_BACKLIGHT | LCD_EN;
    data_t[1] = data_u | LCD_BACKLIGHT;
    data_t[2] = data_l | LCD_BACKLIGHT | LCD_EN;
    data_t[3] = data_l | LCD_BACKLIGHT;

    I2C_MasterSendData(pI2CHandle, data_t, 4, LCD_ADDR, I2C_DISABLE_SR);
}

void LCD_Send_Data(I2C_Handle_t *pI2CHandle, uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);

    data_t[0] = data_u | LCD_BACKLIGHT | LCD_EN | LCD_RS;
    data_t[1] = data_u | LCD_BACKLIGHT | LCD_RS;
    data_t[2] = data_l | LCD_BACKLIGHT | LCD_EN | LCD_RS;
    data_t[3] = data_l | LCD_BACKLIGHT | LCD_RS;

    I2C_MasterSendData(pI2CHandle, data_t, 4, LCD_ADDR, I2C_DISABLE_SR);
}

void LCD_Init(I2C_Handle_t *pI2CHandle)
{
    delay_ms(50);

    LCD_Send_Cmd_Half(pI2CHandle, 0x30);
    delay_ms(5);
    LCD_Send_Cmd_Half(pI2CHandle, 0x30);
    delay_ms(1);
    LCD_Send_Cmd_Half(pI2CHandle, 0x30);
    delay_ms(10);

    LCD_Send_Cmd_Half(pI2CHandle, 0x20);
    delay_ms(10);

    LCD_Send_Cmd(pI2CHandle, 0x28); // 4-bit mod, 2 row, 5x8 font
    delay_ms(1);

    LCD_Send_Cmd(pI2CHandle, 0x08);
    delay_ms(1);

    LCD_Send_Cmd(pI2CHandle, 0x01);
    delay_ms(3);

    LCD_Send_Cmd(pI2CHandle, 0x06);
    delay_ms(1);

    LCD_Send_Cmd(pI2CHandle, 0x0C); // Display ON, Cursor OFF, Blink OFF
    delay_ms(1);
}

void LCD_Set_Cursor(I2C_Handle_t *pI2CHandle, uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_Send_Cmd(pI2CHandle, addr);
}

void LCD_Send_String(I2C_Handle_t *pI2CHandle, char *str)
{
    while (*str)
    {
        LCD_Send_Data(pI2CHandle, (uint8_t)(*str));
        str++;
    }
}
