

![](logo.png)

www.wujique.com

本boot下载协议使用ymodem协议，下载工具SecureCRT 8.5。

通过partition制定分区，请参考doc/partition文件，或代码board.c中定义的默认分区BoardPartitionDef。

分区格式：

> [partition:分区表版本]
>
> {
>
> 名称:类型,起始地址,空间长度;
>
> }

只有分区表中存在的文件才可以下载。



## 设计功能

**使用ymodem协议基于简单分区表，支持多平台多FLASH种类的简单的boot**

* 平台：H750, F407, F103, RT1052

* Flash: ST内部FLASH， QSPI flash，SPI flash

## 备忘

2020.11.14

完成H750 +QSPI FLASH下载。基于屋脊雀Albatross小板，H750VB+W25Q64。

BOOT log用串口2，串口2从外扩排母的UART引出，串口下载用串口4，串口4在调试口引出。

APP log用串口4。

H750控制器进入映射模式后，无法退出，所以，执行QSPI FLASH上的程序后，不能再写同一块FLASH。

跳转到APP后，不能调用SystemInit复位芯片，因为我们要维持QSPI在映射状态。具体修改见启动文件。

进入main函数后，也不要操作QSPI接口。

APP demo编译后得到的bin文件在doc\app_example\H750_MDK_APP\Bin\，改名为app.bin后用SecureCRT 下载即可。