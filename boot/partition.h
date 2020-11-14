#ifndef __PARTITION_H__
#define __PARTITION_H__


typedef struct {
	char name[64];
	char type[16];
	uint32_t addr;
	uint32_t len;
}PartititonNode;

#endif
