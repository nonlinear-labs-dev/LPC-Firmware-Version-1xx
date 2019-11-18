/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    	Communication to Supervisor uC, for audio muting
    @author		KSTR

    Incoming Midi events over USB are intepreted as "audio engine alive".
    Any such event triggers a monoflop with 120ms timeout. To keep the
    monoflop high the incoming events should occur at least every 100ms.

    Whenever this monoflop changes state, the new state is signalled to
    the Supervisor.
    This signal is a 100ms square wave with varying duty cycle.
    - 30ms high, 70ms low : LOW  (MUTE request)
    - 70ms high, 30ms low : HIGH (UNMUTE request)

*******************************************************************************/


#include "sup/nl_sup.h"
#include "drv/nl_dbg.h"
#include "drv/nl_pin.h"
#include "drv/nl_gpio.h"





volatile uint16_t		midi_timeout=0;
uint8_t					audio_engine_online = 0;
uint8_t					override = 0;
uint8_t					override_state = 0;

uint8_t					unmute_state = 0;		// ==0 means muted
uint8_t					old_unmute_state = 0xAA;

uint8_t					step = 0;


#define					PERIOD		(10)
#define					MUTED		(3)
#define					UNMUTED		(7)

uint8_t					transition = MUTED;
uint8_t					new_transition = MUTED;



static SUP_PINS_T* pins;

void SUP_Config(SUP_PINS_T *sup_pins)
{
	pins = sup_pins;
}



void Set_Signalling_GPIO_Pin(uint8_t new_state)
{
	NL_GPIO_SetState(pins->lpc_sup_mute, new_state);
}


void SUP_Init(void)
{
	Set_Signalling_GPIO_Pin(0);

	midi_timeout = 0;
	audio_engine_online = 0;

	override = 0;
	override_state = 0;

	unmute_state = 0;
	old_unmute_state = 0xAA;

	step = 0;

	transition = MUTED;
	new_transition = MUTED;
}



void SUP_MidiTrafficDetected(void)
{
	// set # of timeslices without traffic to raise "audio engine offline"
	midi_timeout = 1 + (TRAFFIC_TIMEOUT / SUP_PROCCESS_TIMESLICE);
}



void SUP_Enable_Override_Muting(uint8_t on_off)
{
	override = ( on_off != 0 );
}

void SUP_Override_Muting(uint8_t new_unmute_state)	// effective only when enabled
{
	override_state = ( new_unmute_state != 0 );
}



void SUP_Process(void)
{
	if (midi_timeout)
		midi_timeout--;

	if (NL_GPIO_Get(pins->lpc_unmute_jumper) == 0)	// hardware jumper reads low (jumper set) ?
		unmute_state = 1;							// 	-> force unmute
	else if (override)								// software override ?
		unmute_state = override_state;				//  -> use override state
	else											// normal operation ?
		unmute_state = (midi_timeout != 0);			//  -> unmute if midi traffic did not time out

	if (unmute_state != old_unmute_state)
	{
		old_unmute_state = unmute_state;
		if (unmute_state)
		{
			// DBG_Led_Audio_On();
			new_transition = UNMUTED;
		}
		else
		{
			// DBG_Led_Audio_Off();
			new_transition = MUTED;
		}
	}

	Set_Signalling_GPIO_Pin(step < transition);
	if (++step >= PERIOD)
	{
		step = 0;
		transition=new_transition;
	}
}
