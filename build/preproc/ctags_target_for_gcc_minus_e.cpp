# 1 "/home/alw/projects/embedded/pico-badge/pico-badge.ino"
# 2 "/home/alw/projects/embedded/pico-badge/pico-badge.ino" 2
# 3 "/home/alw/projects/embedded/pico-badge/pico-badge.ino" 2
# 4 "/home/alw/projects/embedded/pico-badge/pico-badge.ino" 2
# 17 "/home/alw/projects/embedded/pico-badge/pico-badge.ino"
Arduino_DataBus *bus = new Arduino_RPiPicoSPI(
 (8u),
 (9u),
 (10u),
 (11u),
 -1,
 ((spi_inst_t *)((spi_hw_t *)0x40040000u)));

Arduino_GFX *gfx = new Arduino_ST7789(bus, (12u), 3, true, 320, 320, 0, 0, 0, 320 - 240);



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
# 77 "/home/alw/projects/embedded/pico-badge/pico-badge.ino"
// Flip these around to our orientation (not the board markings)
# 87 "/home/alw/projects/embedded/pico-badge/pico-badge.ino"
unsigned readpins(void); // script might want this

volatile uint16_t buttons=0; // buttons read by processor #1
uint16_t getbtns(void)
{
 uint16_t rv;
 rp2040.idleOtherCore();
 rv|=buttons;
 buttons = 0;
 rp2040.resumeOtherCore();
 return rv;
}

int dscale = 1;

void scaledelay(unsigned n)
{
 delay(n * dscale);
}

void delayscaler(int offset)
{
 if (dscale+offset<1)
  dscale = 1;
 else if (dscale+offset>10)
  dscale = 10;
 else
  dscale += offset;
}



# 120 "/home/alw/projects/embedded/pico-badge/pico-badge.ino" 2

unsigned ipins[] = {15, 17, 19, 21, 2, 18, 16, 20, 3};

void initIO1()
{
 for (int i = 0; i < sizeof(ipins) / sizeof(ipins[0]); i++)
 {
  pinMode(ipins[i], INPUT_PULLUP);
 }
}

void initIO()
{
  pinMode(13 /* incorrect in board support?*/, OUTPUT);
 digitalWrite(13 /* incorrect in board support?*/, HIGH); // shouldn't be needed
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
 return 0x1FF ^ rv; // active low buttons so flip 'em
}


void setup1() // second core will handle buttons
{
 initIO1();
}


void loop1()
{
 rp2040.idleOtherCore();
 buttons|=readpins();
 rp2040.resumeOtherCore();
 delay(5);
}

void setup()
{
 initIO();

 gfx->begin();
 gfx->fillScreen(((((0)&0xF8) << 8) | (((0)&0xFC) << 3) | ((0) >> 3)));
 gfx->setFont(u8g2_font_mystery_quest_24_tr);







}


void loop()
{
 int maxstep = sizeof(script)/ sizeof(script[0]);
 for (int i = 0; i < maxstep; i++)
 {
  customize(0, i, maxstep, getbtns());
  switch (script[i].type)
  {
  case CLEAR:
   gfx->fillScreen(script[i].arg);
   break;
  case MSDELAY:
   scaledelay(script[i].arg);
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
   x0 = script[i].arg; // assume we are at XY
   y0 = script[i].arg2;
   if (script[i].arg < 0 || script[i].arg2 < 0) // if either axis is centered
   {
    gfx->getTextBounds(script[i].text, 0, 0, &x0, &y0, &w, &h); // get box around text
    if (script[i].arg < 0) // if we want to center on X
     x0 = (240 - w) / 2;
    else
     x0 = script[i].arg; // reset x0 because only y?
    if (script[i].arg2 < 0) // if we want to center on y
     {
     y0 = (240 - h) / 2;
     y0 += h;
     }
    else
     y0 = script[i].arg2; // reset y0 because only x?
   }
   if (x0 > 500) // if x0 is bigger than screen use current x
    x0 = gfx->getCursorX();
   if (y0 > 500) // if y0 is bigger than screen use current y
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
   gfx->drawBitmap(0, 0, script[i].data, 240, 240, ((((255)&0xF8) << 8) | (((255)&0xFC) << 3) | ((255) >> 3)), ((((0)&0xF8) << 8) | (((0)&0xFC) << 3) | ((0) >> 3)));
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
   break; // never reached!
  }
  customize(1, i,maxstep,getbtns());
 }
}
