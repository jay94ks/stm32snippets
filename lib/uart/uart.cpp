#include "uart.h"

class uart_intr {
public:

    /**
     * proxy ERROR interrupt call to UART instance.
     */
    static void proxy_error(huart_t huart) {
        auto uart = uart_base_t::root();
        while(uart) {
            if (uart->handle() != huart) {
                uart = uart->link();
                continue;
            } 

            uart->on_error();
            break;
        }
    }

    /**
     * proxy RX interrupt call to UART instance.
     */
    static void proxy_rx(huart_t huart) {
        auto uart = uart_base_t::root();
        while(uart) {
            if (uart->handle() != huart) {
                uart = uart->link();
                continue;
            } 

            uart->on_recv();
            break;
        }
    }
};

extern "C" {
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		uart_intr::proxy_rx(huart);
	}

	void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
		uart_intr::proxy_error(huart);
	}
}

// --
uart_base_t* uart_base_t::_root = nullptr;

uart_base_t::uart_base_t(huart_t uart)
    : _uart(uart), _intr(0), _link(nullptr)
{
}

bool uart_base_t::begin_intr(void* buf, uint32_t length) {
    if (_intr & INTR_EN) {
        return false;
    }

    _intr |= INTR_EN;
    _intr &= ~INTR_RX;
    _intr &= ~INTR_ERR;

    if (HAL_UART_Receive_IT(_uart, (uint8_t*) buf, length) != HAL_OK) {
        _intr &= ~INTR_EN;
        _intr |= INTR_ERR;
        return false;
    }

    return true;
}

bool uart_base_t::enable() {
    auto temp = _root;
    while(temp) {
        if (temp == this) {
            return false;
        }

        temp = temp->_link;
    }

    if (!_root) {
        _root = this;
    } 
    
    else {
        _link = _root;
        _root = this;
    }

    return true;
}

bool uart_base_t::disable() {
    if (_root == this) {
        _root = _link;
        _link = nullptr;
        return true;
    }

    auto temp = _root;
    while(temp) {
        if (temp->_link == this) {
            temp->_link = _link;
            _link = nullptr;
            return true;
        }

        temp = temp->_link;
    }

    return false;
}

bool uart_base_t::wait(uint32_t timeout) const {
    if (!intr_en()) {
        return false;
    }

    if (intr_in()) {
        return true;
    }

    uint32_t ticks = HAL_GetTick();
    while(true) {
        if (intr_rx()) {
            return true;
        }

        if (intr_err()) {
            return true;
        }

        uint32_t now = HAL_GetTick();
        if ((now - ticks) >= timeout) {
            return false;
        }
    }
}
