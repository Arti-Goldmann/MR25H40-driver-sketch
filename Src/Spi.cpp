#include "main.h"
#include "Spi.h"

const size_t SPI::BUF_SIZE;

void SPI::isr_handler_byte_transmit_done(void)
{   
    //Шлем TX и параллельно читаем RX
    *TX = txbuf[transmit_index];
    rxbuf[transmit_index++] = *RX;

    //Ну тут еще нужно подумать в какой момент RX считывать
    //Скорее всего тут нужно будет на одно прерывание сдвинуться
    
    if(transmit_index >= transmit_len)
    {
        busy = false;
        chip_select_disable();
        interrupt_disable();
    }
}

uint16_t SPI::transmit(uint16_t bytes_num)
{
    if(!busy && (bytes_num <= sizeof(txbuf)))
    {   
        transmit_len = bytes_num;
        transmit_index = 0;
        busy = true;
        
        chip_select_enable();
        interrupt_enable();
        return 1;
    }
    else
    {
        return 0;
    }
}

uint16_t SPI::is_busy(void)
{
    return busy;
}


SPI::SPI()
{ 
}
void SPI::init(){
}
void SPI::interrupt_enable(){
}
void SPI::interrupt_disable(){
}
void SPI::chip_select_enable(){
}
void SPI::chip_select_disable(){
}


SPI_portA::SPI_portA():
    SPI()
{

}


void SPI_portA::interrupt_enable()
{
    //Дергаем регистры для этого SPI
}

void SPI_portA::interrupt_disable()
{
    //Дергаем регистры для этого SPI
}

void SPI_portA::chip_select_enable()
{
    //Дергаем ножкой чип селекта
}
void SPI_portA::chip_select_disable()
{
    //Дергаем ножкой чип селекта
}

void SPI_portA::init()
{   
    //Настраиваем GPIO (TX, RX, CLK)
    //И чип селект
    //Настраиваем регистры для этого модуля SPI
    
    //Запомним адреса регисторов
    TX = NULL;
    RX = NULL;
    // ...
}