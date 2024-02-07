#ifndef __I2CM_H__
#define __I2CM_H__

// --> STM32CubeMX generated header.
#include "main.h"

// --> shortcut.
using hi2c_t = I2C_HandleTypeDef*;

/**
 * Describes an I2C interface.
 */
class i2cm_t {
private:
    hi2c_t _i2c;
    uint8_t _addr;
    uint8_t _dma;

public:
    /**
     * initialize a new I2C interface as null device.
     */
    i2cm_t()
        : _i2c(nullptr), _addr(0)
    {   
    }

    /**
     * initialize a new I2C interface using its handle and address.
     */
    i2cm_t(hi2c_t i2c, uint8_t addr)
        : _i2c(i2c), _addr(addr)
    {
    }
    
public:
    /**
     * for null check.
     */
    inline operator bool() const { return _i2c != nullptr; }
    inline bool operator !() const { return _i2c == nullptr; }

    /**
     * get the internal I2C handle.
     */
    inline hi2c_t i2c() const { return _i2c; }

    /**
     * get the internal I2C address.
     */
    inline uint8_t addr() const { return _addr; }

    /**
     * write bytes through I2C interface.
     */
    inline bool write(const void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return HAL_I2C_Master_Transmit(_i2c, (_addr << 1) | 0, (uint8_t*) buf, len, timeout) == HAL_OK;
    }

    /**
     * read bytes from I2C interface.
     */
    inline bool read(void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return HAL_I2C_Master_Receive(_i2c, (_addr << 1) | 1, (uint8_t*) buf, len, timeout) == HAL_OK;
    }
};
#endif // __I2CM_H__