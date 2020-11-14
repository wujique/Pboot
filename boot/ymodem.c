/*

	ymodem协议
	协议就是协议，不做任何保存动作和文件处理。
	这是一个被动模块，调用本模块解析接口解析串口收到的函数
	---------------------------------
	第一包：
	| Pathname | Length | Modification Date | Mode | Serial Number |

	Pathname, 是一个字符串，以字符结束符(\0)结束
	Length，  是一个表示十进制数的一串字符，用来表示传输的文件有多少字节
	Modification Date,与Length用一个空格分隔，是一个八进制数的一串字符，该数表示距离1970年1月1日的秒数
	Mode，与Modification Date用一个空格分隔，是一个八进制数的一串字符，用以表示文件的类型
	Serial Number，与Mode用一个空格分隔，用于标识传输程序，用一个八进制字符串表示
	----------------------------------
	YmodemFlow流程，接收到一个帧后，立刻返回ACK，
	那么，发送端会立刻发送数据，
	但，此时，APP尚未处理本次接收到的数据，
	因此，串口需要开一个至少是一个帧的接收缓冲。
	为什么要这样？
	因为将本次的帧写到FLASH上要时间，同时接收数据，可以减少整个通信过程的时间。

*/
#include "mcu.h"
#include "board.h"
#include "log.h"
#include "math.h"
#include "ymodem.h"

#define YmodemDebug uart_printf
//#define YmodemDebug(fm...) 

static uint16_t YmodemCrc(uint8_t * buf, uint16_t len)
{
    uint16_t crc;
    uint16_t j;
    uint16_t i;
    uint8_t * in_ptr; 
    
    in_ptr = buf;
    crc = 0;
    for (j = 0 ; j < len; j++) {
        crc = crc^(uint16_t)(*in_ptr) << 8;
		in_ptr++;
        for (i=8; i!=0; i--) {
            if (crc & 0x8000) {
                crc = crc << 1 ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void YmodemSendRes(YmodemStr *Ymodem, char Res)
{
	char data;
	data = Res;
	
	Ymodem->write(&data, 1);
}
/*
	初始化Ymodem解析流程
*/
int YmodemInit(YmodemStr *Ymodem)
{
	if(Ymodem->read == NULL
		|| Ymodem->write == NULL)
		return -1;

	Ymodem->pcnt = 0;
	Ymodem->dp = Ymodem->buf + YMODEM_HEAD_SIZE;
	return 0;
}
/*
	ymodem通信流程
	走1K一包，速度可达10KBps
	*/
YmodenSta YmodemFlow(YmodemStr *Ymodem, uint32_t timeout)
{
	uint8_t *p;
	uint8_t i;
	char *prbuf;
	int rlen;
	int dlen;
	YmodenSta ret;
	uint8_t filenamelen;
	uint32_t filelen;
	uint32_t timecnt = 0;
	
	ret = YMODEM_PARSE;
	/* 预读1个字节 当读到头，才进入读数据模式 
		读到一个包数据就会退出这个while循环 
		超时也退出 */
	Ymodem->buflen = 0;
	rlen = 1;
	while(1) {
		prbuf = Ymodem->buf + Ymodem->buflen;
		dlen = Ymodem->read(prbuf, rlen);
		if(dlen == 0) {
			if(timecnt >= timeout){
				return YMODEM_TIMEOUT;	
			}
			
			if (Ymodem->step == 0){
				/* 不断发送C, 尝试开始ymodem通信 */
				YmodemSendRes(Ymodem, Y_C);
				HAL_Delay(100);
				timecnt += 100;
			} else {
				HAL_Delay(10);
				timecnt += 10;
			}
			continue;
		}
		
		Ymodem->buflen += dlen;

		/* 解析收到的数据 */
		if (Ymodem->step == 1 || Ymodem->step == 0) {
			/* 	SOH:表示本包数据区大小有128字节; STX表示本包数据区大小为1024 
				接收数据总长度 1+1+len+2， 进入接收数据阶段
				*/
			if((*prbuf == SOH) || (*prbuf == STX)) {
				if(*prbuf == SOH) Ymodem->framelen = 128;
				else Ymodem->framelen = 1024;
				
				Ymodem->step = 2;
				rlen = (Ymodem->framelen + 2 + YMODEM_HEAD_SIZE) - Ymodem->buflen;
			} else if(*prbuf == EOT) {
					/*1个文件接收结束*/
					Ymodem->step = 3;
					//YmodemDebug("one file!");
					YmodemSendRes(Ymodem, NAK);
					ret = YMODEM_FILEEND;
					break;
			} else {
				Ymodem->buflen = 0;
				rlen = 1;
			}
		}
		else if(Ymodem->step == 3  && *prbuf == EOT){
			Ymodem->step = 1;
			YmodemSendRes(Ymodem, ACK);
			/* 启动下个文件传输 */
			Ymodem->pcnt = 0;
			YmodemSendRes(Ymodem, Y_C);
			break;
		}
		else if(Ymodem->step == 2) {

			if (Ymodem->buflen >= (Ymodem->framelen + 2 + YMODEM_HEAD_SIZE)) {
				
				Ymodem->step = 1;
				
				/*计算CRC校验*/
				uint16_t crc, tmp;
				crc = YmodemCrc(Ymodem->buf + YMODEM_HEAD_SIZE, Ymodem->framelen);
				
				//uart_printf("crc:%04x, %02x-%02x\r\n", crc, 
				//			Str->frame.data[Str->wlen],Str->frame.data[Str->wlen+1]);
				tmp = Ymodem->buf[Ymodem->framelen + YMODEM_HEAD_SIZE]<<8;
				tmp += Ymodem->buf[Ymodem->framelen + YMODEM_HEAD_SIZE + 1];
				if(crc != tmp) {
					YmodemDebug("crc err!");
					ret =  YMODEM_ERR;
				} else {
					ret = YMODEM_FRAME;
				}
				break;
			}else {
				/* 继续读剩下的数据 */
				rlen = (Ymodem->framelen + 2 + YMODEM_HEAD_SIZE) - Ymodem->buflen;
			}
		}
		
	}
	
	
	if(ret != YMODEM_FRAME) return ret;
	
	/* 收到一个帧 */
	Ymodem->pcnt++;
	//YmodemDebug("get bag:%d!", Frame.frame.pnum);
	//YmodemDebug("-", Str->frame.pnum);
	if(Ymodem->pcnt != 1) {
		YmodemSendRes(Ymodem, ACK);
		ret = YMODEM_FRAME;
		return ret;
	}
	/*  如果文件属性为空，说明传输结束 */
	if(Ymodem->buf[YMODEM_HEAD_SIZE] == 0x00) {
		//YmodemDebug("end!");
		YmodemSendRes(Ymodem, ACK);
		ret = YMODEM_END; 
		return ret;
	} 

	/* 第一包 包含文件名 */
	filenamelen = strlen(Ymodem->buf + YMODEM_HEAD_SIZE);
	//PrintFormat(Str->frame.data, 128);
	strcpy(Ymodem->filename, Ymodem->buf + YMODEM_HEAD_SIZE);
	
	//YmodemDebug("\r\nfile name:%s\r\n", Str->frame.data);
	//YmodemDebug("file len:%s\r\n", &(Str->frame.data[filenamelen+1]));
	/* 求文件长度 */
	p = Ymodem->buf + YMODEM_HEAD_SIZE + filenamelen+1;
	i = 0;
	while(1) {
		//空格分隔符
		if(*(p+i) == 0x20) {
			*(p+i) = 0;
			break;
		}
		
		i++;
	}
	filelen = str_to_u32(p);
	//YmodemDebug("file len:%s, %d\r\n", lenstr, filelen);
	Ymodem->filelen = filelen;

	YmodemSendRes(Ymodem, ACK);
	YmodemSendRes(Ymodem, Y_C);
	ret = YMODEM_NEWFILE;
	
	return ret;
}



