#include "../inc/LCD.h"


static void shift_out(gpioMap_t data_pin, gpioMap_t clock_pin, enum BYTE_ENCODING_TYPES bit_order, uint8_t value) {
   uint32_t bit;

   for (bit = 0; bit < 8; bit++)  {
      if (bit_order == LSBFIRST) {
         gpioWrite(data_pin, !!(value & (1 << bit)));
      }
      else {
         gpioWrite(data_pin, !!(value & (1 << (7 - bit))));
      }

      gpioWrite(clock_pin, HIGH);
      gpioWrite(clock_pin, LOW);
   }
}

static void send_command(uint8_t command) {
   gpioWrite(LCD_PIN_SCE, HIGH);
   gpioWrite(LCD_PIN_SCLK, LOW);
   gpioWrite(LCD_PIN_DC, LOW);
 
   gpioWrite(LCD_PIN_SCE, LOW);
   shift_out(LCD_PIN_SDIN, LCD_PIN_SCLK, MSBFIRST, command);
   gpioWrite(LCD_PIN_SCE, HIGH);
}

static void send_data(uint8_t data) {
   gpioWrite(LCD_PIN_SCE, HIGH);
   gpioWrite(LCD_PIN_SCLK, LOW);
   gpioWrite(LCD_PIN_DC, HIGH); 
   gpioWrite(LCD_PIN_SCE, LOW);
   shift_out(LCD_PIN_SDIN, LCD_PIN_SCLK, MSBFIRST, data);
   gpioWrite(LCD_PIN_SCE, HIGH);

}


void LCD_init(void) {
 
   gpioInit(LCD_PIN_RES,  GPIO_OUTPUT);
   gpioInit(LCD_PIN_SCE,  GPIO_OUTPUT);
   gpioInit(LCD_PIN_DC,   GPIO_OUTPUT);
   gpioInit(LCD_PIN_SDIN, GPIO_OUTPUT);
   gpioInit(LCD_PIN_SCLK, GPIO_OUTPUT);

   gpioWrite(LCD_PIN_RES, HIGH);
   gpioWrite(LCD_PIN_SCE, HIGH);

   gpioWrite(LCD_PIN_RES, LOW);
   delay(1);
   gpioWrite(LCD_PIN_RES, HIGH);
   send_command(EXTENDED_FUNCTION_SET);
   send_command(MUX_48_SET);

   
   send_command(BASIC_FUNCTION_SET);
   send_command(NORMAL_MODE_SET);
}


void LCD_send_character(char c) {

   int index = 0;
   for (index = 0; index < 5; index++) {
      send_data(ASCII_8[c - 0x20][index]);
   }
   send_data(0x00);
}

void LCD_clear(void) {

   send_command(SET_Y | 0x00);
   send_command(SET_X | 0x00);

   int index;
   for (index = 0; index < LCD_SCREEN_X_MAX * LCD_SCREEN_Y_MAX; index++) {
      send_data(0x00);
   }
}

void LCD_write(const char* message, uint8_t position_x, uint8_t position_y) {
   send_command(SET_X | position_x);
   send_command(SET_Y | position_y);
   while (*message) {
      LCD_send_character(*message++);
   }
}


