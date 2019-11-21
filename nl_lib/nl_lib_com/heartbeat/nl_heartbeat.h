/******************************************************************************/
/** @file		nl_heartbeat.h
    @date		2019-11-18
    @brief    	Definitions for the HeartBeat mechanisms
    @example
    @ingroup  	nl_drv_modules
    @author		KSTR
*******************************************************************************/

#ifndef NL_HEARTBEAT_H_
#define NL_HEARTBEAT_H_

#include <stdint.h>


void HBT_MidiReceive(uint8_t* buff, uint32_t len);

#endif /* nl_heartbeat.h */
