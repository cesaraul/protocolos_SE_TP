/* -------------------------------------------------------------------------

---------------------------------------------------------------------------*/
#include "sapi.h"
#include "lcd_ascii.h"
/* macros */

#define  LCD_SCREEN_X_MAX  84
#define  LCD_SCREEN_Y_MAX  48
 
#define  LCD_PIN_RES    GPIO2 /* external reset input: warning is active in low mode */
#define  LCD_PIN_SCE    GPIO1 /* chip enable: warning is active in low mode */
#define  LCD_PIN_DC     GPIO4 /* data/clock mode select */
#define  LCD_PIN_SDIN   GPIO3 /* serial data input */
#define  LCD_PIN_SCLK   GPIO6/* serial clock input */
 


 #define  FUNCTION_SET             0x00000020 /* set the bit 5 to begins table 1 */
 #define  EXTENDED_FUNCTION_SET    FUNCTION_SET | 0x00000001 /* for set the extendend set of instructions */
 #define  BASIC_FUNCTION_SET       FUNCTION_SET /* set H to low */
 #define  DISPLAY_CONTROL_SET      0x00000008
 #define  SET_Y                    0x00000040
 #define  SET_X                    0x00000080
 #define  NORMAL_MODE_SET          DISPLAY_CONTROL_SET | 0x00000004 
 #define  BIAS_SYSTEM_ENABLED_SET  0x00000010 /* set the bias of voltages bit ready for initialization */
 #define  MUX_48_SET               BIAS_SYSTEM_ENABLED_SET | 0x00000002 | 0x00000001
 #define  V_OP_SET                 0x00000080, /* set bits values for initialize the contrast */

enum BYTE_ENCODING_TYPES {
   LSBFIRST,
   MSBFIRST
};


void LCD_init(void);
void LCD_send_character(char c);
void LCD_clear(void);
void LCD_write(const char* message, uint8_t position_x, uint8_t position_y);

