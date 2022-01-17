/* ###################################################################
**     Filename    : main.c
**     Project     : xvalas10_freertos
**     Processor   : MK60DN512VLQ10
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2021-12-17, 12:09, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "MCUC1.h"
#include "UTIL1.h"
#include "button.h"
#include "d9.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "d10.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "d11.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "d12.h"
#include "LEDpin4.h"
#include "BitIoLdd4.h"
#include "FRTOS1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"

// konstanty pre vTaskDelay()
const TickType_t BUTTON_DELAY = 300 / portTICK_PERIOD_MS;
const TickType_t LED_DELAY = 250 / portTICK_PERIOD_MS;

// mozne a podporovane stavy lediek
enum LEDSTATE {
	OFF,
	RIGHT_1,
	RIGHT_2,
	RIGHT_3,
	LEFT_1,
	LEFT_2,
	LEFT_3,
	ALL_ON
};

// pociatocny stav lediek
int state = OFF;
QueueHandle_t q;

// Uloha spravujuca LEDKY - vytiahne prvok z fronty a zmeni jeho stav
void led_handler_task(void* Parameters) {
	int led_id = 9;
	for(;;) {
		// fronta nie je prazdna, ziskame id ledky ktorej hodnotu znegujeme
		if(xQueueReceive(q, &led_id, 0) == pdPASS) {
			switch(led_id) {
				case 9:
					d9_Neg();
					break;
				case 10:
					d10_Neg();
					break;
				case 11:
					d11_Neg();
					break;
				case 12:
					d12_Neg();
					break;
				default:
					break;
			}
			vTaskDelay(LED_DELAY);
		}
	}
}

// Uloha spravujuca tlacitko SW6 - po stlaceni prida ziadost o zmenu stavu ledky do fronty
void button_handler_task(void* Parameters){
	int led_id = 9;
	int next_state = OFF;
	for(;;) {
		// tlacitko sw6 stlacene
		if(!(button_GetFieldValue(button_DeviceData, b4_sw6))) {
			switch(state){
				case OFF:
					led_id = 12;
					next_state = LEFT_1;
					break;
				case LEFT_1:
					led_id = 11;
					next_state = LEFT_2;
					break;
				case LEFT_2:
					led_id = 10;
					next_state = LEFT_3;
					break;
				case LEFT_3:
					led_id = 9;
					next_state = ALL_ON;
					break;
				case ALL_ON:
					led_id = 12;
					next_state = RIGHT_3;
					break;
				case RIGHT_1:
					led_id = 9;
					next_state = OFF;
					break;
				case RIGHT_2:
					led_id = 10;
					next_state = RIGHT_1;
					break;
				case RIGHT_3:
					led_id = 11;
					next_state = RIGHT_2;
					break;
				default:
					break;
			}
			// odoslanie poziadavky na zmenu stavu ledky do fronty
			if(FRTOS1_xQueueSendToBack(q, &led_id, 0) == pdPASS){
				// zmena stavu
				state = next_state;
				vTaskDelay(BUTTON_DELAY);
			}
		}
	}
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  /* Write your code here */
  // vytvorenie fronty
  q = FRTOS1_xQueueCreate(5, sizeof(int));
  if(q != NULL) {
	  // vytvorenie uloh pre spravu tlacitka a lediek
	  FRTOS1_xTaskCreate(button_handler_task, "button_handler_task", 64, NULL, 1, NULL);
	  FRTOS1_xTaskCreate(led_handler_task, "led_handler_task", 64, NULL, 1, NULL);
	  FRTOS1_vTaskStartScheduler();
  }

  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
