#include "spi.h"

#if SPI_SUPPORT == 2
void spi_t::stop() {
    if (_dma) {
        HAL_SPI_DMAStop(_spi);
    }
}
#endif 

bool spi_t:: wait(uint32_t timeout) {
    uint32_t tick = HAL_GetTick();
    while(true) {
        if (ready()) {
            return true;
        }

        uint32_t now = HAL_GetTick();
        if ((now - tick) >= timeout) {
            return false;
        }
    }
}

#if SPI_SUPPORT != 0
bool spi_t::read(void* buffer, uint32_t len, uint32_t timeout) {
#if SPI_SUPPORT == 2    // --> both.
    if (_dma) {
#endif
        if (HAL_SPI_Receive_DMA(_spi, (uint8_t*) buffer, len) != HAL_OK) {
            return false;
        }

        if (wait(timeout)) {
            return true;
        }

        stop();
        return false;
#if SPI_SUPPORT == 2    // --> both.
    }
    
    return HAL_SPI_Receive(_spi, (uint8_t*) buffer, len, timeout) == HAL_OK;
#endif
}
#endif

#if SPI_SUPPORT != 0
bool spi_t::write(const void* buffer, uint32_t len, uint32_t timeout) {
#if SPI_SUPPORT == 2    // --> both.
    if (_dma) {
#endif
        if (HAL_SPI_Transmit_DMA(_spi, (uint8_t*) buffer, len) != HAL_OK) {
            return false;
        }

        if (wait(timeout)) {
            return true;
        }

        stop();
        return false;
#if SPI_SUPPORT == 2    // --> both.
    }
    
    return HAL_SPI_Transmit(_spi, (uint8_t*) buffer, len, timeout) == HAL_OK;
#endif
}
#endif

#if SPI_SUPPORT != 0
bool spi_t::wread(const void* write, void* read, uint32_t len, uint32_t timeout) {
#if SPI_SUPPORT == 2    // --> both.
    if (_dma) {
#endif
        if (HAL_SPI_TransmitReceive_DMA(_spi,  (uint8_t*) write, (uint8_t*) read, len) != HAL_OK) {
            return false;
        }

        if (wait(timeout)) {
            return true;
        }

        stop();
        return false;
#if SPI_SUPPORT == 2    // --> both.
    }

    return HAL_SPI_TransmitReceive(_spi, (uint8_t*) write, (uint8_t*) read, len, timeout) == HAL_OK;
#endif
}
#endif

#if SPI_SUPPORT == 2    // --> both.
bool spi_t::read_n(void* buffer, uint32_t len) {
    if (_dma) {
        return HAL_SPI_Receive_DMA(_spi, (uint8_t*) buffer, len) == HAL_OK;
    }
    
    return HAL_SPI_Receive(_spi, (uint8_t*) buffer, len, infinite) == HAL_OK;
}
#endif

#if SPI_SUPPORT == 2    // --> both.
bool spi_t::write_n(const void* buffer, uint32_t len) {
    if (_dma) {
        return HAL_SPI_Transmit_DMA(_spi, (uint8_t*) buffer, len) == HAL_OK;
    }

    return HAL_SPI_Transmit(_spi, (uint8_t*) buffer, len, infinite) == HAL_OK;
}
#endif

#if SPI_SUPPORT == 2    // --> both.
bool spi_t::wread_n(const void* write, void* read, uint32_t len) {
    if (_dma) {
        return HAL_SPI_TransmitReceive_DMA(_spi, (uint8_t*) write, (uint8_t*) read, len) == HAL_OK;
    }

    return HAL_SPI_TransmitReceive(_spi, (uint8_t*) write, (uint8_t*) read, len, infinite) == HAL_OK;
}
#endif