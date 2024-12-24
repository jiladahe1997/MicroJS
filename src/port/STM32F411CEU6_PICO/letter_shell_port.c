#include "shell.h"
#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "stdio.h"

Shell shell;
char shellBuffer[512];

//Stream Buffer for letter_shell uart
#define STEAM_BUFFER_SIZE 1024
static StreamBufferHandle_t xStreamBuffer = NULL;
static uint8_t ucStreamBufferStorage[ STEAM_BUFFER_SIZE + 1 ] = {0};
static StaticStreamBuffer_t xStreamBufferStruct;
extern UART_HandleTypeDef huart1;


static short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 0xFFFF);
    return len;
}
static short userShellRead(char *data, unsigned short len)
{
    return xStreamBufferReceive(xStreamBuffer, data, len, portMAX_DELAY);
}

StreamBufferHandle_t get_letter_shell_stream_buffer() {
    return xStreamBuffer;
}


void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);
    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
    {
        printf("shell task creat failed\r\n");
    }
}