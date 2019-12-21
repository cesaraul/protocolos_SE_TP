/* -------------------------------------------------------------------------

---------------------------------------------------------------------------*/
#include "sapi.h"
#include "../inc/LCD.h"
//DEMO DRIVER LCD_PCD8544(NOKIA5110)
int main(void) {

   boardConfig();
   LCD_init(); 
  
   LCD_clear();
   
   while(1) {
      LCD_write("ABS_FASE:556", 2, 1);
      LCD_write("ADD_:1.5.6.8", 2, 2);


   }
}
