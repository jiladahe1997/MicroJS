#include "shell.h"
#include "letter_shell_port.h"
#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "task.h"

Shell shell;
char shellBuffer[512];
#define SHELL_TASK_STACK_SIZE 256
static StaticTask_t xTaskBuffer;
static StackType_t xStack[SHELL_TASK_STACK_SIZE];
static TaskHandle_t xHandle = NULL;

//Stream Buffer for letter_shell uart
#define STEAM_BUFFER_SIZE 1024
static StreamBufferHandle_t StreamBuffer = NULL;
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
    return xStreamBufferReceive(StreamBuffer, data, len, portMAX_DELAY);
}

StreamBufferHandle_t get_letter_shell_stream_buffer() {
    return StreamBuffer;
}


int userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);

    StreamBuffer = xStreamBufferCreateStatic(
                        STEAM_BUFFER_SIZE, 
                        1, 
                        ucStreamBufferStorage,
                        &xStreamBufferStruct);
    if(StreamBuffer == NULL) {
        printf("create shell xStreamBuffer failed!\n");
        return -1;
    }

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);


    xHandle = xTaskCreateStatic(shellTask,
                            "shell", 
                            SHELL_TASK_STACK_SIZE,
                            &shell, 
                            tskIDLE_PRIORITY, 
                            xStack,
                            &xTaskBuffer);
    if (xHandle == NULL)
    {
        printf("shell task creat failed\r\n");
        return -1;
    }

    return 0;
}