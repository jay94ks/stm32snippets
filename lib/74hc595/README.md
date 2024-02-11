## 74HC595 드라이버

74HC595 비트 시프트 레지스터 칩을 구동하는 드라이버입니다.

```
l74hc595_t bs(
    pin_t(GPIOA, GPIO_PIN_0),   // --> 데이터 핀 (SER): PA0
    pin_t(GPIOA, GPIO_PIN_1),   // --> 클록 핀 (SH_CP): PA1
    pin_t(GPIOA, GPIO_PIN_2)    // --> 래치 클록 핀 (ST_CP): PA2
);

// ...
// -- IO 최적화 모드

if (!bs.begin()) {
    return;
}

bs.write(0, high);  // --> 595의 0번 핀을 high로.
bs.write(1, high);  // --> 595의 1번 핀을 high로.

bs.write(2, low);   // --> 595의 2번 핀을 high로.
bs.end();           // --> 상태 캐쉬를 칩셋에 실제 반영함.

// ...
// -- Direct IO 모드
// --> begin, end 메서드를 호출하지 않아야 함.

bs.write(3, high);  // --> 595의 3번 핀을 high로.
bs.write(4, low);   // --> 595의 4번 핀을 low로.
```