/* -------------------------------------------------------------------------

---------------------------------------------------------------------------*/
#include "sapi.h"
#include "lcd_ascii.h"
/* macros */

#define  LCD_SCREEN_X_MAX  84
#define  LCD_SCREEN_Y_MAX  48
 
#define  LCD_PIN_RES    GPIO2 // external reset input: warning is active in low mode 
#define  LCD_PIN_SCE    GPIO1 // chip enable 
#define  LCD_PIN_DC     GPIO4 // data/clock mode select 
#define  LCD_PIN_SDIN   GPIO3 //   data input 
#define  LCD_PIN_SCLK   GPIO6// serial clock input 
 


 #define  FUNCTION_SET             0x00000020 
 #define  EXTENDED_FUNCTION_SET    FUNCTION_SET | 0x00000001 
 #define  BASIC_FUNCTION_SET       FUNCTION_SET
 #define  DISPLAY_CONTROL_SET      0x00000008
 #define  SET_Y                    0x00000040
 #define  SET_X                    0x00000080
 #define  NORMAL_MODE_SET          DISPLAY_CONTROL_SET | 0x00000004 
 #define  BIAS_SYSTEM_ENABLED_SET  0x00000010 
 #define  MUX_48_SET               BIAS_SYSTEM_ENABLED_SET | 0x00000002 | 0x00000001
 #define  V_OP_SET                 0x00000080, 

enum BYTE_ENCODING_TYPES {
   LSBFIRST,
   MSBFIRST
};


void LCD_init(void);//INICIALIzar
void LCD_send_character(char c);//enviar cadena de caracteres
void LCD_clear(void);//limpiar pantalla
void LCD_write(const char* message, uint8_t position_x, uint8_t position_y);

