#ifndef RGBLed_h
#define RGBLed_h
#include <Arduino.h>

#define DEFAULT_MAX_LED_NUMBER  (32)

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

/// @brief Class for RGB Led Module
struct RGBColor
{
	uint8_t green;
	uint8_t red;
	uint8_t blue;
};

/**
 * Class: RGBLed
 *
 * \par Description
 * Declaration of Class RGBLed
 */

class RGBLed
{
public:
	const uint8_t ledCount;

	/**
	 * Alternate Constructor which can call your own function to map the RGBLed to arduino port,
	 * it will assigned the LED display buffer and initialization the GPIO of LED lights. You can
	 * set any slot for the LED data PIN, and reset the LED numberby this constructor.
	 * \param[in]
	 *   port - arduino port
	 * \param[in]
	 *   ledCount - The LED number
	 */
	RGBLed(uint8_t port, uint8_t ledCount = 1);

	/**
	 * Destructor which can call your own function, it will release the LED buffer
	 */
	~RGBLed();

	/**
	 * \par Function
	 *   getColorAt
	 * \par Description
	 *   Get the LED color value from its index
	 * \param[in]
	 *   index - The LED index number you want to read its value
	 * \par Output
	 *   None
	 * \return
	 *   The LED color value, include the R,G,B
	 * \par Others
	 *   The index value from 1 to the max
	 */
	RGBColor getColorAt(uint8_t index);

	/**
	 * \par Function
	 *   setColorAt
	 * \par Description
	 *   Set the LED color for any LED.
	 * \param[in]
	 *   index - The LED index number you want to set its color
	 * \param[in]
	 *   red - Red values
	 * \param[in]
	 *   green - green values
	 * \param[in]
	 *   blue - blue values
	 * \par Output
	 *   None
	 * \return
	 *   TRUE: Successful implementation
	 *   FALSE: Wrong execution
	 * \par Others
	 *   The index value from 0 to the max.
	 */
	bool setColorAt(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

	/**
	 * \par Function
	 *   setColor
	 * \par Description
	 *   Set the LED color for any LED.
	 * \param[in]
	 *   index - The LED index number you want to set its color
	 * \param[in]
	 *   red - Red values
	 * \param[in]
	 *   green - green values
	 * \param[in]
	 *   blue - blue values
	 * \par Output
	 *   None
	 * \return
	 *   TRUE: Successful implementation
	 *   FALSE: Wrong execution
	 * \par Others
	 *   The index value from 1 to the max, if you set the index 0, all the LED will be lit
	 */
	bool setColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

	/**
	 * \par Function
	 *   setColor
	 * \par Description
	 *   Set the LED color for all LED.
	 * \param[in]
	 *   red - Red values
	 * \param[in]
	 *   green - green values
	 * \param[in]
	 *   blue - blue values
	 * \par Output
	 *   None
	 * \return
	 *   TRUE: Successful implementation
	 *   FALSE: Wrong execution
	 * \par Others
	 *   All the LED will be lit.
	 */
	bool setColor(uint8_t red, uint8_t green, uint8_t blue);

	/**
	 * \par Function
	 *   setColor
	 * \par Description
	 *   Set the LED color for any LED.
	 * \param[in]
	 *   value - the LED color defined as long type, for example (white) = 0xFFFFFF
	 * \par Output
	 *   None
	 * \return
	 *   TRUE: Successful implementation
	 *   FALSE: Wrong execution
	 * \par Others
	 *   The index value from 1 to the max, if you set the index 0, all the LED will be lit
	 */
	bool setColor(uint8_t index, long value);

	/**
	 * \par Function
	 *   show
	 * \par Description
	 *   Transmission the data to WS2812
	 * \par Output
	 *   None
	 * \return
	 *   None
	 * \par Others
	 *   None
	 */
	void show();

private:
	uint8_t* _pixels;

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
	 *   TRUE: Successful implementation
	 *   FALSE: Wrong execution
	 * \par Others
	 *   None
	 */
	void rgbled_sendarray_mask(uint8_t* array, uint16_t length, uint8_t pinmask, uint8_t* port);

	const volatile uint8_t* ws2812_port;
	volatile uint8_t* ws2812_port_reg;
	uint8_t pinMask;
};
#endif
