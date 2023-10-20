#include <Arduino.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include "pico-badge.h"

// This is the BADGE namespace that mainly offers services to the script

namespace BADGE
{

	// all of these variables are private to the namespace
	namespace
	{
		int dscale = 1; // delay scaling
		// variables for pausing and looping
		int pauseval = -1;
		int loopstartval = -1;
		int loopmaxval = -1;
	}

	int pause(void)
	{
		return pauseval;
	}							  // read pause
	void pause(int tag, int type) // set pause (type=0 is tag, non-zero is frame)
	{
		pauseval = type ? tag : (findTag(tag) + 1);
	}
	void unpause(void) // stop pause
	{
		pauseval = -1;
	}
	void unloop(void) // stop loop
	{
		loopstartval = loopmaxval = -1;
	}
	void pausehere(int n) // pause on frame number
	{
		pauseval = n;
	};
	// user probably doesn't need these
	int loopstart(void) { return loopstartval; }
	void loopstart(int tag, int type) { loopstartval = type ? tag : (findTag(tag) + 1); }
	int loopmax(void) { return loopmaxval; }
	void loopmax(int tag, int type) { loopmaxval = type ? tag : findTag(tag); }

	// Delay for n milliseconds subject to scale
	// stop if a button gets pushed
	void scaledelay(unsigned n)
	{
		int i;
		for (i = 0; i < n * dscale / 10; i++)
		{
			if (i % 10 && btn_pending())
			{
				break; // pending buttons interrupt delay, check every 10 cycles (100ms)
			}
			delay(10);
		}
	}

	// Set an offset to the delay scale (min 1, max 10)
	void delayscaler(int offset)
	{
		if (dscale + offset < 1)
			dscale = 1;
		else if (dscale + offset > 10)
			dscale = 10;
		else
			dscale += offset;
	}

	// Give access to global getbtns...
	uint16_t getbtns(void) { return ::getbtns(); }

	// Set a loop between two tags (types=0; default) or frames (types=1)
	// If pause is 1, then loop will stop at the end
	// if pause is 0 (default) the loop will recycle each time
	void setloop(int tag0, int tag1, int pause, int type0, int type1)
	{
		loopstartval = type0 ? tag0 : (findTag(tag0) + 1);
		loopmaxval = type1 ? tag1 : (findTag(tag1) + 1);
		if (pause)
			pauseval = loopstartval;
		else
			pauseval = -1;
	}

	// Convert a tag to a frame number
	int findTag(int tag)
	{
		for (int i = 0; i < script_size; i++)
		{
			if (script[i].type == TAG && script[i].arg == tag)
				return i;
		}
		return -1;
	}

	// Turn display on or off
	void on(void) { gfx->displayOn(); }
	void off(void) { gfx->displayOff(); }
};