#ifndef __74HC595_H__
#define __74HC595_H__

// --> PIN library.
#include "../pin/pin.h"

/**
 * Describes 74HC595 bit-shift register.
 */
class l74hc595_t {
private:
    pin_t _data;
    pin_t _clock;
    pin_t _latch;

    uint8_t _state;
    uint8_t _value;

public:
    l74hc595_t() : _state(0), _value(0) { }
    l74hc595_t(pin_t data, pin_t clock, pin_t latch);

public:
    /* begin the state cache to optimise IO clock. */
    bool begin();

    /* end the state cache. */
    void end(bool flush = true);

    /* flush the state cache to physical chipset. */
    void flush();

    /* read a pin from state cache. */
    pin_state_t read(uint8_t pin);

    /* write a pin into state cache or direct out. */
    l74hc595_t& write(uint8_t pin, pin_state_t state);
};

#endif // __74HC595_H__