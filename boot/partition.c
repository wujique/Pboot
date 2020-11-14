/*  
	系统分区表 
*/
#include "mcu.h"
#include "board.h"
#include "log.h"
#include "partition.h"

char BoardPartition[BOARD_PARTITION_SIZE+64];

/*加载Flash中的partition

	失败返回-1
	*/
int partition_load(uint32_t addr, uint32_t len)
{
	Uprintf("partition load...\r\n");
	board_hd_read(addr, BoardPartition, len);
	/* 判断是否是partition */
	if(memcmp(BoardPartition, "[partition", 10) != 0) {
		Uprintf("partition err!\r\n");
		return -1;	
	}
	
	//Uprintf("%s\r\n", BoardPartition);
	return 0;	
}
/* 设置默认分区表
	*/
int partition_set(char *str, uint32_t addr, uint32_t len)
{
	Uprintf("partition set...\r\n");
	board_hd_prog(addr, str, len);	

	board_hd_read(addr, BoardPartition, len);
	/* 判断是否是partition */
	if(memcmp(BoardPartition, "[partition", 10) != 0) {
		Uprintf("set partition err!\r\n");
		return -1;	
	}
	
	//Uprintf("%s\r\n", BoardPartition);
	
	return 0;
}
/*获取指定名称的内容的地址和长度
	*/
int partition_get(char *name, uint32_t *addr, uint32_t *size)
{
	PartititonNode PartNode;
	uint32_t i;
	char *p;
	
	Uprintf("find partiton name:%s\r\n", name);

	i = 0;
	while(1) {
		if(BoardPartition[i] == '{') break;
		i++;
	}
	/*--*/
	i++;
	p = &BoardPartition[i];
	while(1) {
		/* 找名字 */
		i = 0;
		while(1) {
			if(*p < 0x20) {p++;i=0;}
			if(*(p+i) == ':') break;
			else if(*(p+i) == '}') return -1;
			else i++;
		}
		memcpy(PartNode.name, p, i);
		PartNode.name[i] = 0;
		Uprintf("name:%s\r\n", PartNode.name);
		if (strcmp(PartNode.name, name) == 0) break;
		else p += i;

		while(1){
			if(*p == ';') break;
			p++;
		}
		p++;
	}
	
	Uprintf("find partition!\r\n");
	p += i+1;
	i = 0;
	while(1){
		if(*(p+i) == ',') break;
		i++;
	}
	memcpy(PartNode.type, p, i);
	PartNode.type[i] = 0;
	Uprintf("type:%s!\r\n", PartNode.type);
	
	p += i+3;//直接跳过0x

	*addr = str_to_u32_h(p);

	while(1) {
		p++;
		if(*p == ',') break;
	}
	p += 3;
	
	*size = str_to_u32_h(p);
	return 0;	

}

