#ifndef __MCU_YMODEM_H__
#define __MCU_YMODEM_H__

#define SOH 0x01//协议头 128byte类型
#define STX 0x02//协议头 1024byte
#define EOT 0x04//传输结束
#define ACK	0x06//接收响应
#define NAK 0x15//失败响应
#define CAN 0x18//取消传输
#define Y_C 0x43//开启文件传输


/* 真正的数据偏移*/
#define YMODEM_HEAD_SIZE	3
#define YMODEM_BUF_SIZE (1024+2+YMODEM_HEAD_SIZE)

typedef struct 
{
	int (*read)(char *buf, int len);
	int (*write)(char *buf, int len);

	/*读的数据直接保存在buf中*/
	int buflen;//buflen中已经存在的数据
	uint8_t buf[YMODEM_BUF_SIZE];	/* 帧内容 */

	/* 解析过程 */
	uint8_t step;
	/* 以下，APP会用到*/
	uint16_t framelen;	//本帧数据长度
	uint8_t *dp;	//帧数据指针，固定指向buf+YMODEM_HEAD_SIZE
	int pcnt;		//本帧是第几包
	char filename[128];	//文件名
	uint32_t filelen;		//文件长度
}YmodemStr;


typedef enum
{
	YMODEM_PARSE = 0,//解析中
	YMODEM_END,//所有文件传输完成
	YMODEM_NEWFILE,//接收到文件头，开始接受一个新文件
	YMODEM_FILEEND,//一个文件接收结束
	YMODEM_FRAME,//接收到1帧数据
	YMODEM_ERR,//解析出现错误
	YMODEM_TIMEOUT,
}YmodenSta;

extern int YmodemInit(YmodemStr *Ymodem);
extern YmodenSta YmodemFlow(YmodemStr *Ymodem, uint32_t timeout);


#endif

