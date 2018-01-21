//包含头文件
#include "flash.h" 
#include "string.h"//调用函数memcpy需包含此头文件

//=================内部调用函数声明=========================================
//======================================================================
//函数名称：flash_cmd_launch
//函数返回：0-成功 1-失败
//参数说明：无
//功能概要：启动Flash命令
//======================================================================
static uint_32 flash_cmd_launch(void);
//======================================================================

//=================外部接口函数============================================
//======================================================================
//函数名称：flash_init
//函数返回：无
//参数说明：无
//功能概要：初始化flash模块
//======================================================================
void flash_init(void)
{   
    // 等待命令完成
    while(!(FTFA_FSTAT & CCIF));
    
  // 清除访问出错标志位
    FTFA_FSTAT = ACCERR | FPVIOL;
    //置杂项模块中平台控制寄存器的PLACR_ESFC，Flash模块擦写保护
    //BSET(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);
    /*
    //清杂项模块中平台控制寄存器的PLACR_ESFC，Flash模块擦写不保护
    BCLR(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);   //（实验观察现象用）
    */
}
//======================================================================
//函数名称：flash_erase_sector
//函数返回：函数执行执行状态：0=正常；1=异常。
//参数说明：sect：目标扇区号（范围取决于实际芯片，例如KL36：0~63，每扇区1KB）
//功能概要：擦除flash存储器的sect扇区（每扇区1KB）
//======================================================================
uint_8 flash_erase(uint_16 sect)
{
    union
    {
        uint_32  word;
        uint_8   byte[4];
    } dest;
    
    dest.word    = (uint_32)(sect*(1<<10));

    // 设置擦除命令
    FTFA_FCCOB0 = ERSSCR; // 擦除扇区命令
    
    // 设置目标地址
    FTFA_FCCOB1 = dest.byte[2];
    FTFA_FCCOB2 = dest.byte[1];
    FTFA_FCCOB3 = dest.byte[0];
    
    // 执行命令序列
    if(1 == flash_cmd_launch())    //若执行命令出现错误
        return 1;     //擦除命令错误
   
    // 若擦除sector0时，则解锁设备
//    if(dest.word <= 0x800)
    if(dest.word < 0x400)
    {
        // 写入4字节
        FTFA_FCCOB0 = PGM4; 
        // 设置目标地址
        FTFA_FCCOB1 = 0x00;
        FTFA_FCCOB2 = 0x04;
        FTFA_FCCOB3 = 0x0C;
        // 数据
        FTFA_FCCOB4 = 0xFF;
        FTFA_FCCOB5 = 0xFF;
        FTFA_FCCOB6 = 0xFF;
        FTFA_FCCOB7 = 0xFE;
        // 执行命令序列
        if(1 == flash_cmd_launch())  //若执行命令出现错误
            return 2;   //解锁命令错误
    }  
    
    return 0;  //成功返回
}
//======================================================================
//函数名称：flash_write
//函数返回：函数执行状态：0=正常；1=异常。
//参数说明：sect：扇区号（范围取决于实际芯片，例如KL36：0~63，每扇区1KB）
//        offset:写入扇区内部偏移地址（0~1020，要求为0,4，8,12，......）
//        N：写入字节数目（4~1024,要求为4，8,12,......）
//        buf：源数据缓冲区首地址
//功能概要：将buf开始的N字节写入到flash存储器的sect扇区的 offset处
//=======================================================================
uint_8 flash_write(uint_16 sect,uint_16 offset,uint_16 N,uint_8 *buf)
{
    uint_32 size;
    uint_32 destaddr;
    
    union
    {
        uint_32   word;
        uint_8  byte[4];
    } dest;
    
    if(offset%4 != 0)
        return 1;   //参数设定错误，偏移量未对齐（4字节对齐）
    
    // 设置写入命令
    FTFA_FCCOB0 = PGM4;
    destaddr = (uint_32)(sect*(1<<10) + offset);//计算地址
    dest.word = destaddr;
    for(size=0; size<N; size+=4, dest.word+=4, buf+=4)
    {
        // 设置目标地址
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];
 
        // 拷贝数据
        FTFA_FCCOB4 = buf[3];//大端寻址
        FTFA_FCCOB5 = buf[2];
        FTFA_FCCOB6 = buf[1];
        FTFA_FCCOB7 = buf[0];
        
        if(1 == flash_cmd_launch()) 
            return 2;  //写入命令错误
    }
    
    return 0;  //成功执行
}

//======================================================================
//函数名称：flash_read_logic
//函数返回：无
//参数说明：dest：读出数据存放处（传地址，目的是带出所读数据，RAM区）
//       sect：扇区号（范围取决于实际芯片，例如KL36：0~63，每扇区1KB）
//       offset:扇区内部偏移地址（0~1020，要求为0,4，8,12，......）
//       N：读字节数目（4~1024,要求为4，8,12,......）//
//功能概要：读取flash存储器的sect扇区的 offset处开始的N字节，到RAM区dest处
//========================================================================
void flash_read_logic(uint_8 *dest,uint_16 sect,uint_16 offset,uint_16 N)
{
	uint_8 *src;
	src=(uint_8 *)(sect*1024+offset);
	memcpy(dest,src,N);
}


//======================================================================
//函数名称：flash_read_physical
//函数返回：无
//参数说明：dest：读出数据存放处（传地址，目的是带出所读数据，RAM区）
//       addr：目标地址，要求为4的倍数（例如：0x00000004）
//       N：读字节数目（0~1020,要求为4，8,12,......）
//功能概要：读取flash指定地址的内容
//======================================================================
void flash_read_physical(uint_8 *dest,uint_32 addr,uint_16 N)
{	uint_8 *src;
	src=(uint_8 *)addr;
	memcpy(dest,src,N);
}

//======================================================================
//函数名称：flash_protect
//函数返回：无
//参数说明：M：KL25:待保护区域的扇区号入口值，实际保护M~M+3,其中M=0,4,8,......,124
//         KL36(64):待保护区域的扇区号入口值，实际保护M~M+1,其中M=0,2,4,6,......,62
//功能概要：flash保护操作
//说        明：KL25(128)每调用本函数一次，保护4个扇区（M~M+3）128/32=4
//       KL36(64)每调用本函数一次，保护2个扇区（M~M+1） 63/32=2
//======================================================================
void flash_protect(uint_8 M)
{
    uint_8 regionNO;
    //regionNO=M/4;
    regionNO=M/2;
    uint_8 offset;
    offset=regionNO%8;//获得偏移，即保护位号
    uint_8 regionCounter;
    regionCounter=3-regionNO/8;//获得应置位的寄存器号
    switch(regionCounter)
    {
    case 3:     //保护区域[7：0]中某区域（对应[28:0]开始的4个扇区，4扇区对齐）
    	BCLR(offset,FTFA_FPROT3);
        break;
    case 2:     //保护区域[15:8]中某区域（对应[60:32]开始的4个扇区，4扇区对齐）
    	BCLR(offset,FTFA_FPROT2);
        break;
    case 1:     //保护区域[23:16]中某区域（对应[92:64]开始的4个扇区，4扇区对齐）
    	BCLR(offset,FTFA_FPROT1);
        break;
    case 0:     //保护区域[31:24]中某区域（对应[124:96]开始的4个扇区，4扇区对齐）
    	BCLR(offset,FTFA_FPROT0);
        break;
    default:
        break;
    }
}

//======================================================================
//函数名称：flash_isempty
//函数返回：1=目标区域为空；0=目标区域非空。
//参数说明：所要探测的flash区域初始地址
//功能概要：flash判空操作
//======================================================================
uint_8 flash_isempty(uint_8 *buff,uint_16 N)
{
   uint_16 i,flag;
   i = 0;
   flag = 1;
   for(i = 0; i<N; i++)   //遍历区域内字节
   {
      if(buff[i]!=0xff)   //非空
      {
      	  flag=0;
          break;
       }
   }
   return flag;
}

//----------------------以下为内部函数存放处---------------------------------
//======================================================================
//函数名称：flash_cmd_launch
//函数返回：0-成功 1-失败
//参数说明：无
//功能概要：启动Flash命令
//======================================================================
static uint_32 flash_cmd_launch(void)
{
    // 清除访问错误标志位和非法访问标志位
    FTFA_FSTAT = ACCERR | FPVIOL;
    
    // 启动命令
    FTFA_FSTAT = CCIF;

    // 等待命令结束
    while(!(FTFA_FSTAT & CCIF));

    // 检查错误标志
    if(FTFA_FSTAT & (ACCERR | FPVIOL | MGSTAT0))
        return 1 ; //执行命令出错
    return 0; //执行命令成功
}
//======================================================================
