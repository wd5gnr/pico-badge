#include <Arduino.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include "pico-badge.h"

// This is the BADGE class that mainly offers services to the script

int BADGE::dscale = 1;  // delay scaling

// Delay for n milliseconds subject to scale
// stop if a button gets pushed
void BADGE::scaledelay(unsigned n)
{
	int i;
	for (i = 0; i < n*dscale/10; i++)
	{
		if (i%10 && btn_pending())  
		{
			break;  // pending buttons interrupt delay, check every 10 cycles (100ms)
		}
		delay(10);
	}
}

// Set an offset to the delay scale (min 1, max 10)
void BADGE::delayscaler(int offset)
{
	if (dscale+offset<1)
		dscale = 1;
	else if (dscale+offset>10)
		dscale = 10;
	else
		dscale += offset;
}

// variables for pausing and looping
int BADGE::pauseval=-1;
int BADGE::loopstartval = -1;
int BADGE::loopmaxval = -1;


// Give access to global getbtns...
uint16_t BADGE::getbtns(void) { return ::getbtns();  }

// Set a loop between two tags (types=0; default) or frames (types=1)
// If pause is 1, then loop will stop at the end
// if pause is 0 (default) the loop will recycle each time
void BADGE::setloop(int tag0, int tag1, int pause, int type0, int type1)
{
	loopstartval = type0 ? tag0 : (findTag(tag0) + 1);
	loopmaxval=type1?tag1:(findTag(tag1)+1);
	if (pause)
		pauseval = loopstartval;
	else
		pauseval = -1;
}

// Convert a tag to a frame number
int BADGE::findTag(int tag)
{
	for (int i = 0; i < script_size;i++)
	{
		if (script[i].type==TAG && script[i].arg==tag)
			return i;
	}
	return -1;
}

// Turn display on or off
void BADGE::on(void)  { gfx->displayOn();  }
void BADGE::off(void) { gfx->displayOff();  }
