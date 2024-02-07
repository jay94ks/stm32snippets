## W25QXX 드라이버

W25QXX SPI 플래시 칩을 구동하는 드라이버 입니다.
`W25Q32VSIQ` 칩으로 테스트가 완료된 코드입니다.

```
w25qxx_t _flash(spi_t(&hspi), pin_t(GPIOA, GPIO_PIN_4));

// ... <중략> ...
// --> SPI 플래시의 첫 동작은 반드시 `recognize()` 메서드의 호출이어야 합니다.

if (!_flash.recognize()) {
    // --> 플래쉬 메모리 인식 실패.
    set_led_error(high);
    while(true);
}

uint32_t my_conf_uints[8];
uint32_t size = _flash.read(0x0000, my_conf_uints, sizeof(my_conf_uints));

if (size != sizeof(my_conf_uints)) {
    // --> 읽기 실패.
}

size = _flash.write(0x0000, my_conf_uints, sizeof(my_conf_uints));

if (size != sizeof(my_conf_uints)) {
    // --> 쓰기 실패.
}
```

### 512Mbit 이상 인식이 되지 않을때...
`w25qxx.h` 파일을 열어서, 다음 줄을 찾습니다.
```
// --> set 1 to use 512Mbit or over.
#ifndef W25QXX_ENSZ_0x20
#define W25QXX_ENSZ_0x20    0
#endif
```

여기서 `W25QXX_ENSZ_0x20`를 1로 바꿔주면 인식이 됩니다.
이 때, 불필요한 `if`문들이 활성화되므로, 아래 단락을 따라, 
크기를 고정시키는것을 추천합니다.

### MCU 펌웨어 플래시가 부족할때...

`w25qxx.h` 파일을 열어서, 주석처리된 다음 줄을 찾습니다.
```
// --> set this to use explicit implementation for specified size.
// #define W25QXX_EXPLICIT_MBIT  32  // --> 32 Mbit.
```

`W25QXX_EXPLICIT_MBIT`의 주석을 풀고, 비트 수를 작성한 후, 
컴파일하면 칩 인식시에 비트 수가 맞는지만 검사하고, 그 외의 블록 크기 검사 등을 전혀 수행하지 않게 됩니다.
따라서, 불필요한 코드가 일부 줄어들 수 있습니다.
