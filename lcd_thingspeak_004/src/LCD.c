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
   /* initialize the comunication SCE HIGH */
   gpioWrite(LCD_PIN_SCE, HIGH);
   gpioWrite(LCD_PIN_SCLK, LOW); /* make sure is LOW */
   gpioWrite(LCD_PIN_DC, LOW); /* set to LOW for send commands */
   /* NOTE(elsuizo:2018-03-30): luego de esto necesitamos enviar una trama de bits como en la figura, para ello podemos
      utilizar la funcion de Arduino shiftOut(dataPin, clockPin, bitOrder, value)
    */
   gpioWrite(LCD_PIN_SCE, LOW);
   shift_out(LCD_PIN_SDIN, LCD_PIN_SCLK, MSBFIRST, command);
   /* after send command make sure SCE is HIGH!!! */
   gpioWrite(LCD_PIN_SCE, HIGH);
}

static void send_data(uint8_t data) {
   /* initialize the comunication SCE HIGH */
   gpioWrite(LCD_PIN_SCE, HIGH);
   gpioWrite(LCD_PIN_SCLK, LOW); /* make sure is LOW */
   gpioWrite(LCD_PIN_DC, HIGH); /* set to HIGH for send data */
   /* NOTE(elsuizo:2018-03-30): luego de esto necesitamos enviar una trama de bits como en la figura, para ello podemos
      utilizar la funcion de Arduino shiftOut(dataPin, clockPin, bitOrder, value)
    */
   gpioWrite(LCD_PIN_SCE, LOW);
   shift_out(LCD_PIN_SDIN, LCD_PIN_SCLK, MSBFIRST, data);
   /* after send command make sure SCE is HIGH!!! */
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

   /* initialization */
   /* RES pulse for reset function */
   gpioWrite(LCD_PIN_RES, LOW);
   delay(1);
   gpioWrite(LCD_PIN_RES, HIGH);
   /* initialization functions */
   send_command(EXTENDED_FUNCTION_SET); /* use extended functions set */
   send_command(MUX_48_SET); /* set mux rate to 1:48 */
    /* send_command(V_OP_SET | 0x3c); #<{(| set the contrast |)}># #<{(| NOTE(elsuizo:2018-03-30): no ponerlo muy alto porque no se vera nada |)}># */
   
   send_command(BASIC_FUNCTION_SET); /* use extended functions set */
   send_command(NORMAL_MODE_SET); /* enter in NORMAL mode for send data */

}


void LCD_send_character(char c) {

   int index = 0;
   for (index = 0; index < 5; index++) {
      send_data(ASCII_8[c - 0x20][index]);
   }
   send_data(0x00);
}

void LCD_clear(void) {

   /* go to (0, 0) */
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


