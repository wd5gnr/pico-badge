#ifndef __PICO_BADGE_H
#define __PICO_BADGE_H

// Main header for pico badge

#include <U8g2lib.h>  // for fonts
#include <Arduino_GFX_Library.h>  // for graphics

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




// Script commands
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
	TAG,
	END
} SCRIPTTYPE;

// Each step of a script
typedef struct
{
	SCRIPTTYPE type;
	const char *text;
	const uint8_t *data;
	int arg;
	int arg2;
} SCRIPT;

// Preprocessor macros to create script
#define Sbegin SCRIPT script[] = {
#define Send \
    }        \
    ;        \
    unsigned script_size = sizeof(script) / sizeof(script[0]);
#define Sclear(c) {CLEAR, NULL, NULL, c, 0},
#define Sbwimg(img) {BWIMG, NULL, img, WHITE, BLACK},
#define S2img(img,c1,c2) { BWIMG, NULL, img, c1, c2 },
#define Simage(img) {COLORIMG, NULL, (const uint8_t *)img, 0, 0},
#define Sdelay(ms) {MSDELAY, NULL, NULL, ms, 0},
#define Stxtsize(sz) {TSIZE, NULL, NULL, sz, 0},
#define Stxtcolor(fg, bg) {TCOLOR, NULL, NULL fg, bg},
#define Stxttrans(fg) {TCOLOR, NULL, NULL, fg, fg},
#define Stext(x, y, t) {TEXT, t, NULL, x, y},
#define Sfont(fn) { FONT, NULL, u8g2_font_ ## fn, 0, 0 },
#define Son() {ONOFF, NULL, NULL, 1, 0},
#define Soff() {ONOFF, NULL, NULL, 0, 0},
#define Stag(n) { TAG, NULL, NULL, n, 0},
#define Sexit() {END, NULL, NULL, 0, 0}, // not needed unless early exit

// Use this if you want no customize function
#define NO_CUSTOM \
	void customize(int prepost, int &frame, unsigned max, unsigned buttons) {}

// center text axis
#define TCENTER -1
// use current text position
#define TCURRENT 999

// The actual BADGE class

class BADGE
{
	protected:
		static int pauseval;    // frame to pause
		static int loopstartval;  // start of loop (frame)
		static int loopmaxval;   // end of loop (frame)
        static int dscale;      // delay scale (1-10 multiplier for delays)

    public:
        static uint16_t getbtns(void);   // get physical buttons
    // The various switches show up with these bits
	// note these are rearranged from the board layout
	enum { BTNA_MASK = 8,
				  BTNB_MASK = 4,
				  BTNC_MASK = 2,
				  BTND_MASK = 1,
				  JUP_MASK = 32,
				  JDN_MASK = 16,
				  JLF_MASK = 128,
				  JRT_MASK = 64,
				  JPR_MASK = 256 };
	static void scaledelay(unsigned n);   // delay in ms with scale
    static void delayscaler(int offset);   // setup scale for delays
    static void on(void);   // display on
    static void off(void);  // or off
    static int findTag(int tag);  // find tag and return sequence number
    static int pause(void) { return pauseval; }  // read pause
	static void pause(int tag, int type=0) { pauseval = type?tag:(findTag(tag) + 1); }  // set pause (type=0 is tag, non-zero is frame)
	static void unpause(void) { pauseval = -1; }   // stop pause
	static void unloop(void) { loopstartval=loopmaxval = -1; }  // stop loop
	static void setloop(int tag0, int tag1, int pause=0, int type0 = 0, int type1 = 0);  // Set loop easily
	static void pausehere(int n) { pauseval = n; }; // pause on frame number
	// user probably doesn't need these
   static int loopstart(void) { return loopstartval;  }
   static void loopstart(int tag, int type = 0) { loopstartval = type ? tag:(findTag(tag) + 1);  }
   static int loopmax(void) { return loopmaxval;  }
   static void loopmax(int tag, int type=0) { loopmaxval = type?tag:findTag(tag);  }
	};

// your script
extern SCRIPT script[];
// your customzie function (see NO_CUSTOM)
extern void customize(int prepost, int &frame, unsigned max, unsigned buttons);
// see if a button is pending
extern int btn_pending(void);
// library for graphics
extern Arduino_GFX *gfx;
// The external font size
extern unsigned script_size;
// Get buttons (raw)
uint16_t getbtns(void); // should not use directly

#endif