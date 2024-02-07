#ifndef __PCF8574_H__
#define __PCF8574_H__

#include "../i2cm/i2cm.h"

class pcf8574_t {
private:
	i2cm_t _i2c;
	uint8_t _began;
	uint8_t _stateIn;
	uint8_t _stateOut;
	uint8_t _timeIn, _timeOut;

public:
	pcf8574_t(const i2cm_t& i2c);

private:
	bool flush();
	bool receive();

public:
	/* set timeout. */
	pcf8574_t& timeout(uint8_t tin, uint8_t tout);

	/* begin transmission. */
	bool begin();

	/* end transmission. */
	bool end();

	/* set pin out. */
	pcf8574_t& write(uint8_t pin, bool state);

	/* read a pin.*/
	bool read(uint8_t pin);

};

#endif // __PCF8574_H__
