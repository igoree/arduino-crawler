#ifndef RGBLed_h
#define RGBLed_h
#include <Arduino.h>

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

	bool operator == (const RGBColor& otherColor) const;
	bool operator != (const RGBColor& otherColor) const;
};

class RGBLed
{
public:
	const uint8_t ledCount;

	RGBLed(uint8_t port, uint8_t ledCount = 1);
	~RGBLed();

	RGBColor getColor(uint8_t ledIndex);
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
