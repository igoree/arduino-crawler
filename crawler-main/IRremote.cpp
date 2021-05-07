#include "IRremote.h"
// Provides ISR
#ifndef __AVR_ATmega32U4__
#include <avr/interrupt.h>

volatile irparams_t irparams;
bool MATCH(uint8_t measured_ticks, uint8_t desired_us)
{
	// Serial.print(measured_ticks);Serial.print(",");Serial.println(desired_us);
	return(measured_ticks >= desired_us - (desired_us >> 2) - 1 && measured_ticks <= desired_us + (desired_us >> 2) + 1);
}

ISR(TIMER_INTR_NAME)
{
	// Serial.println("ISR");
	 //Serial.println(millis());
	uint8_t irdata = (uint8_t)digitalRead(irparams.recvpin);
	// uint32_t new_time = micros();
	// uint8_t timer = (new_time - irparams.lastTime)>>6;
	irparams.timer++; // One more 50us tick
	if (irparams.rawlen >= RAWBUF)
	{
		// Buffer overflow
		irparams.rcvstate = STATE_STOP;
	}
	switch (irparams.rcvstate)
	{
	case STATE_IDLE: // In the middle of a gap
		if (irdata == MARK)
		{
			irparams.rawlen = 0;
			irparams.timer = 0;
			irparams.rcvstate = STATE_MARK;
		}
		break;
	case STATE_MARK: // timing MARK
		if (irdata == SPACE)
		{
			// MARK ended, record time
			irparams.rawbuf[irparams.rawlen++] = irparams.timer;
			irparams.timer = 0;
			irparams.rcvstate = STATE_SPACE;
		}
		break;
	case STATE_SPACE: // timing SPACE
		if (irdata == MARK)
		{
			// SPACE just ended, record it
			irparams.rawbuf[irparams.rawlen++] = irparams.timer;
			irparams.timer = 0;
			irparams.rcvstate = STATE_MARK;
		}
		else
		{ // SPACE
			if (irparams.timer > GAP_TICKS)
			{
				// big SPACE, indicates gap between codes
				// Mark current code as ready for processing
				// Switch to STOP
				// Don't reset timer; keep counting space width
				irparams.rcvstate = STATE_STOP;
				irparams.lastTime = millis();
			}
		}
		break;
	case STATE_STOP: // waiting, measuring gap
		if (millis() - irparams.lastTime > 120)
		{
			irparams.rawlen = 0;
			irparams.timer = 0;
			irparams.rcvstate = STATE_IDLE;
		}
		else if (irdata == MARK)
		{
			// reset gap timer
			irparams.timer = 0;
		}
		break;
	}
	// irparams.lastTime = new_time;
}

/**
 * Alternate Constructor which can call your own function to map the IR to arduino port,
 * no pins are used or initialized here.
 * \param[in]
 *   None
 */
IRRemote::IRRemote(int pin)
{
	pinMode(pin, INPUT);
	irparams.recvpin = pin;
	// attachInterrupt(INT0, irISR, CHANGE);

	irDelayTime = 0;
	irRead = 0;
	irReady = false;
	irPressed = false;
	begin();
}

/**
 * \par Function
 *    begin
 * \par Description
 *    Initialize interrupt.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
void IRRemote::begin()
{
	cli();
	// setup pulse clock timer interrupt
	//Prescale /8 (16M/8 = 0.5 microseconds per tick)
	// Therefore, the timer interval can range from 0.5 to 128 microseconds
	// depending on the reset value (255 to 0)
	TIMER_CONFIG_NORMAL();

	//Timer2 Overflow Interrupt Enable
	TIMER_ENABLE_INTR;

	// TIMER_RESET;

	sei();  // enable interrupts

	// initialize state machine variables
	irparams.rawlen = 0;
	irparams.rcvstate = STATE_IDLE;
}

/**
 * \par Function
 *    end
 * \par Description
 *    Close the interrupt.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
void IRRemote::end()
{
	EIMSK &= ~(1 << INT0);
}

/**
 * \par Function
 *    decode
 * \par Description
 *    Decodes the received IR message.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    Returns 0 if no data ready, 1 if data ready.
 * \par Others
 *    Results of decoding are stored in results.
 */
ErrorStatus IRRemote::decode()
{
	rawbuf = irparams.rawbuf;
	rawlen = irparams.rawlen;
	if (irparams.rcvstate != STATE_STOP)
	{
		return ERROR;
	}

	if (decodeNEC())
	{
		begin();
		return SUCCESS;
	}
	begin();
	return ERROR;
}

/**
 * \par Function
 *    decodeNEC
 * \par Description
 *    Decodes NEC the received IR message.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    Returns ERROR if decode NEC no done, SUCCESS if decode NEC done.
 * \par Others
 *    Results of decode NEC.
 */
 // NECs have a repeat only 4 items long
ErrorStatus IRRemote::decodeNEC()
{
	static unsigned long repeat_value = 0xFFFFFFFF;
	static byte repeta_time = 0;
	uint32_t data = 0;
	int offset = 0; // Skip first space
	// Initial mark
	if (!MATCH(rawbuf[offset], NEC_HDR_MARK / 50))
	{
		return ERROR;
	}
	offset++;
	// Check for repeat
	if (rawlen == 3 &&
		MATCH(rawbuf[offset], NEC_RPT_SPACE / 50) &&
		MATCH(rawbuf[offset + 1], NEC_BIT_MARK / 50))
	{
		rawbuf[offset] = 0;
		rawbuf[offset + 1] = 0;
		repeta_time++;
		// if(repeta_time > 1) {
		repeta_time = 0;
		bits = 0;
		value = repeat_value;
		// Serial.println("REPEAT");
		decode_type = NEC;
		return SUCCESS;
		//  }
	}
	if (rawlen < (2 * NEC_BITS + 3))
	{
		return ERROR;
	}
	// Initial space
	if (!MATCH(rawbuf[offset], NEC_HDR_SPACE / 50))
	{
		return ERROR;
	}
	rawbuf[offset] = 0;
	offset++;
	for (int i = 0; i < NEC_BITS; i++)
	{
		if (!MATCH(rawbuf[offset], NEC_BIT_MARK / 50))
		{
			return ERROR;
		}
		rawbuf[offset] = 0;
		offset++;
		if (MATCH(rawbuf[offset], NEC_ONE_SPACE / 50))
		{
			//data = (data << 1) | 1;
			data = (data >> 1) | 0x80000000;
		}
		else if (MATCH(rawbuf[offset], NEC_ZERO_SPACE / 50))
		{
			//data <<= 1;
			data >>= 1;
		}
		else
		{
			return ERROR;
		}
		offset++;
	}
	// Success
	bits = NEC_BITS;
	value = data;
	repeat_value = data;
	decode_type = NEC;
	repeta_time = 0;
	return SUCCESS;
}

/**
 * \par Function
 *    mark
 * \par Description
 *    Sends an IR mark for the specified number of microseconds.
 * \param[in]
 *    us - THe time of a PWM.
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
void IRRemote::mark(uint16_t us)
{
	// Sends an IR mark for the specified number of microseconds.
	// The mark output is modulated at the PWM frequency.
	TIMER_ENABLE_PWM; // Enable pin 3 PWM output
	delayMicroseconds(us);
}

/**
 * \par Function
 *    space
 * \par Description
 *    Sends an IR mark for the specified number of microseconds.
 * \param[in]
 *    us - THe time of a PWM.
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
 /* Leave pin off for time (given in microseconds) */
void IRRemote::space(uint16_t us)
{
	// Sends an IR space for the specified number of microseconds.
	// A space is no output, so the PWM output is disabled.
	TIMER_DISABLE_PWM; // Disable pin 3 PWM output
	delayMicroseconds(us);
}

/**
 * \par Function
 *    enableIROut
 * \par Description
 *    Enable an IR for the specified number of khz.
 * \param[in]
 *    us - THe time of a INTR.
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
void IRRemote::enableIROut(uint8_t khz)
{
	TIMER_DISABLE_INTR; //Timer2 disable Interrupt
	TIMER_CONFIG_KHZ(khz);
}

/**
 * \par Function
 *    enableIRIn
 * \par Description
 *    Enable an IR to write in.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    None
 * \par Others
 *    None
 */
 // initialization
void IRRemote::enableIRIn() {
	cli();
	// setup pulse clock timer interrupt
	//Prescale /8 (16M/8 = 0.5 microseconds per tick)
	// Therefore, the timer interval can range from 0.5 to 128 microseconds
	// depending on the reset value (255 to 0)
	TIMER_CONFIG_NORMAL();

	//Timer2 Overflow Interrupt Enable
	TIMER_ENABLE_INTR;

	//TIMER_RESET;

	sei();  // enable interrupts

	// initialize state machine variables
	irparams.rcvstate = STATE_IDLE;
	irparams.rawlen = 0;

	// set pin modes
	pinMode(irparams.recvpin, INPUT);
}

/**
 * \par Function
 *    getCode
 * \par Description
 *    Get the reading code.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    Return the result of reading.
 * \par Others
 *    None
 */
unsigned char IRRemote::getCode()
{
	if (decode())
	{
		irRead = ((value >> 8) >> 8) & 0xff;
		irPressed = true;
		if (irRead == 0xa || irRead == 0xd)
		{
			irReady = true;
		}

		irDelayTime = millis();
	}
	else
	{
		if (irRead > 0)
		{
			if (millis() - irDelayTime > 50)
			{
				irPressed = false;
				irRead = 0;
				irDelayTime = millis();
			}
		}
	}

	return irRead;
}

/**
 * \par Function
 *    keyPressed
 * \par Description
 *    Press key.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \par Return
 *    Return you the pressed key or not.
 * \par Others
 *    None
 */
boolean IRRemote::keyPressed(unsigned char r)
{
	return getCode() == r;
}
#endif // !defined(__AVR_ATmega32U4__)