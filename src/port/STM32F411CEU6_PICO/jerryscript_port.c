#include "FreeRTOS.h"
#include "task.h"
#include "jerryscript.h"
#include "common.h"

#define JS_TASK_STACK_SIZE 1024
static StaticTask_t jsTaskBuffer;
static StackType_t jsTaskStack[JS_TASK_STACK_SIZE];
static TaskHandle_t jsTaskHandle = NULL;

static jerry_value_t
js_port_c_GPIO_write (const jerry_call_info_t *call_info_p,
               const jerry_value_t arguments[],
               const jerry_length_t arguments_count);
static jerry_value_t
js_port_c_sleep (const jerry_call_info_t *call_info_p,
               const jerry_value_t arguments[],
               const jerry_length_t arguments_count);

static void
print_js_value (const jerry_value_t jsvalue)
{
  jerry_value_t value;
  /* If there is an error extract the object from it */
  if (jerry_value_is_exception (jsvalue))
  {
    printf ("Error value detected: ");
    value = jerry_exception_value (jsvalue, false);
  }
  else
  {
    value = jerry_value_copy (jsvalue);
  }

  if (jerry_value_is_undefined (value))
  {
    printf ("undefined");
  }
  else if (jerry_value_is_null (value))
  {
    printf ("null");
  }
  else if (jerry_value_is_boolean (value))
  {
    if (jerry_value_is_true (value))
    {
      printf ("true");
    }
    else
    {
      printf ("false");
    }
  }
  /* Float value */
  else if (jerry_value_is_number (value))
  {
    printf ("number: %f", jerry_value_as_number (value));
  }
  /* String value */
  else if (jerry_value_is_string (value))
  {
    jerry_char_t str_buf_p[256];

    /* Determining required buffer size */
    jerry_size_t req_sz = jerry_string_size (value, JERRY_ENCODING_CESU8);

    if (req_sz <= 255)
    {
      jerry_string_to_buffer (value, JERRY_ENCODING_CESU8, str_buf_p, req_sz);
      str_buf_p[req_sz] = '\0';
      printf ("%s", (const char *) str_buf_p);
    }
    else
    {
      printf ("error: buffer isn't big enough");
    }
  }
  /* Object reference */
  else if (jerry_value_is_object (value))
  {
    printf ("[JS object]");
  }

  printf ("\n");
  jerry_value_free (value);
}

void js_task(void *parameter)
{
    //读取JS文件并执行
    vTaskDelay(500);
    jerry_char_t *script = FLASH_RESERVE_FOR_JS_FILE_ADDR;
    jerry_length_t script_size = strlen(script);

    printf("\r\n\r\n");
    printf("script:%s\r\n", script);
    // printf("123\r\n");
    // printf("start exec js script_size:%d \r\n", script_size);
    // printf("456\r\n");
    // printf("start exec js script_size:%d \r\n", script_size);
    /* Initialize engine */
    jerry_init (JERRY_INIT_EMPTY);

    jerryx_register_global ("GPIO_write", js_port_c_GPIO_write);
    jerryx_register_global ("sleep", js_port_c_sleep);

    /* Run the demo script with 'eval' */
    jerry_value_t eval_ret = jerry_eval (script,
                                        script_size,
                                        JERRY_PARSE_NO_OPTS);

    /* Check if there was any error (syntax or runtime) */
    bool run_ok = !jerry_value_is_exception (eval_ret);

    printf("\r\n");
    printf("javascript exec end return:\r\n");
    print_js_value(eval_ret);

    /* Parsed source code must be freed */
    jerry_value_free (eval_ret);

    /* Cleanup engine */
    jerry_cleanup ();

    jsTaskHandle=NULL;
    vTaskDelete(NULL);
}

int create_js_run_thread() {
    if(jsTaskHandle != NULL) 
        vTaskDelete(jsTaskHandle);

    jsTaskHandle = xTaskCreateStatic(js_task,
                            "js_task", 
                            JS_TASK_STACK_SIZE,
                            NULL, 
                            tskIDLE_PRIORITY, 
                            jsTaskStack,
                            &jsTaskBuffer);
    if (jsTaskHandle == NULL)
    {
        printf("js task creat failed\r\n");
        return -1;
    }

    return 0;
}

#include "stm32f4xx_hal.h"
static jerry_value_t
js_port_c_GPIO_write (const jerry_call_info_t *call_info_p,
               const jerry_value_t arguments[],
               const jerry_length_t arguments_count)
{
    if (arguments_count > 0) {
        jerry_value_t js_gpio_group = jerry_value_to_string (arguments[0]);
        jerry_value_t js_gpio_pin = jerry_value_to_number (arguments[1]);
        jerry_value_t js_gpio_value = jerry_value_to_number (arguments[2]);

        char c_gpio_group[16]; 
        jerry_size_t copied_bytes = jerry_string_to_buffer(js_gpio_group, JERRY_ENCODING_UTF8, 
                                    c_gpio_group, sizeof(c_gpio_group));
        if(copied_bytes >= 16) {
            printf("error !js_port_c_GPIO_write copied_bytes over 16 \r\n");
            goto error;
        }
        c_gpio_group[copied_bytes] = '\0';

        int c_gpio_pin = jerry_value_as_integer(js_gpio_pin);
        int c_gpio_vaule = jerry_value_as_integer(js_gpio_value);

        printf("debug: js_port_c_GPIO_write %s %d %d \r\n", c_gpio_group, c_gpio_pin, c_gpio_vaule);

        //parse GPIO GROUP
        // shoule be "GPIOA GPIOB GPIOC"
        GPIO_TypeDef* GPIOx;
        if(strcmp(c_gpio_group, "GPIOA") == 0) {
            GPIOx = GPIOA;
        } 
        else if(strcmp(c_gpio_group, "GPIOB") == 0) {
            GPIOx = GPIOB;
        } 
        else if(strcmp(c_gpio_group, "GPIOC") == 0) {
            GPIOx = GPIOC;
        } 
        else {
            printf("error !js_port_c_GPIO_write unknown GPIO group:%s \r\n", c_gpio_group);
            goto error;
        }


        HAL_GPIO_WritePin(GPIOx, 1 << c_gpio_pin, (GPIO_PinState)c_gpio_vaule);

error:
        jerry_value_free (js_gpio_group);
        jerry_value_free (js_gpio_pin);
        jerry_value_free (js_gpio_value);
    }
    return jerry_undefined ();
}

static jerry_value_t
js_port_c_sleep (const jerry_call_info_t *call_info_p,
               const jerry_value_t arguments[],
               const jerry_length_t arguments_count)
{
    if (arguments_count > 0) {
        jerry_value_t js_time = jerry_value_to_number (arguments[0]);
        int c_time = jerry_value_as_integer(js_time);
        printf("debug: js_port_c_sleep %d \r\n", c_time);
        vTaskDelay(c_time);
error:
        jerry_value_free (js_time);
    }
    return jerry_undefined ();
}