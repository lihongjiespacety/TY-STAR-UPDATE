/*----------------------
   UART.h header file
  ----------------------*/

#ifdef __cplusplus
extern "C" {
#endif

int uart_open(const char *name);
int uart_config(DWORD baudrate,BYTE bytesize, BYTE parity, BYTE stopbits,UINT timeout);
int uart_flush(void);
int uart_close(const char *name);
DWORD uart_read(BYTE* buff, DWORD len, BOOL* readstat);
int uart_send(BYTE* buff, DWORD len, BOOL* writestat);
int uart_trans(BYTE* outbuff, UINT outlen, BYTE* inbuff, UINT inlen,UINT delay);
int uart_init(void);
#ifdef __cplusplus
}
#endif

