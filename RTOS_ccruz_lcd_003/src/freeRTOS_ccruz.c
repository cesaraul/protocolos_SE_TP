
/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"   //Motor del OS
#include "FreeRTOSConfig.h"
#include "task.h"		//Api de control de tareas y temporización
#include "semphr.h"		//Api de sincronización (sem y mutex)
#include "../inc/LCD.h"

// sAPI header
#include "sapi.h"
#define fase_max  180

/*==================[definiciones y macros]==================================*/

SemaphoreHandle_t Evento_Rx,Mutex_t_uart,error_lectura;
//portTickType TiempoPulsado;
/*==================[definiciones de datos externos]=========================*/
uint8_t dato_uart  = 0;
DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/


/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void Tarea_adc(void* taskParmPtr);
void Tarea_Rx( void* taskParmPtr);
void Tarea_Cambio_Fase(void* taskParmPtr);
void Tarea_error(void* taskParmPtr);
/*==================[funcion principal]======================================*/

int main(void)
{
   uint8_t Error_state = 0;

	// ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();
   adcConfig( ADC_ENABLE ); /* ADC */
   uartConfig( UART_USB, 115200 );
   LCD_init(); 
   LCD_clear();
   LCD_write("ABS_FASE:", 2, 1);

	//_-----------------------------------------------------------------------//
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "Antirrebote con freeRTOS y sAPI." );
	//_-----------------------------------------------------------------------//

    if (NULL == (Evento_Rx = xSemaphoreCreateBinary())){
	   Error_state = 1;
   }

   if (NULL == (Mutex_t_uart = xSemaphoreCreateMutex())){
   	   	   Error_state =1;
      }
   if (NULL == (error_lectura = xSemaphoreCreateBinary())){
   	   	   Error_state =1;
      }
	//_-----------------------------------------------------------------------//
    xTaskCreate(
	Tarea_adc,                     // Funcion de la tarea a ejecutar
	 (const char *)"Tarea_adc",   // Nombre de la tarea como String amigable para el usuario
	 configMINIMAL_STACK_SIZE*2,  // 
	 0,                           // Parametros de tarea
	 tskIDLE_PRIORITY+2,         
	 0                            // Puntero a la tarea creada en el sistema
     );
   xTaskCreate(
     Tarea_Rx,                    // Funcion de la tarea a ejecutar
   	 (const char *)"Tarea_Rx",    // Nombre de la tarea como String amigable para el usuario
   	 configMINIMAL_STACK_SIZE*2,  // 
   	 0,                           // Parametros de tarea
   	 tskIDLE_PRIORITY+1,          //
   	 0                            // Puntero a la tarea creada en el sistema
     );

   xTaskCreate(
	 Tarea_Cambio_Fase,
	 (const char *)"Tarea_Cambio_Fase",
	 configMINIMAL_STACK_SIZE*2,
	 0,
	 tskIDLE_PRIORITY+2,
	 0
     );

	 xTaskCreate(
     Tarea_error,                    // Funcion de la tarea a ejecutar
   	 (const char *)"Tarea_alerta_error",    // Nombre de la tarea como String amigable para el usuario
   	 configMINIMAL_STACK_SIZE*2,  // 
   	 0,                           // Parametros de tarea
   	 tskIDLE_PRIORITY+2,          //
   	 0                            // Puntero a la tarea creada en el sistema
     );
	//_-----------------------------------------------------------------------//

   // Inicio scheduler
   if (0 == Error_state){
  	  vTaskStartScheduler();
   } else{
	  printf("Error al iniciar el sistema !!!!!!!!!!!!!!");
   }

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      // Si cae en este while 1 significa que no pudo iniciar el scheduler
   }

   return 0;
}

/*==================[definiciones de funciones internas]=====================*/
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}
 //-----Funcion de la tarea Evento_adc
void Tarea_Rx(void* taskParmPtr)
{
	portTickType xPeriodicity = 20/portTICK_RATE_MS; // confirmar tiempo
	portTickType xLastWakeTime = xTaskGetTickCount();
	bool_t available;

	while(TRUE)
	{
			xSemaphoreTake(Mutex_t_uart,portMAX_DELAY);

			available=uartReadByte( UART_USB, &dato_uart ); // lectura de uart , dato uart variable global
			
			xSemaphoreGive(Mutex_t_uart);

			if( available )
			  {
		         xSemaphoreGive(Evento_Rx);
		      }
	
	}

	vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	
	}

void Tarea_Cambio_Fase(void* taskParmPtr)
{
	uint8_t Fase;

	while (TRUE){
		xSemaphoreTake(Evento_Rx,portMAX_DELAY); // semaforo binario

			xSemaphoreTake(Mutex_t_uart,portMAX_DELAY);// mutex para coger el valor de dato_uart
												   //
			Fase=dato_uart;

			xSemaphoreGive(Mutex_t_uart);

		switch (Fase)
		{
			case 'A':
				gpioWrite(LED1,ON);
				gpioWrite(LED2,OFF);
				gpioWrite(LED3,OFF);
			break;
			case 'B':
				gpioWrite(LED1,OFF);
				gpioWrite(LED2,ON);
				gpioWrite(LED3,OFF);
			break;
			case 'C':
				gpioWrite(LED1,ON);
				gpioWrite(LED2,ON);
				gpioWrite(LED3,OFF);
			break;
			case 'D':
				gpioWrite(LED1,OFF);
				gpioWrite(LED2,OFF);
				gpioWrite(LED3,ON);
			break;
		
			default:
				gpioWrite(LED1,OFF);
				gpioWrite(LED2,OFF);
				gpioWrite(LED3,OFF);

			break;
		}
			
	
	}
}

void Tarea_adc(void* taskParmPtr)
{
	portTickType xPeriodicity = 50 /portTICK_RATE_MS; // confirmar tiempo
	portTickType xLastWakeTime = xTaskGetTickCount();

	   /* Buffer */
	   static char uartBuff[10];
	   /* Variable para almacenar el valor leido del ADC CH1 */
	   uint16_t muestra_Fase = 0;

	   /* ------------- REPETIR POR SIEMPRE ------------- */
	   while(TRUE) {

	         /* Leo la Entrada Analogica AI0 - ADC0 CH1 */
	         muestra_Fase = adcRead( CH1 );
	         /* Envío la primer parte del mnesaje a la Uart */

	         /* Conversión de muestra_Fase entera a ascii con base decimal */
	         itoa( muestra_Fase, uartBuff, 10 ); /* 10 significa decimal */

	         /* Enviar muestra_Fase y Enter */
	         uartWriteString( UART_USB, "Fase_ADC_CH1: " );
	         uartWriteString( UART_USB, uartBuff );
	         uartWriteString( UART_USB, ";\r\n" );
			 //=====Muestra de datos en LCD
			 LCD_write("ABS_FASE:", 2, 1);
             LCD_write(uartBuff, 2, 2);

			 if(muestra_Fase>fase_max)
			 {
				xSemaphoreGive(error_lectura);
			 }

 
	        
	   }
	    vTaskDelayUntil( &xLastWakeTime, xPeriodicity );

}

void Tarea_error(void* taskParmPtr)
{
while (TRUE){
		xSemaphoreTake(error_lectura,portMAX_DELAY); 

		LCD_write("Error alto", 2, 1);
	}
}

// Implementacion de funcion de la tarea Tecla


/*==================[fin del archivo]========================================*/
