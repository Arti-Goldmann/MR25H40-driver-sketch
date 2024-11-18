#include "main.h"
#include "Spi.h"

void SPI::isr_handler_byte_transmit_done(void)
{   
    if(transmit)
    {   //Передаем данные
        *TXBUF = data[transmit_index++];
    }
    else
    {   //Читаем
        *TXBUF = data[transmit_index++]; //Выставляем клоки
        data[transmit_index++] = *RXBUF;
        //Ну тут конечно нужно еще подумать, на каком прерывании в RX появятся нужные
        //данные а не мусор... но суть такая
    }
    
    if(transmit_index >= transmit_len)
    {
        busy = false;
        chip_select_disable();
        interrupt_disable();
    }
}

uint16_t SPI::tx(uint8_t* tx_buffer, uint16_t tx_len)
{
    if(!busy)
    {   
        data = tx_buffer;
        transmit_len = tx_len;
        transmit_index = 0;
        busy = true;
        transmit = true; //Передаем
        
        chip_select_enable();
        interrupt_enable();
        return 1;
    }
    else{
        return 0;
    }

}

uint16_t SPI::rx(uint8_t* rx_buffer, uint16_t rx_len)
{
    if(!busy)
    {   
        data = rx_buffer;
        transmit_len = rx_len;
        transmit_index = 0;
        busy = true;
        transmit = false; //Читаем
        
        chip_select_enable();
        interrupt_enable();
        return 1;
    }
    else{
        return 0;
    }

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


SPI_A::SPI_A():
    SPI()
{

}


void SPI_A::interrupt_enable()
{
    //Дергаем регистры для этого SPI
}

void SPI_A::interrupt_disable()
{
    //Дергаем регистры для этого SPI
}

void SPI_A::chip_select_enable()
{
    //Дергаем ножкой чип селекта
}
void SPI_A::chip_select_disable()
{
    //Дергаем ножкой чип селекта
}

void SPI_A::init()
{   
    //Настраиваем GPIO (TX, RX, CLK)
    //И чип селект
    //Настраиваем регистры для этого модуля SPI
    
    //Запомним адреса регисторов
    TXBUF = NULL;
    RXBUF = NULL;
    // ...
}