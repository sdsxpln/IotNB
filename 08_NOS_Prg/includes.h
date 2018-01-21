//======================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版本更新：2017-04-07  V1.0
//======================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include "adc.h"
#include "gpio.h"
#include "lcd.h"
#include "lptmr.h"
#include "tsi.h"
#include "uart.h"
#include "light.h"
#include "crc.h"
#include "printf.h"
#include "frame.h"
#include "uecom.h"
#include "timeStamp.h"
#include "flash.h"
#include "systick.h"
#include "power_mode.h"


//定义全局变量
#define TSI_VALUE     0x00CC0011       //0x00640033
#define sectorNo 50           //擦除、写入、（逻辑地址）读取、保护实验所使用扇区号
#define Offset 0              //擦除、写入、（逻辑地址）读取、保护实验所使用扇区内偏移地址
#define CNT 472               //擦除、写入、（逻辑地址）读取、保护实验所使用字节数

struct comData cd;

uint_8 isSendData; // 发送中断数据标记位
uint_8 isReceiveData; // 接收中断数据标记位
uint_8 *mframe;					// 需要发送或接收的帧数据
uint_16 mframeLen;				// 发送或接收的帧数据长度

uint_8 isFrameHead; // 判断是否是头帧
uint_8 isBeginFrame; // 开始接收数据帧
uint_8 isFrameTail; // 判断是否是尾帧

#endif
