#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "shell.h"
#include "xmodem_port.h"
#include "xmodem.h"
#include "stdio.h"

#define FLASH_RESERVE_FOR_JS_FILE_ADDR 0x8060000
#define FLASH_RESERVE_FOR_JS_FILE_SIZE (128*1024)

static bool flag_uart_data_to_xmodem = false;
extern UART_HandleTypeDef huart1;

//Stream Buffer for xmodem
#define STEAM_BUFFER_SIZE 1024
static StreamBufferHandle_t StreamBuffer = NULL;
static uint8_t ucStreamBufferStorage[ STEAM_BUFFER_SIZE + 1 ] = {0};
static StaticStreamBuffer_t xStreamBufferStruct;

void uart_transmit_ch(uint8_t data) {
  HAL_UART_Transmit(&huart1, (uint8_t *)(&data), 1, 0xFFFF);
}

int uart_receive(uint8_t *data, uint16_t length)
{
    uint16_t remain_to_recv = length;
    uint16_t have_receive = 0;
    size_t xReceivedLength;
    while(remain_to_recv > 0) {
        xReceivedLength = xStreamBufferReceive(StreamBuffer, data+have_receive, 1, 1000);
        if(xReceivedLength != 1) {
            return -1;
        }
        remain_to_recv--;
        have_receive++;
    }
    return 0;
}


/* called when receive fisrt packet */
int port_xmodem_first_packet_handle()
{
    HAL_StatusTypeDef ret;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t error;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.Sector = FLASH_SECTOR_7;
    EraseInitStruct.NbSectors   = 1;
    EraseInitStruct.VoltageRange   = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    ret = HAL_FLASHEx_Erase(&EraseInitStruct, &error);
    HAL_FLASH_Lock();

    if(ret != HAL_OK) {
        printf("flash erase failed!\n");
        return -1;
    }

    return 0;
}

int port_xmodem_common_packet_handle(uint8_t *data, uint16_t size, uint16_t xmodem_packet_number) {
    HAL_StatusTypeDef ret;

    for(uint16_t i = 0; i < size; i++) {
        if(data[i] == 0x1A) 
            data[i] = 0x00;
    }

    HAL_FLASH_Unlock();

    for(uint16_t i = 0; i < size / 4; i++) {
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                                FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                    FLASH_RESERVE_FOR_JS_FILE_ADDR + (xmodem_packet_number-1)*128 + i*4,
                    *((uint64_t*)(data+i*4)));
        if(ret != HAL_OK) {
            printf("flash write failed!\n");
            break;
        }
    }
    HAL_FLASH_Lock();

    if(ret != HAL_OK)
        return -1;
    else
        return 0;
}

bool get_flag_uart_data_to_xmodem() {
    return flag_uart_data_to_xmodem;
}

void set_flag_uart_data_to_xmodem(bool flag) {
    flag_uart_data_to_xmodem = flag;
}

StreamBufferHandle_t get_xmodem_stream_buffer() {
    return StreamBuffer;
}


void shell_command_sendjsfile(int argc, char *argv[])
{
    StreamBuffer = xStreamBufferCreateStatic(
                        STEAM_BUFFER_SIZE, 
                        1, 
                        ucStreamBufferStorage,
                        &xStreamBufferStruct);
    if(StreamBuffer == NULL) {
        printf("create shell xStreamBuffer failed!\n");
        return;
    }

    set_flag_uart_data_to_xmodem(true);
    xmodem_receive();

    set_flag_uart_data_to_xmodem(false);
    vStreamBufferDelete(StreamBuffer);
}

SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
sendjs, shell_command_sendjsfile, send_js_file_to_board\r\n);
