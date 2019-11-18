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


static uint64_t audioEngineHeartBeat = 0;
static uint64_t lpcHeartBeat = 0;


/******************************************************************************/
/** @brief		process incoming Midi data (Callback routine for "USB_MIDI_Config()")
    @param[in]	buff	Pointer to data buffer
    @param[in]	len		Amount of bytes in buffer
*******************************************************************************/
void HBT_MidiReceive(uint8_t *buff, uint32_t len)
{
  SUP_MidiTrafficDetected();

  buff++;	// first midi byte is in buff[1]
  if (len)
	  len--;
  while(len >= 3)
  {
	if ( (buff[0] & 0xFC) == 0xA0)		// command byte is 0xA0 ... 0xA3 ?
	{
		unsigned long shift = (buff[0] & 0x03) * 14;

		uint64_t lsb = *(buff + 1);
		uint64_t msb = *(buff + 2);

		audioEngineHeartBeat = (buff[0] == 0xA0) ? 0 : audioEngineHeartBeat;
		audioEngineHeartBeat |= (msb << shift);
		audioEngineHeartBeat |= (lsb << (shift + 7));


		if(buff[0] == 0xA2)
		{
		  uint64_t chainHeartbeat = audioEngineHeartBeat + lpcHeartBeat;
		  BB_MSG_WriteMessage(BB_MSG_TYPE_HEARTBEAT, 4, (uint16_t *) &chainHeartbeat);
		  audioEngineHeartBeat++;
		}
	}
    len -= 3;
  }
}


void HBT_Process(void)
{
	lpcHeartBeat++;
}

// EOF

