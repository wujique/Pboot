#include "mcu.h"
#include "board.h"
#include "log.h"

/*  校验固件 
	参数待定
*/
int pboot_check_firmware(void)
{
	/* 找app */

	/* 检验app :APP最后4K保存APP信息
		计算整个APP的CRC32 hash_CRC32
	*/
	//hash_CRC32(0, 0);

	return 0;	
}

