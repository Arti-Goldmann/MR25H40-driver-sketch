#include "main.h"
#include "MR25H40Driver.h"

const uint16_t MR25H40::WREN_CMD;
const uint16_t MR25H40::READ_CMD;
const uint16_t MR25H40::WRITE_CMD;
const size_t   MR25H40::CMD_SIZE;
const size_t   MR25H40::ADR_SIZE;
const size_t   MR25H40::DATA_SIZE;

MR25H40::MR25H40()
{
    spi = new SPI_portA();
}

uint16_t MR25H40::init(void)
{
    spi->init();
    spi->buf_index = 0;
    //Разрешаем запись данных
    add2buf(spi->txbuf, spi->buf_index, (void*)&WREN_CMD, CMD_SIZE);
    
    return spi->transmit(spi->buf_index + 1);
}

uint16_t MR25H40::transmit(Bureau_t* msg, uint32_t adr)
{
    spi->buf_index = 0;
    add2buf(spi->txbuf, spi->buf_index, (void*)&WRITE_CMD, CMD_SIZE);
    add2buf(spi->txbuf, spi->buf_index, (void*)&adr, ADR_SIZE);

    //Разобьем Структуру Bureau на байты
    if(sizeof(msg) >= DATA_SIZE) return 0;

    uint8_t temp_data[DATA_SIZE] = {0}; //16 байт должно хватить
    size_t data_index = 0;

    add2buf(temp_data, data_index, (void*)msg, sizeof(msg));

    //Для обеспечения целостности данных будем использовать код Хэмминга
    //если в одном бите одного байта будет ошибка, то получится восстановить
    //но платим местом в памяти за это, конечно

    uint16_t encoded_data[DATA_SIZE] = {0};
    encode_hamming(temp_data, sizeof(temp_data), encoded_data);

    //Добавляем закодированные данные в буфер SPI
    add2buf(spi->txbuf, spi->buf_index, encoded_data, sizeof(encoded_data));
     
    return spi->transmit(spi->buf_index + 1);
}

uint16_t MR25H40::receive(Bureau_t* recieved_data, uint32_t adr)
{
    spi->buf_index = 0;
    add2buf(spi->txbuf, spi->buf_index, (void*)&READ_CMD, CMD_SIZE);
    add2buf(spi->txbuf, spi->buf_index, (void*)&adr, ADR_SIZE);

    //2 байта команда + 3 байта адрес + 32 байта данных (так как после закодирования размер данных увеличился в 2 раза) 
    size_t size_of_rx_data = CMD_SIZE + ADR_SIZE + DATA_SIZE * 2;
    if(size_of_rx_data > sizeof(spi->rxbuf)) return 0;

    //Шлем 32 байта 0xFF, чтобы выставить клоки и параллельно читать RX
    while(spi->buf_index < size_of_rx_data)
    {   
        spi->txbuf[spi->buf_index++] = 0xFF;  
    }

    if(!spi->transmit(spi->buf_index + 1)) return 0;

    //Если RTOS, то эту функцию, например, можно было бы выполнять в задаче и заблокировать
    //пока все данные не придут
    while(spi->is_busy())
    {}

    //Склеиваем полученные байты в слова, пропуская первые 5 байт, которые мы прочитали, пока слали CMD и ADR
    uint16_t encoded_data[DATA_SIZE];
    size_t offset = CMD_SIZE + ADR_SIZE;

    for(size_t index = 0; index < DATA_SIZE; index ++)
    {
        uint8_t low_byte =  spi->rxbuf[index * 2 + offset];
        uint8_t high_byte = spi->rxbuf[index * 2 + offset + 1];

        encoded_data[index] = ((uint16_t)high_byte << 8) + (uint16_t)low_byte;
    }

    decode_hamming(encoded_data, DATA_SIZE, (uint8_t*)recieved_data);
    return 1;
}

void MR25H40::add2buf(uint8_t* buf, size_t& buf_index, void* input, size_t len)
{
    uint8_t* input_ptr = static_cast<uint8_t*>(input);

    // Копируем байты из переданного указателя в буфер
    for (size_t i = 0; i < len; i++)
    {
        buf[buf_index++] = input_ptr[i];
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