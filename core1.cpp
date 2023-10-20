#include <Arduino.h>
#include "pico-badge.h"

// Core #1 only handles buttons

// The only thing shared with core 0
extern volatile  uint16_t buttons;

// Button pins
static unsigned ipins[] = {BTNA, BTNB, BTNC, BTND, JUP, JDN, JLF, JRT, JPR};

// Init pins for core #1
static void initIO1()
{
	for (int i = 0; i < sizeof(ipins) / sizeof(ipins[0]); i++)
	{
		pinMode(ipins[i], INPUT_PULLUP);
	}
}

// Read the buttons (call from core #1)
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

// Read buttons with short debounce delay
static unsigned readdebounce(void)
{
	unsigned s1, s2;
	s1 = readpins();
	delay(33);  // debounce delay
	s2 = readpins();
	return s1 & s2;  // must be on both times
}

// Core #1 just reads the buttons
// This code ensures that one button push gets one key event
// to core #0 -- Again, idleOtherCore acts like a critical section
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
