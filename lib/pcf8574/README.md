## PCF8574 드라이버

PCF8574 I2C IO extender 칩을 구동하는 드라이버 입니다.
`PCF8574T` 칩으로 테스트가 완료된 코드입니다.

```

pcf8574_t pcf_1(i2cm_t(&hi2c, 0x20));

// ....

// --> 입력/출력 타임아웃 설정 (이 코드는 무한 대기)
//   : 기본값은 10 ms로 초기화되어 있음.
pcf_1.timeout(0xffffffffu, 0xffffffffu); 

// --> 즉시 실행 모드.
pcf_1.write(1, true); // --> PCF8574 핀 1번을 high로 설정.
pcf_1.write(2, false); // --> PCF8574 핀 2번을 low로 설정.

// --> 트랜잭션 모드.
pcf_1.begin();

pcf_1.write(1, true); // --> PCF8574 핀 1번을 high로 설정.
pcf_1.write(2, false); // --> PCF8574 핀 2번을 low로 설정.

pcf_1.end();

```

### TODO: 인터럽트 벡터
인터럽트를 받는 코드는 작업을 하지 않았으나, 언젠가는 구현될 예정입니다.