/* USER CODE BEGIN Includes */
#include <string.h>
#include "stm32746g_qspi.h"
/* USER CODE END Includes */
/* USER CODE BEGIN Includes */
extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
QSPI_CommandTypeDef s_command;
uint8_t pData[3];
     
uint8_t wData[0x100];
uint8_t rData[0x100];
 
uint32_t i;

extern const u8 test_qspi_tab[33];

static void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi);

/* USER CODE END PV */
/*
	使用spi，dspi，qspi读ID
	然后用qspi测试擦除读写操作。
	本测试可以用于测试SPI是否能用。
	测试能用后再进行映射
*/
void QSPI_TEST(void)
{

    uart_printf("QuadSPI Test 1 2020 10 20..\r\n\r\n");
 	#if 0
	 /*--------------------------------------读ID测试--------------------------------------------------*/ 
    /* Read Manufacture/Device ID */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
     
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("SPI  I/0 Read Device ID : 0x%2X 0x%2X\r\n", pData[0],pData[1]);
     
    /* Read Manufacture/Device ID Dual I/O*/
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = DUAL_READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_2_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0x000000;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_2_LINES;
    s_command.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes    = 0;
    s_command.DataMode          = QSPI_DATA_2_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 4;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        Error_Handler();
    }
	
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        Error_Handler();
    }
	
    uart_printf("Dual I/O Read Device ID : 0x%2X 0x%2X\r\n",pData[0],pData[1]);
 
    /* Read Manufacture/Device ID Quad I/O*/
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    s_command.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes    = 0x00;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 4;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("Quad I/O Read Device ID : 0x%2X 0x%2X\r\n",pData[0],pData[1]);
 
    /* Read JEDEC ID */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_JEDEC_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 3;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("Read JEDEC ID :  0x%2X 0x%2X 0x%2X\r\n\r\n",pData[0],pData[1],pData[2]);
     #endif
    /*------------------------------------------读写测试---------------------------------------------*/ 

    /*-3-QSPI Erase/Write/Read Test */
    /* 填充测试数据 */
    for(i =0;i<0x100;i ++)
    {
        //wData[i] = 0x55;
        wData[i] = i;
        rData[i] = 0;
    }
	/* 调试地址映射不要擦除和写*/
	#if 0
    if(BSP_QSPI_Erase_Block(0) == QSPI_OK)
        uart_printf(" QSPI Erase Block ok\r\n");
    else
        Error_Handler();
 
    if(BSP_QSPI_Write(wData,0x00,0x100)== QSPI_OK)
        uart_printf(" QSPI Write ok\r\n");
    else
        Error_Handler();
	#endif
	
    if(BSP_QSPI_Read(rData,0x00,0x100)== QSPI_OK)
        uart_printf(" QSPI Read ok\r\n\r\n");
    else
        Error_Handler();

	uart_printf("QSPI Read Data : \r\n");
    for(i =0;i<0x100;i++)
        uart_printf("0x%02X  ",rData[i]);
    uart_printf("\r\n");
 
    for(i =0;i<0x100;i++)
        if(rData[i] != wData[i])
			uart_printf("<0x%02X 0x%02X> ",wData[i],rData[i]);
    uart_printf("\r\n");
    /* check date */
    if(memcmp(wData,rData,0x100) == 0 ) 
        uart_printf(" W25Q128FV QuadSPI Test OK\r\n");
    else
        uart_printf(" W25Q128FV QuadSPI Test False\r\n");

	/*--------------------------- 地址映射测试 ---------------------------------*/
	BSP_QSPI_MemoryMap();

	__IO uint8_t *qspi_addr = (__IO uint8_t *)(0x90000000);
	for (i = 0; i < 0x100; i++){
		uart_printf("0x%02X ", *qspi_addr);
		qspi_addr++;
	}

	PrintFormat(test_qspi_tab, 32);

	u8 cnt = 55;
	cnt = test_qpi_run_fun(cnt);
	uart_printf("\r\n---%d-----\r\n", cnt);

	uart_printf("--------ok\r\n");
	while(1);
  /* USER CODE END 2 */
}

int qpi_flash_init(void)
{
	uint8_t tmp[3];
	
	/* 初始化QSPI, 并将Flash配置为QSPI接口模式 */
    BSP_QSPI_Init();

	/*--------------------------------------读ID测试--------------------------------------------------*/ 
    /* Read Manufacture/Device ID */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
     
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("SPI  I/0 Read Device ID : 0x%2X 0x%2X\r\n", pData[0],pData[1]);
     
    /* Read Manufacture/Device ID Dual I/O*/
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = DUAL_READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_2_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0x000000;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_2_LINES;
    s_command.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes    = 0;
    s_command.DataMode          = QSPI_DATA_2_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 4;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        Error_Handler();
    }
	
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        Error_Handler();
    }
	
    uart_printf("Dual I/O Read Device ID : 0x%2X 0x%2X\r\n",pData[0],pData[1]);
 
    /* Read Manufacture/Device ID Quad I/O*/
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = 0;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    s_command.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes    = 0x00;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 4;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("Quad I/O Read Device ID : 0x%2X 0x%2X\r\n",pData[0],pData[1]);
 
    /* Read JEDEC ID */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_JEDEC_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 3;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
 
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    uart_printf("Read JEDEC ID :  0x%2X 0x%2X 0x%2X\r\n\r\n",pData[0],pData[1],pData[2]);

	return 0;
}



