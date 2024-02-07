## UART wrapper.

UART 통신 HAL API를 단순화 시켜주는 wrapper입니다.

```
class my_uart_port : public uart_base_t {
public:
    my_uart_port(huart_t uart) : uart_base_t(uart), _started(0) { }

private:
    uint8_t _buf[255];
    uint8_t _started;

protected:
    virtual void on_read_intr() {
        if (!_started) {
            return;
        }

        /* 여기서 수신받은 버퍼를 확인. */

        begin_intr(_buf, sizeof(_buf)); // --> 수신작업 재개.
    }

public:
    /* UART 인터럽트 수신을 시작합니다. */
    virtual bool enable() override {
        if (uart_base_t::enable()) {
            // --> 수신 시작. _started를 true로 바꾸기 전에 버퍼 리셋이 필요하면 수행.
            _started = true;
            begin_intr(_buf, sizeof(_buf));
            return true;
        }

        return false;
    }

    /* UART 인터럽트 수신을 중지합니다. */
    virtual bool disable() override {
        if (uart_base_t::disable()) {
            _started = false;
            // --> 수신 끝. 여기서 마무리 처리가 필요하면 수행.
            return true;
        }

        return false;
    }
};
```

### 메서드별 prefix/postfix의 의미

1. intr_* --> 인터럽트 플래그를 검사하지만, STM32 자체에서 제공하는 플래그가 아닌, 재가공된 버전입니다.
2. `*_b` --> 블로킹 함수입니다.
3. 그 외에는 비 블로킹 함수이거나, 함수 명 자체가 기능을 암시하는 명칭인 경우엔 생략합니다.
(예를들어 `wait` 메서드는 단어 자체가 암시적으로 블로킹 되는 동작이라는게 내포됨)

### 핸들러 밖에서 데이터를 수신처리 하려면...
`wait` 메서드와 `begin_intr` 메서드를 활용합니다.
이 방식으로 구현하려면, `on_read_intr` 메서드를 구현하지 않거나
`on_read_intr` 메서드 내부에서 begin_intr 메서드를 호출하지 않아야 합니다.

```
class my_uart_port : public uart_base_t {
    // ... <중략> ...

protected:
    virtual void on_read_intr() {
        // begin_intr(_buf, sizeof(_buf)); --> 호출하지 않아야
        // `wait` 메서드가 INTR_RX 플래그를 검사할 수 있습니다.
    }

public:
    /* 클래스 밖에서 호출 할 수 있도록 노출시킵니다. */
    inline bool begin_rx() {
        return begin_intr(_buf, sizeof(_buf));
    }

    // ... <생략> ...
};
```

이렇게 구현하면 아래와 같은 활용이 가능합니다.

```
my_uart_port my_uart;


my_uart.enable();
while(true) {
    my_uart.begin_rx(); // --> 내부에서 INTR_EN 플래그를 검사하므로 별도 검사 불필요.

    // 방법 1 --> 인터럽트 플래그를 직접 확인 (블로킹 회피)
    if (my_uart.intr_rx()) {
        // --> 데이터 준비됨.
        my_uart.write_b("okay, good!\n", 11);
    }

    // 방법 2 --> 블로킹 동작을 수행.
    if (my_uart.wait()) {
        // --> 데이터 준비됨.
        my_uart.write_b("okay, good!\n", 11);
    }

}

```