/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    		Communication to Supervisor uC
    @author		KSTR
*******************************************************************************/
#ifndef _NL_SUP_H_
#define _NL_SUP_H_
#include "cmsis/LPC43xx.h"

#define SUP_PROCCESS_TIMESLICE	(10)	// # of milliseconds between calls to handler

void SUP_Init(void);
void SUP_Process(void);
void SUP_MidiTrafficDetected(void);

#endif
