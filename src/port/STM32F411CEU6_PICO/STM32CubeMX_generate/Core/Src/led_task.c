#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_TASK_STACK_SIZE 256
static StaticTask_t xTaskBuffer;
static StackType_t xStack[LED_TASK_STACK_SIZE];
static TaskHandle_t xHandle = NULL;
void led_task(void * pvParameters){
    for(;;) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        vTaskDelay(670);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        vTaskDelay(330);
    }
}

int start_led_task(void) {
    xHandle = xTaskCreateStatic(
                      led_task,       /* Function that implements the task. */
                      "led_task",          /* Text name for the task. */
                      LED_TASK_STACK_SIZE,      /* Number of indexes in the xStack array. */
                      NULL,    /* Parameter passed into the task. */
                      1,/* Priority at which the task is created. */
                      xStack,          /* Array to use as the task's stack. */
                      &xTaskBuffer );  /* Variable to hold the task's data structure. */
    
    if(xHandle == NULL) {
        printf("ERROR create led_task failed\r\n");
        return -1;
    } else {
        return 0;
    }
}