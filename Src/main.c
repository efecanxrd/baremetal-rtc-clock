#include "stm32f407xx.h"
#include <stdio.h>
#include <string.h>
#include "ds1307.h"
#include "i2c_lcd.h"
#include <time.h>

#define SYSTICK_TIM_CLK 16000000U
volatile uint8_t update_display_flag = 0;
I2C_Handle_t I2C2Handle;

// PB6 -> I2C1_SCL, PB7 -> I2C1_SDA
// PB10 -> I2C2_SCL, P11 -> I2C2_SDA

//Initializing I2C2 for LCD Modulex
void I2C2_GPIO_Init(void)
{
    GPIO_Handle_t I2CPins;

    GPIOB_PCLK_EN();

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4; // AF4 (I2C1)
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD; // Open-Drain
    I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; // Pull-Up
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    I2CPins.GPIO_PinConfig.GPIO_PinNumber = 10;
    GPIO_Init(&I2CPins);

    I2CPins.GPIO_PinConfig.GPIO_PinNumber = 11;
    GPIO_Init(&I2CPins);
}

void I2C2_Inits(void)
{
    I2C2_PCLK_EN();

    I2C2Handle.pI2Cx = I2C2;
    I2C2Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
    I2C2Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C2Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C2Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

    I2C_Init(&I2C2Handle);
}

//systick
void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    /* calculation of reload value */
    uint32_t count_value = ((SYSTICK_TIM_CLK/tick_hz)-1);

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}

void number_to_string(uint8_t num , char* buf)
{

	if(num < 10){
		buf[0] = '0';
		buf[1] = num+48;
	}else if(num >= 10 && num < 99)
	{
		buf[0] = (num/10) + 48;
		buf[1]= (num % 10) + 48;
	}
}

char* time_to_string(RTC_time_t *rtc_time)
{
	static char buf[9];

	buf[2]= ':';
	buf[5]= ':';

	number_to_string(rtc_time->hours,buf);
	number_to_string(rtc_time->minutes,&buf[3]);
	number_to_string(rtc_time->seconds,&buf[6]);

	buf[8] = '\0';

	return buf;

}

char* date_to_string(RTC_date_t *rtc_date)
{
	static char buf[9];

	buf[2]= '/';
	buf[5]= '/';

	number_to_string(rtc_date->date,buf);
	number_to_string(rtc_date->month,&buf[3]);
	number_to_string(rtc_date->year,&buf[6]);

	buf[8]= '\0';

	return buf;

}

char* get_day_of_week(uint8_t i)
{
    static const char* const days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    if(i >= 1 && i <= 7) {
        return (char*)days[i - 1];
    }
    return "---";
}

int main(void) {
    RTC_time_t current_time;
    RTC_date_t current_date;

    // Initialization
    I2C2_GPIO_Init();
    I2C2_Inits();

    I2C_PeripheralControl(I2C2, ENABLE);
    I2C_ManageAcking(I2C2, I2C_ACK_ENABLE);

    // Peripherals
    LCD_Init(&I2C2Handle);

    if(ds1307_init()) {
        printf("RTC initialization has failed\n");
        while(1);
    }

    //Setting the time in the RTC Module with time.h lib. Can be enabled/disabled in case of need
    char month_str[4];
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    struct tm t = {0};
    int full_year;

    //("Mmm dd yyyy") ("hh:mm:ss")
    sscanf(__DATE__, "%3s %d %d", month_str, &t.tm_mday, &full_year);
    sscanf(__TIME__, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);

    t.tm_mon  = (strstr(months, month_str) - months) / 3;
    t.tm_year = full_year - 1900;

    // day of week
    mktime(&t);

    current_date.day   = t.tm_wday + 1;
    current_date.date  = (uint8_t)t.tm_mday;
    current_date.month = (uint8_t)(t.tm_mon + 1);
    current_date.year  = (uint8_t)(full_year % 100);

    // Time format conversion
    if (t.tm_hour >= 12) {
        current_time.time_format = TIME_FORMAT_12HRS_PM;
        current_time.hours = (t.tm_hour > 12) ? (t.tm_hour - 12) : 12;
    } else {
        current_time.time_format = TIME_FORMAT_12HRS_AM;
        current_time.hours = (t.tm_hour == 0) ? 12 : t.tm_hour;
    }
    current_time.minutes = (uint8_t)t.tm_min;
    current_time.seconds = (uint8_t)t.tm_sec;

    // These two lines can be disabled if not needed
    ds1307_set_current_date(&current_date);
    ds1307_set_current_time(&current_time);

    ds1307_get_current_date(&current_date);
    ds1307_get_current_time(&current_time);

    init_systick_timer(1);

    while(1) {
    	 if(update_display_flag) {
    	  update_display_flag = 0;
    	  RTC_time_t current_time;
    	  RTC_date_t current_date;

    	  	ds1307_get_current_time(&current_time);

    	  	char *am_pm = "";
    	  	if(current_time.time_format != TIME_FORMAT_24HRS) {
    	  	    am_pm = (current_time.time_format == TIME_FORMAT_12HRS_PM) ? "PM" : "AM";
    	  	}

    	  	  ds1307_get_current_date(&current_date);

    	  	  LCD_Set_Cursor(&I2C2Handle, 0, 0); // 1st row, 1st column
    	  	  LCD_Send_String(&I2C2Handle, date_to_string(&current_date));
    	  	  LCD_Send_String(&I2C2Handle, " ");
    	  	  LCD_Send_String(&I2C2Handle, get_day_of_week(current_date.day));

    	  	  LCD_Set_Cursor(&I2C2Handle, 1, 0); // 2nd row, 1st column
    	  	  LCD_Send_String(&I2C2Handle, time_to_string(&current_time));
    	  	  LCD_Send_String(&I2C2Handle, " ");
    	  	  LCD_Send_String(&I2C2Handle, am_pm);
    	 }
    }
}

void SysTick_Handler(void) {
	update_display_flag = 1;
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
    // I2C Event / Error Callback
}
