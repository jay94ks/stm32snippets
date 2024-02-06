#include "Pin.h"

pin_t::pin_t()
    : pin_t(0)
{
}

pin_t::pin_t(pin_state_t state)
    : pin_t(nullptr, 0), _state(state)
{
}

pin_t::pin_t(gpio_t port, pin_mask_t pin)
    : _port(port), _pin(pin)
{
}

pin_state_t pin_t::read() const {
    if (_port) {
        return HAL_GPIO_ReadPin(_port, _pin);
    }

    return _state;
}

pin_t& pin_t::write(pin_state_t state) {
    if (_port) {
        HAL_GPIO_WritePin(_port, _pin, state);
    }

    _state = state;
    return *this;
}

