#ifndef __PIN_PIN_H__
#define __PIN_PIN_H__

// --> STM32CubeMX generated header.
#include "main.h"

// --> shortcut.
using gpio_t = GPIO_TypeDef*;
using pin_mask_t = uint16_t;
using pin_state_t = GPIO_PinState;

constexpr pin_state_t high = GPIO_PIN_SET;
constexpr pin_state_t low = GPIO_PIN_RESET;

/**
 * Describes a GPIO PIN reference.
 */
class pin_t {
private:
    gpio_t _port;
    pin_mask_t _pin;
    pin_state_t _state;

public:
    /**
     * initialize a new pin_t as `none` value.
     */
    pin_t();

    /**
     * initialize a new pin_t as `none` value, but with state.
     */
    pin_t(pin_state_t state);

    /**
     * initialize a new pin_t with `port` and `pin` mask.
     * usage: Pin(GPIOA, GPIO_PIN_4).
     */
    pin_t(gpio_t port, pin_mask_t pin);

public:
    /* get the port for this pin. */
    gpio_t port() const { return _port; }

    /* get the pin mask for this pin. */
    pin_mask_t pin() const { return _pin; }

public:
    /* read pin state.*/
    pin_state_t read() const;

    /* write pin state. */
    pin_t& write(pin_state_t state);

public:
    /* implicit conversion to boolean. */
    operator bool() const { return read() == GPIO_PIN_SET;}

    /* invert overator. */
    bool operator !() const { return read() == GPIO_PIN_RESET; }

    /* assignment operator. */
    pin_t& operator =(bool value) { return write(value ? GPIO_PIN_SET : GPIO_PIN_RESET); }
};

#endif // __PIN_H__