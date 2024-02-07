## I2C (master) wrapper.
단순 HAL API wrapper 입니다.

```
i2cm_t i2c(&hi2c, 0x20);

// ....

uint8_t cmd = 0x12;

// --> 1 바이트 쓰기.
i2c.write(&cmd, 1);

// --> 1 바이트 읽기.
i2c.read(&cmd, 1);
```