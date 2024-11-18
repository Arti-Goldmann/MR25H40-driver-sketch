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

    MR25H40(SPI*);
    ~MR25H40() = default;

    void init();
    uint16_t transmit(Bureau_t* msg, uint16_t adr);
    uint16_t receive(Bureau_t* msg, uint16_t adr);

private:

    //Сколько за раз можно отправить/получить байт
    static constexpr size_t MAX_MSG_BYTES_SIZE = 100;
    uint8_t spi_buf[MAX_MSG_BYTES_SIZE];

    SPI* spi_handle;
    void add2buf(uint8_t*, size_t&, void*, size_t);

    //Функции для работы с кодом Хэмминга
    void encode_hamming(uint8_t *data, size_t len, uint16_t *encoded);
    void decode_hamming(uint16_t *encoded, size_t len, uint8_t *decoded);
    uint8_t parity(uint16_t x);
};

