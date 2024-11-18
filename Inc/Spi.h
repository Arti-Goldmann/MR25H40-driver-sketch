#pragma once
#include "main.h"

class SPI
{

public:
    
    SPI();
    ~SPI() = default;
    
    virtual void init();
    uint16_t transmit(uint16_t);
    uint16_t is_busy();
    
    static constexpr size_t BUF_SIZE =  50;
    uint8_t txbuf[BUF_SIZE];
    uint8_t rxbuf[BUF_SIZE];
    size_t  buf_index;
    
protected:
    virtual void interrupt_enable();
    virtual void interrupt_disable();

    virtual void chip_select_enable();
    virtual void chip_select_disable();

    uint16_t busy = {false};

    uint8_t* data;
    uint16_t transmit_len;
    uint16_t transmit_index;

    uint16_t* TX; //указатель на регистр TX
    uint16_t* RX; //указатель на регистр RX

    void isr_handler_byte_transmit_done(void);
};

class SPI_portA : public SPI
{
public:
    SPI_portA();
    ~SPI_portA() = default;

    void init();

public:
    void interrupt_enable();
    void interrupt_disable();

    void chip_select_enable();
    void chip_select_disable();
};