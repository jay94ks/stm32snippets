#include "w25qxx.h"

#ifdef W250XX_EXPLICIT_BLK
#define W25QXX_INIT_GUARD(ret) \
    if (!_init) { return ret; }
#else
#define W25QXX_INIT_GUARD(ret)
#endif

bool w25qxx_t::busy() const {
    uint8_t tx[2] = { 0x05, 0xa5 }; // --> read status 1
    uint8_t rx[2];

    select();
    if (_spi.wread(tx, rx, 2, 100) == false) {
        deselect();
        return true; // --> busy.
    }

    deselect();
    return (rx[1] & (1 << 0)) != 0;
}

bool w25qxx_t::wait_busy(uint32_t timeout) const {
    uint32_t ticks = HAL_GetTick();
    while(_spi) {
        if (busy() == false) {
            return true;
        }

        uint32_t now = HAL_GetTick();
        if ((now - ticks) >= timeout) {
            break;
        }
    }

    return false;
}

bool w25qxx_t::enwrite() {
    uint8_t cmd = 0x06;
    
    select();
    bool ret = _spi.write(&cmd, 1, 100);
    deselect();

    return ret;
}

bool w25qxx_t::diswrite() {
    uint8_t cmd = 0x04;
    
    select();
    bool ret = _spi.write(&cmd, 1, 100);
    deselect();

    return ret;
}

bool w25qxx_t::recognize() {
#ifdef W250XX_EXPLICIT_BLK
    if (_init) {
        return true;
    }
#else
    if (_block) {
        return true;
    }
#endif

    if (!diswrite()) {
        return false;
    }

    uint8_t tx[4] = { 0x9f, 0xff, 0xff, 0xff };
    uint8_t rx[4];

    select();
    if (!_spi.wread(tx, rx, 4, 100)) {
        deselect();
        return false;
    }

    deselect();
    uint8_t sz = rx[3] & 0x0f;

    /**
     * 0x10, 0x11, 0x12, ... 0x19
     * 1Mbit, 2Mbit, 4Mbit .... 256Mbit.
     * 0x20: 512Mbit.
     */
#ifdef W250XX_EXPLICIT_BLK
    uint32_t _block = 0;
#endif
    if ((rx[3] & 0xf0) != 0x10 || sz > 0x09) {
#if W25QXX_ENSZ_0x2X
        if (rx[3] != 0x20) {
            return false;
        }
        
        sz = 10;
#else
        return false;
#endif
    }
    
    _block = uint32_t(1) << (sz);
#ifdef W250XX_EXPLICIT_BLK
    if (_block != W250XX_EXPLICIT_BLK) {
        _init = 0;
        return false;
    }

    _init = 1;
#endif
    return true;
}

uint32_t w25qxx_t::read(uint32_t addr, void* buf, uint32_t len, uint32_t timeout) {
    W25QXX_INIT_GUARD(0);
    uint32_t max = max_addr();
    if (addr >= max) {
        return 0;
    }

    if (addr + len > max) {
        len = max - addr;
    }

    select();
    bool ret = false;

#if W250XX_EXPLICIT_BLK >= 512
    uint8_t tx[5] = { 0x13,
        uint8_t((addr >> 24) & 0xff),
        uint8_t((addr >> 16) & 0xff),
        uint8_t((addr >> 8) & 0xff),
        uint8_t((addr) & 0xff)
    };

    ret = _spi.write(tx, 5, 100);
#else
#if W25QXX_ENSZ_0x20
        if (_block >= 512) {
            uint8_t tx[5] = { 0x13,
                uint8_t((addr >> 24) & 0xff),
                uint8_t((addr >> 16) & 0xff),
                uint8_t((addr >> 8) & 0xff),
                uint8_t((addr) & 0xff)
            };

            ret = _spi.write(tx, 5, 100);
        }

        else {
#endif
        uint8_t tx[4] = { 0x03,
            uint8_t((addr >> 16) & 0xff),
            uint8_t((addr >> 8) & 0xff),
            uint8_t((addr) & 0xff)
        };

        ret = _spi.write(tx, 4, 100);
#if W25QXX_ENSZ_0x20
    }
#endif
#endif

    if (ret) {
        ret = _spi.read(buf, len, timeout);
    }

    deselect();
    return ret ? len : 0;
}

uint32_t w25qxx_t::write_pi(uint32_t page, uint32_t offset, const void* buf, uint32_t len) {
    W25QXX_INIT_GUARD(0);
    if (page >= max_page()) {
        return 0;
    }

    if (offset >= PAGE_SIZE) {
        return 0;
    }

    uint32_t addr = page * PAGE_SIZE;
    uint32_t max = PAGE_SIZE - offset;

    if (len > max) {
        len = max;
    }

    if (enwrite() == false) {
        return 0;
    }

    select();

    bool ret = false;
#if W250XX_EXPLICIT_BLK >= 512
    uint8_t tx[5] = { 0x12,
        uint8_t((addr >> 24) & 0xff),
        uint8_t((addr >> 16) & 0xff),
        uint8_t((addr >> 8) & 0xff),
        uint8_t((addr) & 0xff)
    };

    ret = _spi.write(tx, 5, 100);
#else
#if W25QXX_ENSZ_0x20
    if (_block >= 512) {
        uint8_t tx[5] = { 0x12,
            uint8_t((addr >> 24) & 0xff),
            uint8_t((addr >> 16) & 0xff),
            uint8_t((addr >> 8) & 0xff),
            uint8_t((addr) & 0xff)
        };

        ret = _spi.write(tx, 5, 100);
    }

    else {
#endif
        uint8_t tx[4] = { 0x02,
            uint8_t((addr >> 16) & 0xff),
            uint8_t((addr >> 8) & 0xff),
            uint8_t((addr) & 0xff)
        };

        ret = _spi.write(tx, 4, 100);
#if W25QXX_ENSZ_0x20
    }
#endif
#endif

    if (ret) {
        ret = _spi.write(buf, len);
    }
    
    if (ret) {
        wait_busy();
    }

    diswrite();
    deselect();
    return ret ? len : 0;
}

uint32_t w25qxx_t::write(uint32_t addr, const void* buf, uint32_t len, uint32_t timeout) {
	const uint8_t* cursor = (uint8_t*) buf;
    uint32_t ticks = HAL_GetTick();
    uint32_t slice;

	while (len > 0) {
		uint32_t page = addr / PAGE_SIZE;
		uint32_t offset = addr % PAGE_SIZE;

        if ((slice = write_pi(page, offset, cursor, len)) <= 0) {
            uint32_t now = HAL_GetTick();
            if ((now - ticks) >= timeout) {
                break; // --> timeout reached.
            }

            continue; // --> retry.
        }

		addr += slice;
		len -= slice;
        cursor += slice;
	}

	return uint32_t(cursor - (uint8_t*)buf);
}

bool w25qxx_t::erase() {
    W25QXX_INIT_GUARD(0);
    if (enwrite() == false) {
        return false;
    }

    select();
    uint8_t cmd = 0x60;
    bool ret = _spi.write(&cmd, 1, 100);
    deselect();

    if (ret) {
        wait_busy();
    }
    
    diswrite();
    return ret;
}

bool w25qxx_t::erase_sector(uint32_t sector) {
    W25QXX_INIT_GUARD(0);
    if (sector >= max_sector()) {
        return false;
    }

    if (enwrite() == false) {
        return false;
    }

    select();
    uint32_t addr = sector * SECTOR_SIZE;

    bool ret = false;
#if W250XX_EXPLICIT_BLK >= 512
    uint8_t tx[5] = { 0x21,
        uint8_t((addr >> 24) & 0xff),
        uint8_t((addr >> 16) & 0xff),
        uint8_t((addr >> 8) & 0xff),
        uint8_t((addr) & 0xff)
    };

    ret = _spi.write(tx, 5, 100);
#else
#if W25QXX_ENSZ_0x20
    if (_block >= 512) {
        uint8_t tx[5] = { 0x21,
            uint8_t((addr >> 24) & 0xff),
            uint8_t((addr >> 16) & 0xff),
            uint8_t((addr >> 8) & 0xff),
            uint8_t((addr) & 0xff)
        };
        
        ret = _spi.write(tx, 5, 100);
    }

    else {
#endif
	uint8_t tx[4] = { 0x20,
		uint8_t((addr >> 16) & 0xff),
		uint8_t((addr >> 8) & 0xff),
		uint8_t((addr) & 0xff)
	};

    ret = _spi.write(tx, 4, 100);
#if W25QXX_ENSZ_0x20
    }
#endif
#endif
    deselect();

    if (ret) {
        wait_busy();
    }
    
    diswrite();
    return ret;
}

bool w25qxx_t::erase_block(uint32_t block) {
    W25QXX_INIT_GUARD(0);
    if (block >= max_block()) {
        return false;
    }

    if (enwrite() == false) {
        return false;
    }

    select();
    uint32_t addr = block * BLOCK_SIZE;
	
    bool ret = false;
#if W250XX_EXPLICIT_BLK >= 512
    uint8_t tx[5] = { 0xdc,
        uint8_t((addr >> 24) & 0xff),
        uint8_t((addr >> 16) & 0xff),
        uint8_t((addr >> 8) & 0xff),
        uint8_t((addr) & 0xff)
    };

    ret = _spi.write(tx, 5, 100);
#else
#if W25QXX_ENSZ_0x20
    if (_block >= 512) {
        uint8_t tx[5] = { 0xdc,
            uint8_t((addr >> 24) & 0xff),
            uint8_t((addr >> 16) & 0xff),
            uint8_t((addr >> 8) & 0xff),
            uint8_t((addr) & 0xff)
        };

        ret = _spi.write(tx, 5, 100);
    }

    else {
#endif
	uint8_t tx[4] = { 0xd8,
		uint8_t((addr >> 16) & 0xff),
		uint8_t((addr >> 8) & 0xff),
		uint8_t((addr) & 0xff)
	};

    ret = _spi.write(tx, 4, 100);
#if W25QXX_ENSZ_0x20
    }
#endif
#endif
    deselect();

    if (ret) {
        wait_busy();
    }
    
    diswrite();
    return ret;
}
