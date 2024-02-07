#ifndef __UART_H__
#define __UART_H__

#include "main.h"

// --> shortcut.
using huart_t = UART_HandleTypeDef*;

// --> forward decl: uart.cpp.
class uart_intr;

/**
 * Describes an UART communication port.
 * warning: do not copy this instance anyway.
 * because, the interrupt will not be passed to last initialized instance.
 */
class uart_base_t {
    friend class uart_intr;

private:
    huart_t _uart;
    uint8_t _intr;

private:
    static constexpr uint8_t INTR_EN = 0x80;
    static constexpr uint8_t INTR_IN = 0x40;

    static constexpr uint8_t INTR_RX = 0x01;
    static constexpr uint8_t INTR_ERR = 0x02;

protected:
    static uart_base_t* _root;
    uart_base_t* _link;

public:
    /**
     * initialize an uart_t using its handle.
     */
    uart_base_t(huart_t uart);

    /**
     * de-initialize the uart_t.
     * this will unlink self from root.
     */
    virtual ~uart_base_t() { disable(); }

protected:
    /* get the root instance. */
    inline static uart_base_t* root() { return _root; }

    /* get the linked instance. */
    inline uart_base_t* link() const { return _link; }
    
protected:
    /**
     * called when `error` interrupt occurred.
     */
    virtual void on_error() {
        _intr |= INTR_ERR | INTR_IN; 

        if (_intr & INTR_EN) {
            _intr &= ~INTR_EN;
            
            on_read_intr();
        }

        _intr &= ~INTR_IN;
    }

    /**
     * (abstract) called when `recv` interrupt occurred.
     * do not do complex something in this method.
     */
    virtual void on_recv() { 
        _intr |= INTR_RX | INTR_IN; 

        if (_intr & INTR_EN) {
            _intr &= ~INTR_EN;

            on_read_intr();
        }

        _intr &= ~INTR_IN;
    }
    
protected:
    /**
     * begin the interrupt based recv and enable interrupt.
     * call this from child class that inherited this class.
     */
    bool begin_intr(void* buf, uint32_t length);

    /**
     * called from interrupt to renew buffer.
     * this must call `begin_intr` to receive more bytes.
     */
    virtual void on_read_intr() { }

public:
    /**
     * get the handle.
     */
    inline huart_t handle() const { return _uart; }

    /* test whether the UART instance is running under interrupt mode or not. */
    inline bool intr_en() const { return _intr & INTR_EN; }

    /* test whether the RX interrupt triggered or not. */
    inline bool intr_rx() const { return _intr & INTR_RX; }

    /* test whether the ERR interrupt triggered or not. */
    inline bool intr_err() const { return _intr & INTR_ERR; }

    /* test whether the code is running under interrupt or not. */
    inline bool intr_in() const { return _intr & INTR_IN; }

    /**
     * enable this instance.
     * actually, link this instance to root.
     */
    virtual bool enable();

    /**
     * disable this instance.
     * actually, unlink this instance from root.
     */
    virtual bool disable();

    /**
     * wait for RX/ERR interrupt.
     */
    bool wait(uint32_t timeout = 0xffffffffu);

    /**
     * write the buffer through UART port.
     * returns false if timeout reached or error.
     */
    inline bool write_b(const void* buf, uint32_t length, uint32_t timeout = 0xffffffffu) {
        return HAL_UART_Transmit(_uart, (uint8_t*) buf, length, timeout);
    }

};


#endif // __UART_H__
