#ifndef _lcd_mode_
#define _lcd_mode_

//if using 8bit mode,set the below macro definition to 1
//if using 16bit mode,set the below macro definition to 0
#define CONFIG_USE_8BIT_BUS 0

//if using 8bit mode on Mega2560 and the data pin is from 22 to 29,please uncomment the below macro definition and set it to 1
//if using 8bit mode on Mega2560 and the data pin is from 30 to 37,please uncomment the below macro definition and set it to 0
//if using 8bit mode on UNO or Mega2560 and the data pin is from 2 to 9,please comment the below macro definition
//#define USE_8BIT_SHIELD_ON_MEGA 0

#endif // _mcu_pin_magic_