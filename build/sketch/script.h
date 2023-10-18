#line 1 "/home/alw/projects/embedded/pico-badge/script.h"
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
// https://javl.github.io/image2cpp/ (b&w) or
// https://lvgl.io/tools/imageconverter (color) or
//  use GIMP to export them to a .C file (see notes.md)
// Image must be mono or RGB565 and must be 240x240
#include "wrencher.h"  // black and white
#include "head2gimp.h"  // RGB565 image
#include "qr-code.h"  // QR code for github
#include "monoback.h"  // two different backgrounds (black/white and white/black)

// setup
int tdelay = 2000;

// clang-format off
Sbegin
  // draw Jolly Wrencher
  Sbwimg(wrencher)
  Sdelay(tdelay)

  // Clear to red and print Hello!
  Sclear(RED)
  Sfont(timR24_tr)
  Stxtsize(1)
  Stxttrans(WHITE)
  Stext(TCENTER, 100, "Hello I'm\n")
  Sfont(helvB24_tr)
  Stext(TCENTER, TCURRENT, "Al Williams")
  Sdelay(tdelay)

// Show picture
  Simage(headphone240)
  Sdelay(tdelay)

// Fancy name
  Sclear(BLUE)
  Sfont(CursivePixel_tr)
  Stxtsize(3)
  Stxttrans(YELLOW)
  Stext(TCENTER, TCENTER, "Al Williams")
  Sdelay(tdelay)

// Show GithHub QR Code
  Sbwimg(qrcode)
  Sdelay(tdelay)

// Hackaday name plate
  Stxtsize(1)
  Sfont(helvB24_tr)
  Sbwimg(monobkrev)
  Stxttrans(WHITE)
  Stext(TCENTER,60,"HACKADAY")
  Stxttrans(BLACK)
  Stext(TCENTER,180,"Al Williams")
  Sdelay(tdelay)

Send



// clang-format on

/*
You can have a custom C function here called customize
You can modify the current frame and also check buttons againt
BTNA_MASK, BTNB_MASK, BTNC_MASK, BTND_MASK (Button A, B, C, D)
JUP_MASK, JDN_MASK, JLF_MASK, JRT_MASK, JPR_MASK (Joystick up, down, left, right, push)


	void customize(int prepost, int &frame, unsigned max, unsigned buttons)
{
	// pre=0 for before frame and 1 for after frame
    // frame is current step (you can change it)
    // max is the maximum value for frame
    // buttons is the current state of the buttons (use readpins() to refresh )
}

If you don't want a customize function use NO_CUSTOM

Example:
void customize(int prepost, int &frame, unsigned max, unsigned buttons)
{
    while (buttons & BTND_MASK)
        // do something for BTND
}


*/

#define QRFRAME 18
#define MAINFRAME 20
#define MAINFREND 26

void customize(int prepost, int &frame, unsigned max, unsigned cbuttons)
{
  static int pause = 0;
  static int next = -1;
  static unsigned buttons = 0;
  // If this is the post frame we just see if any buttons are pushed and bail
  if (prepost==1)
  {
    buttons = cbuttons;
    return;
  }
// everything from here out is on pre frame

  buttons |= cbuttons;  // mix pre with post

  if (buttons & JRT_MASK)  // advance
  {
    // right ?

  }
  if (buttons & JPR_MASK)
  {

  }
  if (buttons & JLF_MASK)  // reverse
  {
    frame = 0;
  }
  if (buttons & JUP_MASK)
  {
    delayscaler(1);
  }
  if (buttons & JDN_MASK)
    delayscaler(-1);
  if (buttons & BTNC_MASK) // Button C pause
  {
    pause = 1;  // don't return because we need to adjust 
  }
  if (buttons & BTND_MASK)  
  {// BTN D pause off
    pause = 0;
    next = -1;
    return;
  }
// deal with freezing MAINFRAME-MAINFREND and then ending
 if (next>=MAINFRAME)  // in the middle of BTNA name tag mode
  {
    if (next==MAINFREND+1)  // are we at the end
    {
      frame = MAINFREND;
      next = -1;
      pause = 1; // now pause
    }
    else
    {
       frame = next;  // do the next step towards MAINFREND
       next++;
    }
  }
// look to see if we need to rewind to keep a pause going
  else if (pause) 
    if (frame==0)
      frame = max; 
      else
      frame--;

// If BTN A we are going to run from MAINFRAME to MAINFREND
  if (buttons & BTNA_MASK)
  {
      next = frame = MAINFRAME;  // turn it on (don't pause)
      return;
  }
// if BTN B we are to just pause on QRFRAME
  if (buttons & BTNB_MASK)
  {
      frame = QRFRAME;
      pause = 1;
      return;
  }
}
//NO_CUSTOM

