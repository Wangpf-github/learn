#ifndef __OLED_H
#define __OLED_H

/******************************************************************************
* C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* EXPORTED MACROS AND DEFINITIONS
******************************************************************************/ 
#include <stdlib.h>

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/
void oled_Clear(void) ;
void oled_Init(void);
void oled_UpScreenOn(void);

void oled_PrintfString(uint8_t x,uint8_t y,u8 *str);
void oled_PrintfChar(uint8_t x,uint8_t y,uint8_t val);
void oled_SetPostion( uint8_t x, uint8_t y) ;
void oled_SetCharSize( uint8_t val );

/******************************************************************************
* END OF C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H *///[/mw_shl_code]
