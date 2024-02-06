#ifndef __PIN_PORT_H__
#define __PIN_PORT_H__

// --> pin library.
#include "../pin/pin.h"

/* pin ID type (custom). */
typedef struct { uint32_t _; } pin_id_t;

/**
 * GPIO port informations.
 */
struct gpio {
    struct base_type { };

#define DEF_GPIO_TYPE(letter, target, n) \
    struct letter : public base_type { \
        static constexpr uint32_t base = (target);\
        static constexpr uint32_t index = n; \
        const uint8_t pin; \
        constexpr letter(uint8_t pin) : pin(pin) { } \
    };

#ifdef GPIOA_BASE
DEF_GPIO_TYPE(a, GPIOA_BASE, 0);
#endif
#ifdef GPIOB_BASE
DEF_GPIO_TYPE(b, GPIOB_BASE, 1);
#endif
#ifdef GPIOC_BASE
DEF_GPIO_TYPE(c, GPIOC_BASE, 2);
#endif
#ifdef GPIOD_BASE
DEF_GPIO_TYPE(d, GPIOD_BASE, 3);
#endif
#ifdef GPIOE_BASE
DEF_GPIO_TYPE(e, GPIOE_BASE, 4);
#endif
#ifdef GPIOF_BASE
DEF_GPIO_TYPE(f, GPIOF_BASE, 5);
#endif

    /**
     * write pin state to port.
     * e.g. gpio::write<gpio::a>(0, GPIO_PIN_SET);
     *  --> write gpio A0 pin.
     */
	template<typename T>
	static void write(uint8_t pin, pin_state_t state) {
		HAL_GPIO_WritePin((gpio_t) T::base, 1 << pin, state);
	}

    /**
     * read pin state from port.
     * e.g. gpio::read<gpio::a>(0);
     *  --> read gpio A0 pin.
     */
    template<typename T>
    static pin_state_t read(uint8_t pin) {
        return HAL_GPIO_ReadPin((gpio_t) T::base, 1 << pin);
    }

    /* get base address from pin id. */
    static gpio_t base(pin_id_t id) {
        return (gpio_t)(id._ & (~0x000000ffu));
    }

    /* get pin number from pin id. */
    static uint8_t pin(pin_id_t id) {
        return id._ & 0xff;
    }

    /**
     * make pin id for pin.
     * usage: gpio::id(gpio::a(1));
     *  --> returns ID of gpio pin for A1.
     */
    template<typename T>
    static pin_id_t id(T pin);
};

/**
 * GPIO pin type trait.
 * Test whether the type, `T`, is GPIO port type or not in compile-time.
 */
template<typename T>
struct is_gpio_pin_t {
    struct yes_type { int _; };
    struct no_type { char _; };

    static yes_type test(gpio::base_type*);
    static no_type test(...);

    static constexpr bool value = sizeof(decltype(test((T*)0))) == sizeof(yes_type);
};

/**
 * GPIO pin type trait.
 * Test whether the type, `T`, is GPIO port type or not in compile-time.
 */
template<typename T>
constexpr bool is_gpio_pin = is_gpio_pin_t<T>::value;

/* impl of gpio::id(...); */
template<typename T>
pin_id_t gpio::id(T pin) {
    static_assert(is_gpio_pin<T>, "the specified type, `T` is not GPIO type.");
    return { T::base | pin.pin };
}
#endif