/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    		Communication to Supervisor uC
    @author		KSTR
*******************************************************************************/

// 10ms

#include "sup/nl_sup.h"
#include "drv/nl_dbg.h"
#include "drv/nl_pin.h"
#include "drv/nl_gpio.h"

// time in milliseconds without incoming midi traffic to raise "audio engine offline"
#define TRAFFIC_TIMEOUT		(120)




static PIN_CFG_T lpc_sup_mute = {
	.pinId  		= {5,6},
	.ioType 		= PIN_TYPE_GPIO,
	.gpioId 		= {2,15},
	.direction		= PIN_GPIO_DIR_OUT,
	.inputBuffer	= PIN_INBUF_OFF,
	.glitchFilter 	= PIN_FILTER_ON,
	.slewRate 		= PIN_SRATE_SLOW,
	.pullDown 		= PIN_PDN_OFF,
	.pullUp 		= PIN_PUP_ON,
	.function		= 0
};











volatile uint16_t		midi_timeout=0;
uint8_t					audio_engine_online = 0;


void SUP_Init(void)
{
	PIN_Config(&lpc_sup_mute);
	NL_GPIO_SetState(&lpc_sup_mute.gpioId, 1);

	midi_timeout = 0;
	audio_engine_online = 0;
	//DBG_Led_Audio_Engine_Off();
}



void SUP_MidiTrafficDetected(void)
{
	// set # of timeslices without traffic to raise "audio engine offline"
	midi_timeout = 1 + (TRAFFIC_TIMEOUT / SUP_PROCCESS_TIMESLICE);
}



void SUP_Process(void)
{
	if (midi_timeout)
		midi_timeout--;

	if (midi_timeout == 0)
	{
		DBG_Led_Audio_Off();
		NL_GPIO_SetState(&lpc_sup_mute.gpioId, !(audio_engine_online=0));
	}
	else
	{
		DBG_Led_Audio_On();
		NL_GPIO_SetState(&lpc_sup_mute.gpioId, !(audio_engine_online=1));
	}
}
