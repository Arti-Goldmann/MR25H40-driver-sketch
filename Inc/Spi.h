#pragma once
#include "main.h"

class SPI
{

public:
    SPI();
    ~SPI() = default;

    virtual void init();

    uint16_t tx(uint8_t*, uint16_t);
    uint16_t rx(uint8_t*, uint16_t);

    uint16_t busy = {false};
    
protected:
    virtual void interrupt_enable();
    virtual void interrupt_disable();

    virtual void chip_select_enable();
    virtual void chip_select_disable();

    uint8_t* data;
    uint16_t transmit_len;
    uint16_t transmit_index;
    uint16_t transmit;

    uint16_t* TXBUF; //указатель на регистр TX
    uint16_t* RXBUF; //указатель на регистр RX

    void isr_handler_byte_transmit_done(void);
};

class SPI_A : public SPI
{
public:
    SPI_A();
    ~SPI_A() = default;

    void init();

public:
    void interrupt_enable();
    void interrupt_disable();

    void chip_select_enable();
    void chip_select_disable();
};