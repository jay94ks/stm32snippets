#include "PCF8574.h"

pcf8574_t::pcf8574_t(const i2cm_t& i2c)
	: _i2c(i2c), _began(0),
	  _stateIn(0xff), _stateOut(0xff),
	  _timeIn(10), _timeOut(10)
{
}

bool pcf8574_t::flush() {
	return _i2c.write(&_stateOut, sizeof(_stateOut), _timeOut);
}

bool pcf8574_t::receive() {
	return _i2c.read(&_stateIn, sizeof(_stateIn), _timeIn);
}

pcf8574_t& pcf8574_t::timeout(uint8_t tin, uint8_t tout) {
	_timeIn = tin;
	_timeOut = tout;
	return *this;
}

bool pcf8574_t::begin() {
	if (_began) {
		return false;
	}

	if (receive() == false) {
		return false;
	}

	_began = true;
	return true;
}

bool pcf8574_t::end() {
	if (!_began) {
		return false;
	}

	if (flush() == false) {
		return false;
	}

	_began = false;
	return true;
}

pcf8574_t& pcf8574_t::write(uint8_t pin, bool state) {
	uint8_t mask = (1 << pin);

	if (state) {
		_stateOut |= mask;
	}
	else {
		_stateOut &= ~mask;
	}


	if (!_began) {
		flush();
	}

	return *this;
}

bool pcf8574_t::read(uint8_t pin) {
	uint8_t mask = (1 << pin);

	if (!_began) {
		receive();
	}

	return (_stateIn & mask) != 0;
}
