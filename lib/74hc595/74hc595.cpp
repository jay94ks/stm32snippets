#include "74hc595.h"

l74hc595_t::l74hc595_t(pin_t data, pin_t clock, pin_t latch)
    : _data(data), _clock(clock), _latch(latch),
        _state(0), _value(0)
{
}

bool l74hc595_t::begin() {
    if (_state) {
        return false;
    }

    _state = 1;
    return true;
}

void l74hc595_t::end(bool flush) {
    if (!_state) {
        return;
    }

    _state = 0;
    if (flush) {
        this->flush();
    }
}

void l74hc595_t::flush() {
    select();
    _latch.write(low);

    for(uint8_t i = 0; i < 8; ++i) {
        pin_state_t state = (_value & (1 << i)) ? high : low;

        _data.write(state);
        _clock.write(high);

        pin_t::delay();
        _clock.write(low);
    }

    _latch.write(high);
    deselect();
}

pin_state_t l74hc595_t::read(uint8_t pin) {
    return (_value & (1 << pin)) ? high : low;
}

l74hc595_t& l74hc595_t::write(uint8_t pin, pin_state_t state) {
    if (state) {
        _value |= 1 << pin;
    }

    else {
        _value &= ~(1 << pin);
    }

    if (!_state) {
        flush();
    }

    return *this;
}