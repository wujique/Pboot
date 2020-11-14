#ifndef __BOARD_H__
#define __BOARD_H__

#define  PC_PORT MCU_UART_4
#define  DEBUG_PORT	MCU_UART_2

/* alloc.c管理的内存*/
#define AllocArraySize (120*1024)

#define BOARD_PARTITION_ADDR 0x90000000
#define BOARD_PARTITION_SIZE 512

#endif

