/******************************************************************************/
/** @file		nl_heartbeat.c
    @date		2019-11-18
    @brief    	The HeartBeat mechanisms
    @example
    @ingroup  	nl_drv_modules
    @author		KSTR/hhoegelo
*******************************************************************************/


#include "heartbeat/nl_heartbeat.h"
#include "spibb/nl_bb_msg.h"
#include "sup/nl_sup.h"
#include "drv/nl_dbg.h"


static uint64_t audio_engine_heartbeat = 0;	// accumulator for collect proccess
static uint64_t lpc_heartbeat = 0;			// our own value
static uint64_t heartbeat = 0;				// value to transmit
static uint8_t	heartbeat_update = 0;		// flag for pending transmit
static uint8_t	step=0;						// collect process step chain
static uint8_t	traffic_update = 0;			// flag for incoming traffic

/******************************************************************************/
/** @brief		process incoming Midi data, interrupt(!!) callback for "USB_MIDI_Config()"
    @param[in]	buff	Pointer to data buffer
    @param[in]	len		Amount of bytes in buffer
*******************************************************************************/
void HBT_MidiReceive(uint8_t *buff, uint32_t len)
{
// TODO
//	- check whether the dummy byte to be skipped appears only at the buffer start or before every
//	  MIDI command
//	- allow future command extensions (refactor into new general module "midi receive),
//	  for example, audio engine reporting actual signal level sent to soundcard, for level metering etc.

	traffic_update = 1;

	buff++;	// first midi byte is in buff[1], not buff[0]
	if (len)
	  len--;

	while(len >= 3)	// repeat until no commands left
	{
		len -= 3;		// CAUTION : this assumes only 3-byte midi commands are ever received!

		switch (step)
		{
		case 0 :	// wait for lowest 14 bits
			if ( buff[0] == 0xA0 )	// marker found
			{
				audio_engine_heartbeat = ((uint64_t)buff[1] << (7*0)) | ((uint64_t)buff[2] << (7*1));
				step = 1;
			}
			break;
		case 1 :	// wait for middle 14 bits
			if ( buff[0] == 0xA1 )	// marker found
			{
				audio_engine_heartbeat |= ((uint64_t)buff[1] << (7*2)) | ((uint64_t)buff[2] << (7*3));
				step = 2;
			}
			else
				step = 0;	// reset step chain if command sequence is interrupted
			break;
		case 2 :	// wait for highest 14 bits and set new heartbeat value
			if ( (buff[0] == 0xA2) && !heartbeat_update )	// marker found and no pending transmits
			{
				audio_engine_heartbeat |= ((uint64_t)buff[1] << (7*4)) | ((uint64_t)buff[2] << (7*5));
				heartbeat = audio_engine_heartbeat + lpc_heartbeat;
				lpc_heartbeat++;
				heartbeat_update = 1;
			}
			step = 0;	// reset step chain
			break;
		} // switch
	} // while
}

// periodically called every 10ms or so, to process pending updates
void HBT_Process(void)
{
	if (traffic_update)
	{
		SUP_MidiTrafficDetected();
		traffic_update = 0;
	}

	if (heartbeat_update)
	{
		if (BB_MSG_WriteMessage(BB_MSG_TYPE_HEARTBEAT, 4, (uint16_t *) &heartbeat) != -1)
			BB_MSG_SendTheBuffer();
		heartbeat_update = 0;
	}
}

/*********
	if ( (buff[0] & 0xFC) == 0xA0)		// command byte is 0xA0 ... 0xA3 ?
	{
		unsigned long shift = (buff[0] & 0x03) * 14;

		uint64_t lsb = *(buff + 1);
		uint64_t msb = *(buff + 2);

		audio_engine_heartbeat = (buff[0] == 0xA0) ? 0 : audio_engine_heartbeat;
		audio_engine_heartbeat |= (msb << shift);
		audio_engine_heartbeat |= (lsb << (shift + 7));


		if(buff[0] == 0xA2)
		{
		  uint64_t chainHeartbeat = audio_engine_heartbeat + lpc_heartbeat;
		  if (BB_MSG_WriteMessage(BB_MSG_TYPE_HEARTBEAT, 4, (uint16_t *) &chainHeartbeat) != -1)
			  BB_MSG_SendTheBuffer();
		  lpc_heartbeat++;
		}
	}
  }
*******/

// EOF

