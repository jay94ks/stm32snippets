#ifndef __74HC595_H__
#define __74HC595_H__

// --> PIN library.
#include "../pin/pin.h"

// --> memset(...).
#include <string.h>

/**
 * Describes a 74HC595 controller.
 */
class l74hc595_ctl_t {
private:
    pin_t _data;
    pin_t _clock;
    pin_t _latch;
    uint8_t _state;

public:
    l74hc595_ctl_t() : _state(0) { }
    l74hc595_ctl_t(const pin_t& data, const pin_t& clock, const pin_t& latch);

public:
    /* transmit bits. */
    void xfer(uint8_t value);

    /* trigger the latch. */
    void latch();
    
    /* begin the state to optimise IO clock. */
    bool begin();

    /* end the state. */
    bool end();

    /* access the state. */
    inline uint8_t state() const { return _state; }
};

/* write bit to state buffer. */
void l74hc595_write_bit(uint8_t* buf, uint8_t pin, pin_state_t state);

/**
 * Describes 74HC595 bit-shift register.
 */
template<uint32_t pcs = 1>
class g74hc595_t {
private:
    l74hc595_ctl_t _ctl;
    uint8_t _value[pcs];

public:
    g74hc595_t() {
        memset(_value, 0, sizeof(_value));
    }

    g74hc595_t(pin_t data, pin_t clock, pin_t latch)
        : _ctl(data, clock, latch)
    {
        memset(_value, 0, sizeof(_value));
    }

public:
    /* begin the state cache to optimise IO clock. */
    inline bool begin() { return _ctl.begin(); }

    /* end the state cache. */
    inline void end(bool flush = true) {
        if (!_ctl.end()) {
            return;
        }
        
        if (flush) {
            this->flush();
        }
    }

    /* flush the state cache to physical chipset. */
    void flush() {
        for(uint8_t i = 0; i < pcs; ++i) {
            _ctl.xfer(_value[i]);
        }

        _ctl.latch();
    }

    /* read a pin from state cache. */
    pin_state_t read(uint8_t pin) {
        return (_value[pin / 8] & (1 << pin)) ? high : low;
    }

    /* write a pin into state cache or direct out. */
    g74hc595_t& write(uint8_t pin, pin_state_t state) {
        l74hc595_write_bit(_value, pin, state);

        if (!_ctl.state()) {
            flush();
        }

        return *this;
    }
};

/* 74HC595, 1 pcs. */
using l74hc595_t = g74hc595_t<1>;

#endif // __74HC595_H__