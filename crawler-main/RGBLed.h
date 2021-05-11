#ifndef RGBLed_h
#define RGBLed_h
#include <Arduino.h>

#define RGB_RED     0xFF0000
#define RGB_GREEN   0x00FF00
#define RGB_BLUE    0x0000FF
#define RGB_YELLOW  0xFFFF00
#define RGB_PURPLE  0xFF00FF
#define RGB_ORANGE  0xFFA500
#define RGB_INDIGO  0x4b0082
#define RGB_VIOLET  0x8a2be2
#define RGB_WHITE   0xFFFFFF
#define RGB_BLACK   0

struct RGBColor
{
	RGBColor()
		: RGBColor(0u, 0u, 0u)
	{
	}

	RGBColor(uint8_t red, uint8_t green, uint8_t blue)
		: green(green), red(red), blue(blue)
	{
	}

	uint8_t green;
	uint8_t red;
	uint8_t blue;
};

class RGBLed
{
public:
	const uint8_t ledCount;

	RGBLed(uint8_t port, uint8_t ledCount = 1);
	~RGBLed();

	RGBColor getColor(uint8_t ledIndex);
	bool setColor(uint8_t ledIndex, uint8_t red, uint8_t green, uint8_t blue);
	bool setColor(uint8_t ledIndex, RGBColor color);
	void show();

private:
	RGBColor* _pixels;

	/**
	 * \par Function
	 *   rgbled_sendarray_mask
	 * \par Description
	 *   Set the LED color for any LED.
	 * \param[in]
	 *   *data - the LED color store memory address
	 * \param[in]
	 *   datlen - the data length need to be transmitted.
	 * \param[in]
	 *   maskhi - the gpio pin mask
	 * \param[in]
	 *   *port - the gpio port address
	 * \par Output
	 *   None
	 * \return
	 *   None
	 * \par Others
	 *   None
	 */
	void rgbled_sendarray_mask(uint8_t* array, uint16_t length, uint8_t pinmask, uint8_t* port);

	const volatile uint8_t* ws2812_port;
	volatile uint8_t* ws2812_port_reg;
	uint8_t pinMask;
};
#endif
