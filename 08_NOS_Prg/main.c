//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//======================================================================

#include "includes.h"   //包含总头文件

// 获得光敏数据
uint_16 getLightSensor();
// 获得MCU温度数据
uint_16 getMCUTemp();

int main(void)
{
	uint_8 mflag = 0;					// 指令执行是否成功，0表示成功，非0表示不成功
	uint_8 mRetdata[100] = {0};			// 存放uecom初始化返回的结果
	uint_16 signalPower = 0;			// 获取信号强度
	uint_16 lightSensor = 0;			// 获取光敏值
	uint_16 mcuTemp = 0;				// 获取MCU温度值
	uint_8 lightErrCount = 0;			// 光敏数据发送错误次数
	uint_8 mcuErrCount = 0;				// MCU数据发送错误次数
	uint_8 serverIp[] = "39.104.87.214";	// 服务器IP
	uint_8 serverPort[] = "8080";				// 服务器端口

	isSendData = 0;						// 没有发送数据
	isReceiveData = 0;					// 没有接收数据

	//（1）关总中断
	DISABLE_INTERRUPTS;

	//（2）初始化外设模块
	light_init(LIGHT_RED, LIGHT_ON);	// 初始化红灯，默认暗
	gpio_init(PTE_NUM | 22, 1, 1);		// 默认给不通信模块供电
	LCDInit();							// LCD初始化
	lptmr_init(500);					// LPTMR计时器初始化为10毫秒
	uart_init(UART_1, 115200);			// 初始化串口1，波特率115200
	uart_init(UART_2, 115200);			// 初始化串口2，波特率115200
	tsi_init(3, TSI_VALUE);				// 初始化触摸按键TSI（本产品使用通道3）,设定触发阈值

	//（3）创建数据帧指针
	mframe = frameCreate();

	//（4）使能模块中断
	enable_lptmr_int();					// 开启LPTMR计时器中断
	tsi_enable_re_int();				// 开TSI中断
	uart_enable_re_int(UART_2);			// 使能串口2接收中断

	//（5）开总中断
	ENABLE_INTERRUPTS;   //开总中断

	//（6）UE模块供电
	printf("Enter Send And Receive Operate\n");
	LCDShowRunMsg('H', 1, 1001);								// LCD提示“H1-1001”,表示给UE供电
	printf("H1-1001, Power On NB-IOT Module\n");
	gpio_set(PTE_NUM | 22, 1);									// UE模块供电
	Delay_ms(2000);

	//（7）UE模块初始化
	LCDShowRunMsg('H', 1, 1002);								// LCD提示“H1-1002”,表示开始初始化
	printf("H1-1002, Initialize NB-IOT Module\n");
//	mflag = uecom_init(mRetdata, "39.104.87.214", "7280");		// 设置服务器IP和端口号
//	mflag = uecom_init(mRetdata, "139.129.39.20", "8080");		// 设置服务器IP和端口号
	mflag = uecom_init(mRetdata, serverIp, serverPort);			// 设置服务器IP和端口号

	//（8）根据初始化是否成功，决定是否发送数据
	if(mflag)													// 初始化失败，LCD显示提示
	{
		LCDShowRunMsg('F', 1, 1000 + mflag);					// LCD提示“F1-10xx”,uecom初始化失败提示
		printf("F1-%d, UE Initialize Fail\n",1000+mflag);

		gpio_set(PTE_NUM | 22, 0);								// UE模块断电
		printf("NB-IOT Module Power Off\n");

		while(1);
	}
	LCDShowRunMsg('H', 1, 1003);								// LCD提示“H1-1003”,表示UE初始化成功
	printf("H1-1003, Initialize NB-IOT Module Success\n");
	Delay_ms(2000);

	//（9）获取信号强度
	uecom_getSignalPower(&signalPower);							// 获取信号强度
	signalPower = signalPower * 100 / 31;						// 计算信号强度
	LCDShowRunMsg('H', 1, signalPower);							// LCD显示当前信号强度
	printf("NB-IOT Module Signal Power: %d\n", signalPower);
	Delay_ms(2000);

	while(1)
	{
		if(isSendData)
		{
			//（10）UE模块发送数据
			LCDShowRunMsg('H', 1, 1004);						// LCD提示“H1-1004”,表示组帧提示
			printf("H1-1004, Encode Frame\n");

			while(1)
			{
				lightSensor = getLightSensor();					// 获取光敏值
				cd.body[0] = 1;									// 数据类型
				cd.body[1] = lightSensor >> 8;
				cd.body[2] = lightSensor & 0xFF;
				mframeLen = frameEncode(cd, mframe);			//组帧操作
				LCDShowRunMsg('H', 1, 1005);					//LCD提示“H1-1005”,表示开始发送数据
				printf("H1-1005, NB-IOT Module Send Light Data\n");
				mflag = uecom_send(mframeLen, mframe);			//发送帧操作

				//（11）LCD提示“F1-20xx”,数据发送失败提示
				if(mflag)
				{
					LCDShowRunMsg('F', 1, 2500 + mflag);
					printf("F2-%d, UE Send Light Data Fail\n", 2000 + mflag);
					if(lightErrCount >= 10)							// 发送数据失败10次就把GPRS重启
					{
						while(1)
						{
							gpio_set(PTE_NUM | 22, 0);				// GPRS断电
							Delay_ms(2000);
							gpio_set(PTE_NUM | 22, 1);				// GPRS上电
							Delay_ms(2000);
							mflag = uecom_init(mRetdata, serverIp, serverPort);			// 设置服务器IP和端口号
							if(mflag == 0)													// 初始化失败，LCD显示提示
							{
								break;
							}
						}

					}
					else
					{
						lightErrCount++;
					}
					Delay_ms(2000);
				}
				else
				{
					lightErrCount = 0;
					break;
				}
			}
			LCDShowRunMsg('H', 1, 1006);						//LCD提示“H1-1006”,表示UE发送成功
			printf("H1-1006, NB-IOT Module Send Light Data Success\n");
			Delay_ms(2000);

			while(1)
			{
				mcuTemp = getMCUTemp();							// 获取MCU温度值
				cd.body[0] = 2;									// 数据类型
				cd.body[1] = mcuTemp >> 8;
				cd.body[2] = mcuTemp & 0xFF;
				mframeLen = frameEncode(cd, mframe);			//组帧操作
				LCDShowRunMsg('H', 1, 1007);					//LCD提示“H1-1007”,表示开始发送数据
				printf("H1-1005, NB-IOT Module MCU Light Data\n");
				mflag = uecom_send(mframeLen, mframe);			//发送帧操作

				//（11）LCD提示“F1-20xx”,数据发送失败提示
				if(mflag)
				{
					LCDShowRunMsg('F', 1, 2700 + mflag);
					printf("F2-%d, UE Send MCU Data Fail\n", 2000 + mflag);
					if(mcuErrCount >= 10)							// 发送数据失败10次就把GPRS重启
					{
						while(1)
						{
							gpio_set(PTE_NUM | 22, 0);				// GPRS断电
							Delay_ms(2000);
							gpio_set(PTE_NUM | 22, 1);				// GPRS上电
							Delay_ms(2000);
							mflag = uecom_init(mRetdata, serverIp, serverPort);			// 设置服务器IP和端口号
							if(mflag == 0)													// 初始化失败，LCD显示提示
							{
								break;
							}
						}

					}
					else
					{
						mcuErrCount++;
					}
					Delay_ms(2000);
				}
				else
				{
					mcuErrCount = 0;
					break;
				}
			}

			LCDShowRunMsg('H', 1, 1008);						//LCD提示“H1-1008”,表示UE发送成功
			printf("H1-1006, NB-IOT Module Send MCU Data Success\n");

			isSendData = 0;										// 发送数据定时器开始计数
		}

		if(isReceiveData)
		{
			printf("H1-1006, NB-IOT Module Receive Data Length %d\n", mframeLen);
			printf("Receive Data %d %d %d %d %d %d %d %d %d %d\n",
					mframe[0], mframe[1],
					mframe[2], mframe[3], mframe[4], mframe[5],
					mframe[6], mframe[7], mframe[8], mframe[9]);

			mframeLen = 0;
			isReceiveData = 0;
		}
	}
}

// 获得光敏数据
uint_16 getLightSensor()
{
	uint_16 tmpAD = 0;

	//进主循环之前显示设备温度和IMSI号
	adc_init(MUXSEL_A, 0, 16, SAMPLE32);   //初始化，通道组、单端输入，采样精度，硬件均值
	tmpAD = adc_read(7);

	return tmpAD;
}

// 获得MCU温度数据
uint_16 getMCUTemp()
{
	uint_16 tmpAD = 0;

	//进主循环之前显示设备温度和IMSI号
	adc_init(MUXSEL_A, 0, 16, SAMPLE32);   //初始化，通道组、单端输入，采样精度，硬件均值
	tmpAD = 25.0 - (adc_read(26) * 3.3 * 1000 / 65535 - 719) / 1.715;

	return tmpAD;
}

