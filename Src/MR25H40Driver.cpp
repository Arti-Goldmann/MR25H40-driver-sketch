#include "main.h"
#include "MR25H40Driver.h"

const uint16_t MR25H40::WREN_CMD;
const uint16_t MR25H40::READ_CMD;
const uint16_t MR25H40::WRITE_CMD;

MR25H40::MR25H40(SPI* spi):
    //Будем считать, что регистры SPI проинициализировали в другом месте
    //и нам дали структуру/класс со всем необходимым
    spi_handle(spi)
{

}

void MR25H40::init(void)
{  
    size_t spi_buf_index = 0;
    //Разрешаем запись данных
    add2buf(spi_buf, spi_buf_index, (void*)&WREN_CMD, sizeof(WREN_CMD));
    
    spi_handle->tx(spi_buf, (spi_buf_index + 1));
}

uint16_t MR25H40::transmit(Bureau_t* msg, uint16_t adr)
{
    size_t spi_buf_index = 0;
    add2buf(spi_buf, spi_buf_index, (void*)&WRITE_CMD, sizeof(WRITE_CMD));
    add2buf(spi_buf, spi_buf_index, (void*)&adr, sizeof(adr));

    //Разобьем Структуру Bureau на байты
    uint8_t temp_data[16] = {0}; //16 байт должно хватить
    size_t data_index = 0;

    add2buf(temp_data, data_index, (void*)msg, sizeof(msg));

    //Для обеспечения целостности данных будем использовать код Хэмминга
    //если в одном бите одного байта будет ошибка, то получится восстановить
    //но платим местом в памяти за это, конечно
    uint16_t encoded_data[16] = {0};
    encode_hamming(temp_data, sizeof(temp_data), encoded_data);

    //добавляем закодированные данные в буфер SPI
    add2buf(spi_buf, spi_buf_index, encoded_data, sizeof(encoded_data));
    
    spi_handle->tx(spi_buf, (spi_buf_index + 1));
}

uint16_t MR25H40::receive(Bureau_t* msg, uint16_t adr)
{
    size_t spi_buf_index = 0;
    add2buf(spi_buf, spi_buf_index, (void*)&READ_CMD, sizeof(READ_CMD));
    add2buf(spi_buf, spi_buf_index, (void*)&adr, sizeof(adr));

    //32 байта данных + 2 байта команда + 3 байта адрес
    uint8_t rx_data[37] = {0};
    add2buf(spi_buf, spi_buf_index, rx_data, sizeof(rx_data));
    spi_handle->rx(spi_buf, (spi_buf_index + 1));

    //Если RTOS, то эту функцию можно было бы выполнять в задаче и заблокировать
    //пока все данные не придут
    while(spi_handle->busy)
    {}

    //Склеиваем байты в слова, пропуская первые 5 байт
    uint16_t encoded_data[16];
    for(size_t index = 0; index < 16; index ++)
    {
        uint8_t low_byte =  rx_data[index*2 + 5];
        uint8_t high_byte = rx_data[index*2 + 5 + 1];

        encoded_data[index] = ((uint16_t)high_byte << 8) + (uint16_t)low_byte;
    }

    uint8_t decoded_data[16] = {0};
    decode_hamming(encoded_data, sizeof(decoded_data), decoded_data);

    memcpy(msg, decoded_data, sizeof(Bureau_t));
}

void MR25H40::add2buf(uint8_t* buf, size_t& buf_index, void* msg, size_t len)
{
    uint8_t* msgPtr = static_cast<uint8_t*>(msg);

    // Копируем байты из переданного указателя в буфер
    for (size_t i = 0; i < len; i++)
    {
        buf[buf_index++] = msgPtr[i];
    }
}


uint8_t MR25H40::parity(uint16_t x) {
    //Функция для вычисления бита четности для заданного набора битов */
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}


void MR25H40::encode_hamming(uint8_t *data, size_t len, uint16_t *encoded) {
    for (size_t i = 0; i < len; i++) {
        uint16_t word = data[i];
        encoded[i] = (word << 3) | (parity(word) << 2) | (parity(word << 4) << 1) | parity(word << 8);
    }
}


void MR25H40::decode_hamming(uint16_t *encoded, size_t len, uint8_t *decoded) {
    for (size_t i = 0; i < len; i++) {
        uint16_t word = encoded[i];
        uint8_t syndrome = (parity(word) << 2) | (parity(word << 4) << 1) | parity(word << 8);

        if (syndrome) {
            //Исправление ошибки
            word ^= (1 << (syndrome - 1));
        }

        decoded[i] = word >> 3;
    }
}