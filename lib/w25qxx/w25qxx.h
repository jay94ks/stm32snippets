#ifndef __W25QXX_H__
#define __W25QXX_H__

// --> SPI wrapper.
#include "../spi/spi.h"

// --> GPIO wrapper.
#include "../pin/pin.h"

// --> set 1 to use 512Mbit or over.
#ifndef W25QXX_ENSZ_0x20
#define W25QXX_ENSZ_0x20    0
#endif

// --> set this to use explicit implementation for specified size.
// #define W25QXX_EXPLICIT_MBIT  32  // --> 32 Mbit.

// --> alter W25QXX_ENSZ_0x20 and set W250XX_EXPLICIT_BLK
#ifdef W25QXX_EXPLICIT_MBIT
#if W25QXX_EXPLICIT_MBIT >= 512
#undef W25QXX_ENSZ_0x20
#define W25QXX_ENSZ_0x20    1
#endif
#define W250XX_EXPLICIT_BLK (W25QXX_EXPLICIT_MBIT) * 2
#define W250XX_SWITCH_INIT(a, b)    b
#else 
#ifdef W250XX_EXPLICIT_BLK
#undef W250XX_EXPLICIT_BLK
#endif
#define W250XX_SWITCH_INIT(a, b)    a
#endif

/**
 * Describes a SPI Flash memory.
 */
class w25qxx_t {
public:
	static constexpr uint32_t PAGE_SIZE = 0x100;
	static constexpr uint32_t SECTOR_SIZE = 0x1000;
	static constexpr uint32_t BLOCK_SIZE = 0x10000;

private:
    mutable spi_t _spi;
    mutable pin_t _cs;

#ifdef W250XX_EXPLICIT_BLK
    uint8_t _init;
#else 
    uint32_t _block; // --> block size.
#endif

public:
    /**
     * initialize a w25qxx_t using SPI and CS pin.
     */
    w25qxx_t(const spi_t& spi, const pin_t& cs = pin_t())
        : _spi(spi), _cs(cs), W250XX_SWITCH_INIT(_block(0), _init(0))
    {   
    }

protected:
    /* chip select. */
    virtual void select() const { _cs.write(low); pin_t::delay(); }

    /* chip deselect. */
    virtual void deselect() const { _cs.write(high); pin_t::delay(); }
    
public:
    /* test whether the chip is busy or not. */
    bool busy() const;

#ifdef W250XX_EXPLICIT_BLK
    /* size in bytes. */
    constexpr uint32_t max_addr() const { return W250XX_EXPLICIT_BLK * BLOCK_SIZE; }

	/* max page. */
	constexpr uint32_t max_page() const { return (W250XX_EXPLICIT_BLK * BLOCK_SIZE) / PAGE_SIZE; }

	/* max sector. */
	constexpr uint32_t max_sector() const { return (W250XX_EXPLICIT_BLK * BLOCK_SIZE) / SECTOR_SIZE; }
    
	/* max block. */
    constexpr uint32_t max_block() const { return W250XX_EXPLICIT_BLK; }
#else
    /* size in bytes. */
    uint32_t max_addr() const { return _block * BLOCK_SIZE; }

	/* max page. */
	uint32_t max_page() const { return max_addr() / PAGE_SIZE; }

	/* max sector. */
	uint32_t max_sector() const { return max_addr() / SECTOR_SIZE; }

	/* max block. */
	uint32_t max_block() const { return _block; }
#endif

    /**
     * wait for the chip to be idle.
     */
    bool wait_busy(uint32_t timeout = 0xffffffffu) const;

    /* enable write. */
    bool enwrite();

    /* disable write. */
    bool diswrite();

    /**
     * recognize the chip size in bytes.
     * this method must be called at first-time.
     * this can recognize maximum 256 Mbit, aka 32Mbytes.
     */
    bool recognize();

    /**
     * read bytes from specified address.
     */
    uint32_t read(uint32_t addr, void* buf, uint32_t len, uint32_t timeout = 0xffffffffu);

    /**
     * read bytes from the specified page.
     * this can read over page boundary.
     */
    inline bool read_p(uint32_t page, void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return read(page * PAGE_SIZE, buf, len, timeout);
    }

    /**
     * read bytes from the specified sector.
     * this can read over sector boundary.
     */
    inline bool read_s(uint32_t sector, void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return read(sector * SECTOR_SIZE, buf, len, timeout);
    }

    /**
     * read bytes from the specified block.
     * this can read over block boundary.
     */
    inline bool read_b(uint32_t block, void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return read(block * BLOCK_SIZE, buf, len, timeout);
    }

    /**
     * write bytes into specified address and returns written bytes.
     * note that, timeout is only for `retry`.
     */
    uint32_t write(uint32_t addr, const void* buf, uint32_t len, uint32_t timeout = 0xffffffffu);

private:
    /**
     * (internal-only) write bytes into specified page and its offset.
     * this returns written bytes.
     */
    uint32_t write_pi(uint32_t page, uint32_t offset, const void* buf, uint32_t len);

public:
    /**
     * write bytes into the specified page.
     * this can write over page boundary.
     */
    inline bool write_p(uint32_t page, const void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return write(page * PAGE_SIZE, buf, len, timeout);
    }

    /**
     * write bytes into the specified sector.
     * this can write over sector boundary.
     */
    inline bool write_s(uint32_t sector, const void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return write(sector * SECTOR_SIZE, buf, len, timeout);
    }

    /**
     * write bytes into the specified block.
     * this can write over block boundary.
     */
    inline bool write_b(uint32_t block, const void* buf, uint32_t len, uint32_t timeout = 0xffffffffu) {
        return write(block * BLOCK_SIZE, buf, len, timeout);
    }

    /**
     * erase entire chip.
     */
    bool erase();

    /**
     * erase a sector.
     */
    bool erase_sector(uint32_t sector);

    /**
     * erase a block.
     */
    bool erase_block(uint32_t block);
};

#endif // __W25QXX_H__
