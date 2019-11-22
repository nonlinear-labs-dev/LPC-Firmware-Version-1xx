/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    		Communication to Supervisor uC
    @author		KSTR
*******************************************************************************/
#ifndef _NL_SUP_H_
#define _NL_SUP_H_
#include "cmsis/LPC43xx.h"
#include "drv/nl_pin.h"


// # of milliseconds between calls to handler. DO NOT CHANGE!
#define SUP_PROCCESS_TIMESLICE	(10)

// time in milliseconds without incoming midi traffic to raise "audio engine offline"
#define TRAFFIC_TIMEOUT		(120)

typedef struct {
	GPIO_NAME_T* lpc_sup_mute_request;
	GPIO_NAME_T* lpc_sup_mute_status;
	GPIO_NAME_T* lpc_unmute_jumper;
} SUP_PINS_T;

void SUP_Config(SUP_PINS_T *sup_pins);
void SUP_Init(void);
void SUP_Process(void);
void SUP_MidiTrafficDetected(void);

void SUP_Enable_Override_Muting(uint8_t on_off);
void SUP_Override_Muting(uint8_t new_unmute_state);	// ==0 means muted. effective only when enabled

extern uint16_t	SUP_unmute_status;
// bit masks
#define	SUP_UNMUTE_STATUS_IS_VALID				(0b1000000000000000)	// status has actually been set
#define	SUP_UNMUTE_STATUS_JUMPER_OVERRIDE		(0b0000000010000000)	// hardware jumper overriding everything ...
#define	SUP_UNMUTE_STATUS_JUMPER_VALUE			(0b0000000001000000)	// ... with this value
#define	SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE		(0b0000000000100000)	// software command overriding midi-derived status ...
#define	SUP_UNMUTE_STATUS_SOFTWARE_VALUE		(0b0000000000010000)	// ... with this value
#define	SUP_UNMUTE_STATUS_MIDI_DERIVED			(0b0000000000001000)	// midi-derived status ...
#define	SUP_UNMUTE_STATUS_MIDI_DERIVED_VALUE	(0b0000000000000100)	// ... with this value
#define	SUP_UNMUTE_STATUS_HARDWARE_IS_VALID		(0b0000000000000010)	// hardware status is valid (signal from SUP uC was detected) ...
#define	SUP_UNMUTE_STATUS_HARDWARE_VALUE		(0b0000000000000001)	// ... with this value


#endif
