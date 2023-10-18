#include <Arduino.h>
#include "pico-badge.h"
#include <U8g2lib.h>   // needed for fonts
#include <Arduino_GFX_Library.h>  // needed for colors
/* GNR-Badge script

You can use the following commands (no semicolons). You can do some C code here (e.g., variables)
but note they will be evaluated one time during setup (so, making a random number here only gets you
one random number per reset).

Commands:
Stag(number) - Set tag number to identify this part of the script later
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
  Stag(10)
  Sbwimg(wrencher)
  Sdelay(tdelay)

  // Clear to red and print Hello!
  Stag(20)
  Sclear(RED)
  Sfont(timR24_tr)
  Stxtsize(1)
  Stxttrans(WHITE)
  Stext(TCENTER, 100, "Hello I'm\n")
  Sfont(helvB24_tr)
  Stext(TCENTER, TCURRENT, "Al Williams")
  Sdelay(tdelay)

// Show picture
  Stag(30)
  Simage(headphone240)
  Sdelay(tdelay)

// Fancy name
  Stag(40)
  Sclear(BLUE)
  Sfont(CursivePixel_tr)
  Stxtsize(3)
  Stxttrans(YELLOW)
  Stext(TCENTER, TCENTER, "Al Williams")
  Sdelay(tdelay)

// Show GithHub QR Code
  Stag(50)
  Sbwimg(qrcode)
  Sdelay(tdelay)

// Hackaday name plate
  Stag(60)
  Stxtsize(1)
  Sfont(helvB24_tr)
  Sbwimg(monobkrev)
  Stxttrans(WHITE)
  Stext(TCENTER,60,"HACKADAY")
  Stxttrans(BLACK)
  Stext(TCENTER,180,"Al Williams")
  Sdelay(tdelay)
  Stag(9999)     // Elliot's favorite EOF marker

Send



// clang-format on

/*
You can have a custom C function here called customize
You can modify the current frame and also check buttons against these constants in BADGE::
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
    while (buttons & BADGE::BTND_MASK)
        // do something for BTND
}

The BADGE object has helper functions:
BADGE::getbtns(void) - Return current buttons (and clears them)
BADGE::scaledelay(void) - Delay scaled by current scale factor
BADGE::delayscaler(scale) - Add/subtract offset to delay scale (limits to 1 to 10, so to go full offset -1000 and 1000 for example)
BADGE::on() - Badge on (default)
BADGE::off() - Badge off (still runs, just dark. Use BADGE::on to restore
BADGE::findTag(tag) - Gives you the step number for the TAG you specify
BADGE::pause(void) - Returns current pause frame #
BADGE::pause(tag) - Sets pause by tag
BADGE::pausehere(frame) - Set pause by frame #
BADGE::unpause(void) - Stop pausing
BADGE::setloop(tag0, tag1, pause) - Set up a loop from tag0 to tag1. If pause is not zero, badge pauses at end otherwise it repeats
BADGE::setloop(tag0, tag1, pause, type0, type1) - Same as above but type0 or type1 can be non-zero to specify frame instead of tag
BADGE::unloop(void) - Cancel loop

*/


#if 1    // custom vs no custom
/*
A simple example custom file.

On post process it only stores buttons that need attention
On pre process it processes all buttons

Right - no action (tempting to make it "advance" but since some frames build up that's annoying)
Left - Go to start (frame 0)
Press - display on/off
Up - Scale time delay up (slower)
Down - Scale time delay down (faster)
A - Show MAINFRAME through MAINFRAMEEND and pause
B - Show QR Code and pause
C - Pause now
D - Resume from A, B, or C
*/

#define QRFRAME 50
#define MAINFRAME 60
#define MAINFREND 9999

void customize(int prepost, int &frame, unsigned max, unsigned cbuttons)
{
  static unsigned sbuttons = 0;
  unsigned buttons;
  // If this is the post frame we just see if any buttons are pushed and bail
  if (prepost==1)
  {
    sbuttons = cbuttons;
    return;
  }
// everything from here out is on pre frame

  sbuttons |= cbuttons;  // mix pre with post
  buttons = sbuttons;
  sbuttons = 0;

  if (buttons & BADGE::JRT_MASK) // Test looping (right joystick toggles)
  {
    static int toggle = 0;
    // right joystick
    toggle ^= 1;
    if (toggle)
    {
      frame = BADGE::findTag(QRFRAME);
      BADGE::setloop(QRFRAME, MAINFREND);
    }
    else
      BADGE::unloop();
  }
  if (buttons & BADGE::JPR_MASK)
  {
    static int toggle= 0;
    toggle ^= 1;
    if (toggle)
      BADGE::off();
    else
      BADGE::on();
  }
  if (buttons & BADGE::JLF_MASK)  // reverse
  {
    frame = 0;
  }
  if (buttons & BADGE::JUP_MASK)
  {
    BADGE::delayscaler(1);
  }
  if (buttons & BADGE::JDN_MASK)
  {
    BADGE::delayscaler(-1);
  }
  if (buttons & BADGE::BTNC_MASK) // Button C pause
  {
    BADGE::pausehere(frame);
  }
  if (buttons & BADGE::BTND_MASK)  
  {// BTN D pause off
    BADGE::unpause();
    BADGE::unloop(); 
    return;
  }

// If BTN A we are going to run from MAINFRAME to MAINFREND
  if (buttons & BADGE::BTNA_MASK)
  {
    frame = BADGE::findTag(MAINFRAME);
    BADGE::setloop(MAINFRAME, MAINFREND, 1);
    return;
  }
// if BTN B we are to just pause on QRFRAME
  if (buttons & BADGE::BTNB_MASK)
  {
    BADGE::pause(QRFRAME);
    return;
  }
}
#else

NO_CUSTOM

#endif
