#ifndef __XMODEM_PORT_H__
#define __XMODEM_PORT_H__
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "stdbool.h"

bool get_flag_uart_data_to_xmodem();
void set_flag_uart_data_to_xmodem(bool flag);
int port_xmodem_first_packet_handle();
int port_xmodem_common_packet_handle(uint8_t *data, uint16_t size, uint16_t xmodem_packet_number);
void uart_transmit_ch(uint8_t data);
int uart_receive(uint8_t *data, uint16_t length);
StreamBufferHandle_t get_xmodem_stream_buffer();
#endif // __XMODEM_PORT_H__