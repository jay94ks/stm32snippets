## SPI wrapper.

SPI 통신 HAL API를 단순화 시켜주는 wrapper입니다.
처음 이 파일들을 복사하면, `spi.h` 파일을 열어서 `SPI_SUPPORT` 전처리기를 수정하거나,
`main.h` 파일에서 `SPI_SUPPORT` 전처리기를 선언해주시면 됩니다.

```
// --> SPI support type. this will remove useless code permanently.
//   : 0: HAL only, 1: DMA only, 2: bothway.
#ifndef SPI_SUPPORT
#define SPI_SUPPORT     2
#endif
```

이렇게 되어 있는데, 

* 0: HAL API만 사용 (항상 블로킹)
* 1: DMA API만 사용 (블로킹/비동기)
* 2: 양쪽 모두 사용 (생성자의 `dma` 페러미터로 제어)

이므로, 적절하게 변경하시면 되고, 당연하게도 `2`번을 지정하면
플래시 메모리 공간을 조금 더 많이 사용하게 됩니다.

### 사용 방법
```
spi_t spi1(&hspi1, true); // --> SPI1을 DMA 모드로 사용.

// ...

int identify_spi_flash() {
	uint8_t tx[4] = { 0x9f, 0xff, 0xff, 0xff };
	uint8_t rx[4];

    spi1.wread(tx, rx, 4, spi_t::infinite);

    if ((rx[3] & 0xf0) != 0x10) {
        return -1; // --> error.
    }

    switch(rx[3] & 0x0f) {
        case 0x01: return 1; // 1 mbit.
        case 0x02: return 2; // 2 mbit.
        // ... <생략> ...
    }

    return -1; // --> not supported flash.
}
```

### `_n`이 붙은 함수들은
비 블로킹 함수입니다. 다만, HAL 모드에서는 항상 블로킹 동작이 됩니다.
사용 방법은 다음과 같습니다.
```
if (_spi_write_pending) {
    if (spi1.ready()) {
        _spi_write_pending = 0;
    }

    // --> 또는, spi1.wait() 함수를 호출하여 블로킹 상태로 진입할수 있음.
    return;
}

if (spi1.write_n(buf, sizeof(buf))) {
    _spi_write_pending = 1;
}
```