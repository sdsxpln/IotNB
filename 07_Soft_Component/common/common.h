//=====================================================================
//文件名称：common.h
//功能概要：公共要素头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2017-04-07   V1.0
//=====================================================================
#ifndef _COMMON_H    //防止重复定义（_COMMON_H  开头)
#define _COMMON_H

//1．芯片寄存器映射文件及处理器内核属性文件
#include "MKL36Z4.h"      // 包含芯片头文件
#include "core_cm0plus.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "system_MKL36Z4.h"

//2．定义开关总中断
#define ENABLE_INTERRUPTS   __enable_irq()  //开总中断
#define DISABLE_INTERRUPTS  __disable_irq()  //关总中断

//3．位操作宏函数（置位、清位、获得寄存器一位的状态）
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //清寄存器的一位
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //获得寄存器一位的状态
#define BSET(bit,Register)  ((Register) |= (1<<(bit)))    //置寄存器的一位

//4．重定义基本数据类型（类型别名宏定义）
typedef unsigned char        uint_8;   // 无符号8位数，字节
typedef unsigned short int   uint_16;  // 无符号16位数，字
typedef unsigned long int    uint_32;  // 无符号32位数，长字
typedef unsigned long long   uint_64;
typedef char                 int_8;    // 有符号8位数
typedef short int            int_16;   // 有符号16位数
typedef int                  int_32;   // 有符号32位数
typedef long long            int_64;
//不优化类型
typedef volatile uint_8      vuint_8;  // 不优化无符号8位数，字节
typedef volatile uint_16     vuint_16; // 不优化无符号16位数，字
typedef volatile uint_32     vuint_32; // 不优化无符号32位数，长字
typedef volatile int_8       vint_8;   // 不优化有符号8位数
typedef volatile int_16      vint_16;  // 不优化有符号16位数
typedef volatile int_32      vint_32;  // 不优化有符号32位数

//5. 定义系统使用的时钟频率
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // 芯片系统时钟频率(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // 芯片总线时钟频率(KHz)

//============================================================================
//函数名称：enable_irq
//函数返回：无
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：使能irq中断
//============================================================================
void enable_irq (IRQn_Type irq);

//============================================================================
//函数名称：disable_irq
//函数返回：无
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：禁止irq中断
//============================================================================
void disable_irq (IRQn_Type irq);

//======================================================================
//函数名称：Delay_us
//函数返回：无
//参数说明：无
//功能概要：延时 - 微秒级
//======================================================================
void Delay_us(uint16_t u16us);

//======================================================================
//函数名称：Delay_us
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void Delay_ms(uint16_t u16ms);

//===========================================================================
//函数名称：SecAdd1
//函数返回：无
//参数说明：*p:为指向一个时分秒数组p[3]
//功能概要：秒单元+1，并处理时分单元（00:00:00-23:59:59)
//===========================================================================
void SecAdd1(uint_8 *p);

#endif //防止重复定义（_COMMON_H  结尾)
