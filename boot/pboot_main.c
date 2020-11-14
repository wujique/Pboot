
#include "mcu.h"
#include "board.h"
#include "log.h"

#include "ymodem.h"

uint8_t data[256];
uint8_t recbuf[256];
uint8_t checkbuf[256];

YmodemStr YmodemDocker;


int pboot_download_getdata(char *buf, int len)
{
	int rlen;
	
	rlen = mcu_uart_read(PC_PORT, buf, len);

	return rlen;
}

int pboot_download_putdata(char *buf, int len)
{
	int rlen;
	
	rlen = mcu_uart_write(PC_PORT, buf, len);

	return rlen;
}

/*
	下载流程
	*/
int pboot_download(uint32_t time_out)
{
	YmodenSta Yres;
	static uint32_t addr;
	uint32_t size;
	int res;
	
	YmodemDocker.read = pboot_download_getdata;
	YmodemDocker.write = pboot_download_putdata;
	YmodemInit(&YmodemDocker);

	/* 启动传输 */
    while (1) {
		//Uprintf("rlen:%d\r\n", res);
		/* 进入ymodem传输交互，
		超时，或得到一个包后退出 */
		Yres = YmodemFlow(&YmodemDocker, time_out);	
		switch(Yres)
		{
			case YMODEM_PARSE:
				break;
				
			case YMODEM_FRAME:
				/* 接收到的数据写入对应地址
					第1帧是文件信息，第2帧才是数据
					*/
				//Uprintf("%d %d\r\n", YmodemDocker.pcnt, YmodemDocker.framelen);
				/* flash 擦1个block是 4K */
				board_hd_prog_continue(addr, YmodemDocker.dp, YmodemDocker.framelen);	
				addr +=  YmodemDocker.framelen;
				break;
				
			case YMODEM_END:
				/* ymoden通信结束, 说明下载结束 */
				Uprintf(" Ymoden end\r\n");
				return YMODEM_END;
				break;
			
			case YMODEM_NEWFILE:
				Uprintf("file:%s, len:%d\r\n", YmodemDocker.filename, YmodemDocker.filelen);
				/* 从partiton查找文件的地址和空间*/
				res = partition_get(YmodemDocker.filename, &addr, &size);
				
				if(res == -1 ){
					Uprintf("file name err!\r\n");
					return YMODEM_NEWFILE;
				}
				
				Uprintf("partition addr:0x%08x, size:0x%x\r\n", addr, size);
				if ( YmodemDocker.filelen > size) {
					Uprintf("warning: file len over hd size!\r\n");
				}
				break;
			
			case YMODEM_FILEEND:
				Uprintf("\r\nfile end\r\n");
				/* 如果是程序文件，将文件名和长度添加到partition指定的app空间后4K位置*/
				break;
			
			case YMODEM_ERR:
				Uprintf("err\r\n");
				break;
			case YMODEM_TIMEOUT:
				Uprintf("timeout\r\n");
				return YMODEM_TIMEOUT;
				break;
			default:
				Uprintf("????\r\n");
				break;
		}

    }
}



void pboot_main(void)
{
	int res;
	uint32_t time_out;

	time_out = 2000;
	/* 检测按键，或者等待一定时间 决定是否进入 BOOT下载流程*/
	Uprintf(" wait pause or download..\r\n");
	
	/* 检测固件有效性 */
	res = pboot_check_firmware();
	if(res == -1) {
		Uprintf(" firmware check err!\r\n");	
		time_out = 0xffffffff;
	}
	/* 进入pboot_download流程 */
	res = pboot_download(time_out);
	
	/* 不进入流程，进行跳转 */
	mcu_jump_address();
}

