#include <Arduino.h>
#include "pico-badge.h"


static Arduino_DataBus *bus = new Arduino_RPiPicoSPI(
	PIN_LCD_DC,
	PIN_LCD_CS,
	PIN_LCD_SCLK,
	PIN_LCD_MOSI,
	-1,
	spi1);

Arduino_GFX *gfx = new Arduino_ST7789(bus, PIN_LCD_RST, 3, true, 320, 320, 0, 0, 0, 320 - 240);




volatile static uint16_t buttons = 0; // buttons read by processor #1

int btn_pending(void) { return buttons!=0; }

uint16_t getbtns(void)
	{
		uint16_t rv;
		rp2040.idleOtherCore();
		rv = buttons;
		buttons = 0;
		rp2040.resumeOtherCore();
		return rv;
}





static unsigned ipins[] = {BTNA, BTNB, BTNC, BTND, JUP, JDN, JLF, JRT, JPR};

static void initIO1()
{
	for (int i = 0; i < sizeof(ipins) / sizeof(ipins[0]); i++)
	{
		pinMode(ipins[i], INPUT_PULLUP);
	}
}

static void initIO()
{
 	pinMode(LCDBL, OUTPUT);
	digitalWrite(LCDBL, HIGH); // shouldn't be needed
}

static unsigned readpins(void)
{
	int i;
	unsigned rv = 0, mask = 1;
	for (i = 0; i < sizeof(ipins) / sizeof(ipins[0]);i++)
	{
		if (digitalRead(ipins[i]))
			rv |= mask;
		mask <<= 1;
	}
	return 0x1FF ^ rv;  // active low buttons so flip 'em
}


void setup1() // second core will handle buttons
{
	initIO1();
}

static unsigned readdebounce(void)
{
	unsigned s1, s2;
	s1 = readpins();
	delay(33);  // debounce delay
	s2 = readpins();
	return s1 & s2;  // must be on both times
}

void loop1()
{
	static unsigned btnstate = 0;
	static unsigned xbtn;
	unsigned cbutton;
	cbutton = readdebounce();  // current buttons
	xbtn = cbutton & ~btnstate;  // turn off any that were on before
	btnstate = cbutton;   // ready for next time
	rp2040.idleOtherCore();
	buttons |= xbtn; // put in any that are set; client will reset them
	rp2040.resumeOtherCore();
	delay(5); // give other core some time to run
}

void setup()
{
	initIO();

	gfx->begin();
	gfx->fillScreen(BLACK);
	gfx->setFont(u8g2_font_mystery_quest_24_tr);
#if 0    
    gfx->setCursor(10, 10);  // bare minimum setup if you want to test things
    gfx->setTextSize(5);
    gfx->setTextColor(RED);
    gfx->println("WD5GNR");
#else
#endif
}


void loop()
{
	static int looping = -1;
	int maxstep = script_size;
	for (int i = 0; i < maxstep; i++)
	{
		if (BADGE::loopstart() >= 0)
			if (looping<0) looping=i = BADGE::loopstart();
			else
				i = looping++;

		if (BADGE::loopmax()>=0 && i>=BADGE::loopmax())
		{
			if (BADGE::pause()>=0)
			{
				BADGE::pause(i,1);
				BADGE::unloop();
				looping = -1;
			}
			else
				looping = BADGE::loopstart();
			break;
		}
		if (BADGE::pause()>=0 && BADGE::loopstart()<0)
			i = BADGE::pause();
		customize(0, i, maxstep, getbtns());
		switch (script[i].type)
		{
		case TAG:
			continue;  // just a label
		case CLEAR:
			gfx->fillScreen(script[i].arg);
			break;
		case MSDELAY:
			BADGE::scaledelay(script[i].arg);
			break;
		case TCOLOR:
			gfx->setTextColor(script[i].arg, script[i].arg2);
			break;
		case TSIZE:
			gfx->setTextSize(script[i].arg);
			break;
		case TEXT:
		{

			int16_t x0, y0;
			uint16_t w, h;
			x0 = script[i].arg;  // assume we are at XY
			y0 = script[i].arg2;
			if (script[i].arg < 0 || script[i].arg2 < 0)  // if either axis is centered
			{
				gfx->getTextBounds(script[i].text, 0, 0, &x0, &y0, &w, &h);  // get box around text
				if (script[i].arg < 0)  // if we want to center on X
					x0 = (240 - w) / 2;
				else
					x0 = script[i].arg;   // reset x0 because only y?
				if (script[i].arg2 < 0)   // if we want to center on y
					{
					y0 = (240 - h) / 2;  
					y0 += h;
					}
				else
					y0 = script[i].arg2;  // reset y0 because only x?
			}
			if (x0 > 500)  // if x0 is bigger than screen use current x
				x0 = gfx->getCursorX();
			if (y0 > 500)    // if y0 is bigger than screen use current y
				y0 = gfx->getCursorY();
// go to whatever we figured out
			gfx->setCursor(x0, y0);    
// print text			
			gfx->print(script[i].text);
		}
		break;
		case FONT:
			gfx->setFont(script[i].data);
			break;

		case BWIMG:
			gfx->drawBitmap(0, 0, script[i].data, 240, 240, WHITE, BLACK);
			break;
		case COLORIMG:
			// printf("Color\r\n");  // without this printf, (not serial.printf) the next call doesn't work?
			//  unless "stack protector" is on in board settings
			gfx->draw16bitRGBBitmap(0, 0, (const uint16_t *)script[i].data, 240, 240);
			break;
		case ONOFF:
			if (script[1].arg)
				gfx->displayOn();
			else
				gfx->displayOff();
		case END: // you don't really need this but it is handy to be able to stop the script early
			// when debugging
			return; // end loop and start again later
			break;	// never reached!
		}
		customize(1, i,maxstep,getbtns());
	}
}
