#ifndef __LETTER_SHELL_PORT_H__
#define __LETTER_SHELL_PORT_H__
#include "FreeRTOS.h"
#include "stream_buffer.h"

int userShellInit(void);
StreamBufferHandle_t get_letter_shell_stream_buffer();

#endif /* __LETTER_SHELL_PORT_H__ */