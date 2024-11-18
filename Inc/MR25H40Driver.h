#pragma once
#include "main.h"
#include "Spi.h"

class MR25H40
{

public:
    //Команды
    static constexpr uint16_t WREN_CMD =  0b00000110;
    static constexpr uint16_t READ_CMD =  0b00000011;
    static constexpr uint16_t WRITE_CMD = 0b00000010;

    //Размер частей сообщения в байтах
    static constexpr size_t CMD_SIZE =  2;
    static constexpr size_t ADR_SIZE =  3;
    //Структура Bureau с учетом выравниваний будет иметь размер 16 байт максимум
    static constexpr size_t DATA_SIZE = 16; 

    MR25H40();
    ~MR25H40() = default;

    uint16_t init();
    uint16_t transmit(Bureau_t* msg, uint32_t adr);
    uint16_t receive(Bureau_t* msg, uint32_t adr);

private:
    SPI* spi;
    void add2buf(uint8_t*, size_t&, void*, size_t);

    //Функции для работы с кодом Хэмминга
    void encode_hamming(uint8_t*, size_t, uint16_t*);
    void decode_hamming(uint16_t*, size_t, uint8_t*);
    uint8_t parity(uint16_t x);
};

