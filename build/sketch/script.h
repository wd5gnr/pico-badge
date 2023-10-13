#line 1 "/home/alw/projects/embedded/ltest/script.h"
/* GNR-Badge script

You can use the following commands (no semicolons). You can do some C code here (e.g., variables)
but note they will be evaluated one time during setup (so, making a random number here only gets you
one random number per reset).

Commands:

Sclear(color) - Clears screen
Sbwimg(imgarray) - Show 240x240 monochrome image
Simage(imgarray) - Show 240x240 RGB565 image
Sdelay(ms) - Delay milliseconds (may be scaled)
Stxtsixe(n) - Set text size multiplier
Stxtcolor(fg, bg) - Set foreground/background color
Stxttrans(fg) - Set text to color (transparent background)
Sfont(font) - Set font name from https://github.com/olikraus/u8g2/wiki/fntlistall (do not use u8g2_font_ prefix)
Stext(x,y,text) - Write text. Use -1 to center either axis (one line only), 999 to use current, \n ok
Son() - Turn display on
Soff() - Turn display off
Sexit() - Not necessary at end, but useful if you want to end early for testing

*/

// Load images here
// You can generate these include files using something like
// https://javl.github.io/image2cpp/ (b&w) or use GIMP to export them to a .C file (see notes.md)
// Image must be mono or RGB565 and must be 240x240
#include "wrencher.h"  // black and white
#include "head2gimp.h"  // RGB565 image

// setup
int tdelay = 2000;

// clang-format off
Sbegin
  Sclear(BLACK)
  Sbwimg(wrencher)
  Sdelay(tdelay)
  Sclear(RED)
  Sfont(mystery_quest_24_tr)
  Stxtsize(2)
  Stxttrans(WHITE)
  Stext(TCENTER, TCENTER, "Hello\n")
  Stext(TCENTER,TCURRENT, "I'm\n")
  Stext(TCENTER, TCURRENT, "Al Williams")
  Sdelay(tdelay)
  Sclear(BLACK)
  Simage(headphone240)
  Sdelay(tdelay)
  Sclear(BLUE)
  Sfont(CursivePixel_tr)
  Stxtsize(3)
  Stxttrans(YELLOW)
  Stext(TCENTER, TCENTER, "Al Williams")
  Sdelay(tdelay)

Send



// clang-format on

/*
You can have a custom C function here called customize
You can modify the current frame and also check buttons againt
BTNA_MASK, BTNB_MASK, BTNC_MASK, BTND_MASK (Button A, B, C, D)
JUP_MASK, JDN_MASK, JLF_MASK, JRT_MASK, JPB_MASK (Joystick up, down, left, right, push)


	void customize(int prepost, int &frame, unsigned max, unsigned buttons)
{
	// pre=0 for before frame and 1 for after frame
    // frame is current step (you can change it)
    // max is the maximum value for frame
    // buttons is the current state of the buttons (use readpins() to refresh )
}

If you don't want a customize function use NO_CUSTOM

*/
// NO_CUSTOM

void customize(int prepost, int &frame, unsigned max, unsigned buttons)
{
    while (buttons & BTND_MASK)
        buttons = readpins();
}
