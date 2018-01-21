//===========================================================================
//文件名称：systick.c
//功能概要：systick定时器模块构件源文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2016-3-20   V4.0
//===========================================================================
#include "systick.h"

//===========================================================================
//函数名称：systick_init
//函数返回：无
//参数说明：clk_src_sel：时钟源选择：1：内核时钟（CORE_CLK_KHZ）;
//                             0：内核时钟/16。
//         int_ms:中断的时间间隔。单位ms 推荐选用5,10,......,最大为50
//功能概要：初始化SysTick模块，设置中断的时间间隔
//说    明：内核时钟频率SYSTEM_CLK_KHZ宏定义在common.h中
//       systick以ms为单位，最大可为349（2^24/48000，向下取整），合理范围1~349。前提
//时钟是内核时钟，为48000000Hz。假如时钟频率升高，合理范围会缩小。
//KL25的SysTick时钟源可以是内核时钟，也可设置为内核时钟的16分频。
//24位计数器，减1计数
//时间范围：1ms~349ms（内核时钟）;1ms~5592ms（内核时钟的16分频）
//===========================================================================
void systick_init(uint_8 clk_src_sel, uint_8 int_ms)
{
    SysTick->CTRL = 0;   //设置前先关闭systick
    SysTick->VAL  = 0;   //清除计数器

    //根据计数频率，确定并设置重载寄存器的值
    if(0==clk_src_sel)              //0:内核时钟/16
    {
        if((int_ms<1)&&(int_ms>5592))
        {
            int_ms = 10;
        }
        SysTick->LOAD = SYSTEM_CLK_KHZ*int_ms/16;
    }
    else                            //1:内核时钟
    {
        if((int_ms<1)&&(int_ms>349))
        {
            int_ms = 10;
        }
        SysTick->LOAD = SYSTEM_CLK_KHZ*int_ms;
        SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk);
    }
    //设定 SysTick优先级为3(SHPR3寄存器的最高字节=0xC0)
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    //设置时钟源,允许中断,使能该模块,开始计数
    SysTick->CTRL |= ( SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk );
}



