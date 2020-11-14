

#include "mcu.h"
#include "log.h"


volatile uint32_t spvalue, reseth;

/* 	直接跳转到指定地址运行
	常规操作包括：
	1.关中断。
	2.清cache
*/
int mcu_jump_address(uint32_t addr)
{
	uint32_t i;

	Uprintf(" jump to firmware\r\n");

	BSP_QSPI_MemoryMap();

	__IO uint8_t *qspi_addr = (__IO uint8_t *)(0x90001000);
	PrintFormat(qspi_addr, 256);

	void (*fun)(void);
	
	/* 关掉所有中断 */
	__disable_irq();
	
	SCB_DisableICache();
	SCB_DisableDCache();

	spvalue = qspi_addr[0]
				+ (qspi_addr[1]<<8) 
				+ (qspi_addr[2]<<16) 
				+ (qspi_addr[3]<<24);
	reseth = qspi_addr[4]
				+ (qspi_addr[5]<<8) 
				+ (qspi_addr[6]<<16) 
				+ (qspi_addr[7]<<24);
	
	Uprintf("spvalue=%08x,reseth=%08x\r\n", spvalue, reseth);
	/* 设置堆栈指针 */
	__set_MSP(spvalue);
	/* 把app当做一个函数执行*/
	fun = (void (*)(void))reseth;
	
	fun();

	return 0;
}


