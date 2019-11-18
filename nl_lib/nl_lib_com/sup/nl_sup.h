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
	GPIO_NAME_T* lpc_sup_mute;
	GPIO_NAME_T* lpc_unmute_jumper;
} SUP_PINS_T;

void SUP_Config(SUP_PINS_T *sup_pins);
void SUP_Init(void);
void SUP_Process(void);
void SUP_MidiTrafficDetected(void);

void SUP_Enable_Override_Muting(uint8_t on_off);
void SUP_Override_Muting(uint8_t new_unmute_state);	// ==0 means muted. effective only when enabled

#endif
