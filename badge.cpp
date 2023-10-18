#include <Arduino.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include "pico-badge.h"

int BADGE::dscale = 1;

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


void BADGE::delayscaler(int offset)
{
	if (dscale+offset<1)
		dscale = 1;
	else if (dscale+offset>10)
		dscale = 10;
	else
		dscale += offset;
}

int BADGE::pauseval=-1;
int BADGE::loopstartval = -1;
int BADGE::loopmaxval = -1;


uint16_t BADGE::getbtns(void) { return ::getbtns();  }

void BADGE::setloop(int tag0, int tag1, int pause, int type0, int type1)
{
	loopstartval = type0 ? tag0 : (findTag(tag0) + 1);
	loopmaxval=type1?tag1:(findTag(tag1)+1);
	if (pause)
		pauseval = loopstartval;
	else
		pauseval = -1;
}

int BADGE::findTag(int tag)
{
	for (int i = 0; i < script_size;i++)
	{
		if (script[i].type==TAG && script[i].arg==tag)
			return i;
	}
	return -1;
}


void BADGE::on(void)  { gfx->displayOn();  }
void BADGE::off(void) { gfx->displayOff();  }
