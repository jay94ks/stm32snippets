#include "74hc595.h"

l74hc595_ctl_t::l74hc595_ctl_t(const pin_t& data, const pin_t& clock, const pin_t& latch)
    : _data(data), _clock(clock), _latch(latch), _state(0)
{
    _latch.write(low);
}

void l74hc595_ctl_t::xfer(uint8_t value) {
    for(uint8_t i = 0; i < 8; ++i) {
        pin_state_t state = (value & (1 << i)) ? high : low;

        _data.write(state);
        _clock.write(high);

        pin_t::delay();
        _clock.write(low);
    }
}

void l74hc595_ctl_t::latch() {
    _latch.write(high);

    pin_t::delay();
    _latch.write(low);
}

bool l74hc595_ctl_t::begin() {
    if (_state) {
        return false;
    }

    _state = 1;
    return true;
}

bool l74hc595_ctl_t::end() {
    if (!_state) {
        return false;
    }

    _state = 0;
    return true;
}

void l74hc595_write_bit(uint8_t* buf, uint8_t pin, pin_state_t state) {
    const uint8_t n = pin / 8;

    if (state) {
        buf[n] |= 1 << pin;
    }

    else {
        buf[n] &= ~(1 << pin);
    }
}