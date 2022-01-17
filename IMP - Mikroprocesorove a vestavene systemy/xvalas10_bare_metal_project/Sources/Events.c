/* ###################################################################
**     Filename    : Events.c
**     Project     : xvalas10_bare_metal_project
**     Processor   : MK60DN512VLQ10
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2021-12-16, 20:23, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
// enumeracia moznych stavov lediek D12|D11|D10|D9
enum LEDSTATE {
	OFF = 0,
	RIGHT_1 = 1,
	RIGHT_2 = 3,
	RIGHT_3 = 7,
	LEFT_1 = 8,
	LEFT_2 = 12,
	LEFT_3 = 14,
	ALL_ON = 15
};

// ziskanie hodnoty lediek <0-15> -> <0000, 1111>
int get_state(){
	int val = 0;
	if(d12_Get())
		val = val + 8;
	if(d11_Get())
		val = val + 4;
	if(d10_Get())
		val = val + 2;
	if(d9_Get())
		val++;
	return val;
}

// spracovanie poziadavky na zmenu stavu jednej z LEDiek
void process_request(int state){
	switch(state){
	case OFF:
		d12_On();
		break;
	case LEFT_1:
		d11_On();
		break;
	case LEFT_2:
		d10_On();
		break;
	case LEFT_3:
		d9_On();
		break;
	case ALL_ON:
		d12_Off();
		break;
	case RIGHT_3:
		d11_Off();
		break;
	case RIGHT_2:
		d10_Off();
		break;
	case RIGHT_1:
		d9_Off();
		break;
	default:
		break;
	}
}


/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MK60DN512LQ10]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  button_OnPortEvent (module Events)
**
**     Component   :  button [GPIO_LDD]
*/
/*!
**     @brief
**         Called if defined event on any pin of the port occured.
**         OnPortEvent event and GPIO interrupt must be enabled. See
**         SetEventMask() and GetEventMask() methods. This event is
**         enabled if [Interrupt service/event] is Enabled and disabled
**         if [Interrupt service/event] is Disabled.
**     @param
**         UserDataPtr     - Pointer to RTOS device
**                           data structure pointer.
*/
/* ===================================================================*/
void button_OnPortEvent(LDD_TUserData *UserDataPtr)
{
	int state = get_state();
	process_request(state);
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
