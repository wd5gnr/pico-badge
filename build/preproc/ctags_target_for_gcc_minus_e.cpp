# 1 "/home/alw/projects/embedded/ltest/ltest.ino"
# 2 "/home/alw/projects/embedded/ltest/ltest.ino" 2
# 3 "/home/alw/projects/embedded/ltest/ltest.ino" 2
# 4 "/home/alw/projects/embedded/ltest/ltest.ino" 2
# 17 "/home/alw/projects/embedded/ltest/ltest.ino"
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
# 76 "/home/alw/projects/embedded/ltest/ltest.ino"
// Flip these around to our orientation (not the board markings)
# 86 "/home/alw/projects/embedded/ltest/ltest.ino"
unsigned readpins(void); // script might want this



# 91 "/home/alw/projects/embedded/ltest/ltest.ino" 2

unsigned ipins[] = {15, 17, 19, 21, 2, 18, 16, 20, 3};

void initIO()
{
 for (int i = 0; i < sizeof(ipins) / sizeof(ipins[0]); i++)
 {
  pinMode(ipins[i], INPUT_PULLUP);
 }
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
 return 0x1FF ^ rv;
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
  customize(1, i,maxstep,readpins());
 }
}
