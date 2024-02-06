#ifndef __SPI_H__
#define __SPI_H__

// --> STM32CubeMX generated header.
#include "main.h"

// --> SPI support type. this will remove useless code permanently.
//   : 0: HAL only, 1: DMA only, 2: bothway.
#ifndef SPI_SUPPORT
#define SPI_SUPPORT     2
#endif

// --> to remove DMA fields permanently.
#if SPI_SUPPORT == 2
#define SPI_SUPPORT_FILTER(...) __VA_ARGS__
#else
#define SPI_SUPPORT_FILTER(...)
#endif

// --> shortcut.
using hspi_t = SPI_HandleTypeDef*;

/**
 * Describes a SPI communication port.
 */
class spi_t {
private:
    hspi_t _spi;

#if SPI_SUPPORT == 2
    bool _dma;
#endif 

public:
    /**
     * initialize a new SPI interface using its handle.
     */
    spi_t(hspi_t spi, bool dma = false)
        : _spi(spi) SPI_SUPPORT_FILTER(, _dma(dma))
    {
    }

public:
    /**
     * infinite timeout.
     */
    static constexpr uint32_t infinite = 0xffffffffu;

    /**
     * get the internal SPI handle.
     */
    inline hspi_t spi() const { return _spi; }

    /**
     * indicates whether the SPI instance uses DMA or not.
     */
#if SPI_SUPPORT == 2
    inline bool use_dma() const { return _dma; }
#else
    inline bool use_dma() const { return (SPI_SUPPORT) == 1; }
#endif

    /**
     * test whether the SPI interface is ready or not.
     */
    inline bool ready() { 
        return HAL_SPI_GetState(_spi) == HAL_SPI_STATE_READY; 
    }

#if SPI_SUPPORT == 2
    /**
     * stop the previously requested operation.
     */
    void stop();
#elif SPI_SUPPORT == 1
    inline void stop() { HAL_SPI_DMAStop(_spi); }
#else 
    inline void stop() { }
#endif 
    /**
     * wait for the SPI interface to be ready.
     * returns true if its ready before timeout.
     */
    bool wait(uint32_t timeout = infinite);

    /**
     * read bytes from the target device.
     * returns true if success. note that, if timeout reached, this will stop the request.
     */
#if SPI_SUPPORT != 0
    bool read(void* buffer, uint32_t len, uint32_t timeout = infinite);
#else
    inline bool read(void* buffer, uint32_t len, uint32_t timeout = infinite) {
        return HAL_SPI_Receive(_spi, (uint8_t*) buffer, len, timeout) == HAL_OK;
    }
#endif

    /**
     * write bytes to the target device.
     * returns true if success. note that, if timeout reached, this will stop the request.
     */
#if SPI_SUPPORT != 0
    bool write(const void* buffer, uint32_t len, uint32_t timeout = infinite);
#else
    inline bool write(const void* buffer, uint32_t len, uint32_t timeout = infinite) {
        return HAL_SPI_Transmit(_spi, (uint8_t*) buffer, len, timeout) == HAL_OK;
    }
#endif
    
    /**
     * write bytes to the target device, and then read bytes from the target device.
     * returns true if success. note that, if timeout reached, this will stop the request.
     */
#if SPI_SUPPORT != 0
    bool wread(const void* write, void* read, uint32_t len, uint32_t timeout = infinite);
#else
    inline bool wread(const void* write, void* read, uint32_t len, uint32_t timeout = infinite) {
        return HAL_SPI_TransmitReceive(_spi, (uint8_t*) write, (uint8_t*) read, len, timeout) == HAL_OK;
    }
#endif
    /**
     * read bytes from the target device.
     * returns true if success but, the request was not completed.
     * if this returns false, it means that SPI interface is not ready to work.
     * and, this will not stop even if timeout reached.
     */
#if SPI_SUPPORT == 2
    bool read_n(void* buffer, uint32_t len);
#elif SPI_SUPPORT == 1
    inline bool read_n(void* buffer, uint32_t len) {
        return HAL_SPI_Receive_DMA(_spi, (uint8_t*) buffer, len) == HAL_OK;
    }
#else
    inline bool read_n(void* buffer, uint32_t len) {
        return HAL_SPI_Receive(_spi, (uint8_t*) buffer, len, infinite) == HAL_OK;
    }
#endif

    /**
     * write bytes to the target device.
     * returns true if success but, the request was not completed.
     * if this returns false, it means that SPI interface is not ready to work.
     * and, this will not stop even if timeout reached.
     */
#if SPI_SUPPORT == 2
    bool write_n(const void* buffer, uint32_t len);
#elif SPI_SUPPORT == 1
    inline bool write_n(const void* buffer, uint32_t len) {
        return HAL_SPI_Transmit_DMA(_spi, (uint8_t*) buffer, len) == HAL_OK;
    }
#else
    inline bool write_n(const void* buffer, uint32_t len) {
        return HAL_SPI_Transmit(_spi, (uint8_t*) buffer, len, infinite) == HAL_OK;
    }
#endif

    /**
     * write bytes to the target device, and then read bytes from the target device.
     * returns true if success but, the request was not completed.
     * if this returns false, it means that SPI interface is not ready to work.
     * and, this will not stop even if timeout reached.
     */
#if SPI_SUPPORT == 2
    bool wread_n(const void* write, void* read, uint32_t len);
#elif SPI_SUPPORT == 1
    inline bool wread_n(const void* write, void* read, uint32_t len) {
        return HAL_SPI_TransmitReceive_DMA(_spi, (uint8_t*) write, (uint8_t*) read, len) == HAL_OK;
    }
#else
    inline bool wread_n(const void* write, void* read, uint32_t len) {
        return HAL_SPI_TransmitReceive(_spi, (uint8_t*) write, (uint8_t*) read, len, infinite) == HAL_OK;
    }
#endif
};




#endif // __SPI_H__