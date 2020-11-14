/*

	板上设备初始化
*/
#include "mcu.h"
#include "board.h"
#include "log.h"

const char *BoardPartitionDef="[partition:ver20201029]\r\n\
{\r\n\
boot:prog,0x08000000,0x10000;\r\n\
partition:data,0x90000000,0x1000;\r\n\
app:prog,0x90001000,0x200000;\r\n\
}";


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

QSPI_HandleTypeDef hqspi;

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 2;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 22;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
}

/* */
void board_init(void)
{
	int res;
	
	/* 串口是输出LOG，最先初始化 */
	mcu_uart_init();
	mcu_uart_open(DEBUG_PORT);
	wjq_log(LOG_INFO, "------Albatross(h750vb)------\r\n");

	mcu_uart_open(PC_PORT);
	
	MX_GPIO_Init();
	MX_QUADSPI_Init();

	qpi_flash_init();

	//QSPI_TEST();
	
	/* 读partition出来 */
	res = partition_load(BOARD_PARTITION_ADDR, BOARD_PARTITION_SIZE);
	if(res != 0) {
		partition_set(BoardPartitionDef, BOARD_PARTITION_ADDR, BOARD_PARTITION_SIZE);	
		res = partition_load(BOARD_PARTITION_ADDR, BOARD_PARTITION_SIZE);
	}

	return;
}

#include "stm32746g_qspi.h"
/* 写flash时用的临时缓冲，
	*/
#define NOR_FLASH_BLOCK_SIZE	0x1000
char HdBuf[NOR_FLASH_BLOCK_SIZE];

/*  board  存储 读写接口 
	任意写 
	*/
int board_hd_prog(uint32_t addr, char *data, uint32_t len)
{
	uint32_t flash_addr;
	uint32_t rlen;
	uint32_t block_addr;
	uint8_t qspi_ret;

	/* 三段 */
	uint32_t bs,bw;
	uint32_t ms,mw;
	uint32_t as,aw;
	
	flash_addr = addr - 0x90000000;
	Uprintf("\r\n\r\n>addr:%08x\r\n", flash_addr);
	
	rlen = 0;
	bs = 0;//永远为0
	while(1) {
		if(rlen >= len ) break;

		block_addr = flash_addr&0xfffff000;
		ms = flash_addr - block_addr;
	
		if(ms !=0 ) {
			/* 写地址不是block对齐*/
			bw = ms;
			qspi_ret = BSP_QSPI_Read(HdBuf, block_addr+bs, bw);
		} else {
			bw = 0;
		}
		
		mw = len-rlen;
		if(mw < NOR_FLASH_BLOCK_SIZE-ms){
			/* 写结束也不是block对齐*/
			as = ms+mw;
			aw = NOR_FLASH_BLOCK_SIZE- as;
			qspi_ret = BSP_QSPI_Read(HdBuf+as, block_addr+as, aw);
		} else {
			mw = NOR_FLASH_BLOCK_SIZE-ms;
			aw = 0;
		}

		qspi_ret = BSP_QSPI_Erase_Block(block_addr);

		if(bw != 0) {
			Uprintf("b:%d, %d \r\n", bs, bw);
	   		qspi_ret = BSP_QSPI_Write(HdBuf, block_addr+bs, bw);
		}
		
		if(mw != 0) {
			Uprintf("m:%d, %d \r\n", ms, mw);
			//PrintFormat(data+rlen, mw);
			qspi_ret = BSP_QSPI_Write(data+rlen, block_addr+ms, mw);
			
		}

		if(aw != 0) {
			Uprintf("a: %d, %d \r\n", as, aw);
			//PrintFormat(HdBuf+as, aw);
			qspi_ret = BSP_QSPI_Write(HdBuf+as, block_addr+as, aw);

		}

		//qspi_ret = BSP_QSPI_Read(HdBuf, block_addr, 256);
		//PrintFormat(HdBuf, 256);
		
		flash_addr += mw;
		rlen += mw;
	}
	
	Uprintf("board_hd_prog %d\r\n", qspi_ret);
	return len;
}


int board_hd_read(uint32_t addr, char *data, uint32_t len)
{
	uint32_t flash_addr;
	
	flash_addr = addr - 0x90000000;

    if(BSP_QSPI_Read(data, flash_addr, len)== QSPI_OK)
        uart_printf(" QSPI Read ok\r\n\r\n");
    else
        Error_Handler();
	
	return len;
}

int board_hd_write(uint32_t addr, char *data, uint32_t len)
{
	uint32_t flash_addr;
	
	flash_addr = addr - 0x90000000;

    if(BSP_QSPI_Write(data, flash_addr, len)== QSPI_OK)
        uart_printf(" QSPI Write ok\r\n\r\n");
    else
        Error_Handler();
	
	return len;
}


int board_hd_erase_block(uint32_t addr)
{
	uint32_t flash_addr;
	
	flash_addr = addr - 0x90000000;

    if(BSP_QSPI_Erase_Block(flash_addr)== QSPI_OK)
        uart_printf(" QSPI erase ok\r\n\r\n");
    else
        Error_Handler();
	
	return 0;
}
/*
	连续写，：
	1. 地址夸block时擦除
	2. 不做数据恢复, 比如从Block中间开始写，整个block被擦除，被多擦的数据不会进行恢复。
*/
uint32_t EraseBlockAddr = 0xffffffff;

int board_hd_prog_continue(uint32_t addr, char *data, uint32_t len)
{
	uint32_t flash_addr;
	uint32_t rlen;
	uint32_t block_addr;
	uint8_t qspi_ret;

	uint32_t windex,wlen;

	flash_addr = addr - 0x90000000;
	Uprintf(">addr:%08x ", flash_addr);
	
	rlen = 0;
	while(1) {
		if(rlen >= len ) break;

		block_addr = flash_addr&0xfffff000;
		if(block_addr != EraseBlockAddr) {
			qspi_ret = BSP_QSPI_Erase_Block(block_addr);
			EraseBlockAddr = block_addr;
			Uprintf("<s>\r\n");
		}
		windex = flash_addr-block_addr;
		wlen = len-rlen;
		
		if(wlen > NOR_FLASH_BLOCK_SIZE-windex){
			wlen = NOR_FLASH_BLOCK_SIZE-windex;
		}
		
		qspi_ret = BSP_QSPI_Write(data+rlen, block_addr+windex, wlen);

		flash_addr += wlen;
		if(flash_addr - EraseBlockAddr >= NOR_FLASH_BLOCK_SIZE){
			EraseBlockAddr = 0xffffffff;
			Uprintf(" f ");
		}
		rlen += wlen;
	}
	
	Uprintf("board_hd_prog %d\r\n", qspi_ret);
	return len;
}



