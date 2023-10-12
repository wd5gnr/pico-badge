#include <Arduino.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>

#define BTNA 15
#define BTNB 17
#define BTNC 19
#define BTND 21
#define JUP 2
#define JDN 18
#define JLF 16
#define JRT 20
#define JPR 3

#define LCDBL 13 // incorrect in board support?

Arduino_DataBus *bus = new Arduino_RPiPicoSPI(
	PIN_LCD_DC,
	PIN_LCD_CS,
	PIN_LCD_SCLK,
	PIN_LCD_MOSI,
	-1,
	spi1);

Arduino_GFX *gfx = new Arduino_ST7789(bus, PIN_LCD_RST, 3, true, 320, 320, 0, 0, 0, 320 - 240);



typedef enum
{
	CLEAR,
	TEXT,
	TSIZE,
	TCOLOR,
	BWIMG,
	COLORIMG,
	MSDELAY,
	ONOFF,
	FONT,
	END
} SCRIPTTYPE;

typedef struct
{
	SCRIPTTYPE type;
	const char *text;
	const uint8_t *data;
	int arg;
	int arg2;
} SCRIPT;

#define Sbegin SCRIPT script[] = {
#define Send \
	}        \
	;
#define Sclear(c) {CLEAR, NULL, NULL, c, 0},
#define Sbwimg(img) {BWIMG, NULL, img, 0, 0},
#define Simage(img) {COLORIMG, NULL, (const uint8_t *)img, 0, 0},
#define Sdelay(ms) {MSDELAY, NULL, NULL, ms, 0},
#define Stxtsize(sz) {TSIZE, NULL, NULL, sz, 0},
#define Stxtcolor(fg, bg) {TCOLOR, NULL, NULL fg, bg},
#define Stxttrans(fg) {TCOLOR, NULL, NULL, fg, fg},
#define Stext(x, y, t) {TEXT, t, NULL, x, y},
#define Sfont(fn) { FONT, NULL, u8g2_font_ ## fn, 0, 0 },
#define Son() {ONOFF, NULL, NULL, 1, 0},
#define Soff() {ONOFF, NULL, NULL, 0, 0},
#define Sexit() {END, NULL, NULL, 0, 0}, // not needed unless early exit

#define NO_CUSTOM \
	void customize(int prepost, int &frame, unsigned max, unsigned buttons) {}


#define TCENTER -1
#define TCURRENT 999

// Flip these around to our orientation (not the board markings)
#define BTNA_MASK 8
#define BTNB_MASK 4
#define BTNC_MASK 2
#define BTND_MASK 1
#define JUP_MASK 32
#define JDN_MASK 16
#define JLF_MASK 128
#define JRT_MASK 64
#define JPR_MASK 256
unsigned readpins(void);  // script might want this



#include "script.h"

unsigned ipins[] = {BTNA, BTNB, BTNC, BTND, JUP, JDN, JLF, JRT, JPR};

void initIO()
{
	for (int i = 0; i < sizeof(ipins) / sizeof(ipins[0]); i++)
	{
		pinMode(ipins[i], INPUT_PULLUP);
	}
	pinMode(LCDBL, OUTPUT);
	digitalWrite(LCDBL, HIGH); // shouldn't be needed
}

unsigned readpins(void)
{
	int i;
	unsigned rv = 0, mask = 1;
	for (i = 0; i < sizeof(ipins) / sizeof(ipins[0]);i++)
	{
		if (digitalRead(ipins[i]))
			rv |= mask;
		mask <<= 1;
	}
	return 0x1FF ^ rv;
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
	int maxstep = sizeof(script)/ sizeof(script[0]);
	for (int i = 0; i < maxstep; i++)
	{
		customize(0, i, maxstep, readpins());
		switch (script[i].type)
		{
		case CLEAR:
			gfx->fillScreen(script[i].arg);
			break;
		case MSDELAY:
			delay(script[i].arg);
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
			x0 = script[i].arg;
			y0 = script[i].arg2;
			if (script[i].arg < 0 || script[i].arg2 < 0)
			{
				gfx->getTextBounds(script[i].text, 0, 0, &x0, &y0, &w, &h);
				if (script[i].arg < 0)
					x0 = (240 - w) / 2;
				else
					x0 = script[i].arg;
				if (script[i].arg2 < 0)
					{
					y0 = (240 - h) / 2;
					y0 += h / 2;
					}
				else
					y0 = script[i].arg2;
			}
			if (x0 > 500)
				x0 = gfx->getCursorX();
			if (y0 > 500)
				y0 = gfx->getCursorY();
			gfx->setCursor(x0, y0);
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
		customize(1, i,maxstep,readpins());
	}
}
