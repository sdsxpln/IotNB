//=====================================================================
//文件名称：uecom.c
//功能概要：UE驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2017-06-04   V1.0
//=====================================================================
#include "uecom.h"

//======================内部函数声明==================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_8 buf[]);
void delay_ms(uint_16 mseconds);
void uint_to_str(uint_32 ch,char *str);
//======================内部函数声明结束================================

//=====================================================================
//			变量声明
//=====================================================================

	//函数中使用到的AT指令
	char AT[]   = "AT\r";									//测试与GPRS模块通讯的指令
	char IPR[] = "AT+IPR=115200\r";							//将模块波特率设置为115200
	char CGSN[] = "AT+CGSN\r";								//获取设备的IMEI号指令
	char CGCLASS_B[] ="AT+CGCLASS=\"B\"\r";					//将卡的类型设置为B型
	char CGDCONT[] ="AT+CGDCONT=1, \"IP\", \"CMNET\"\r";	//将上网类型设置为net，可以上外网
	char CGACT_1[] ="AT+CGACT=1\r";							//激活PDP，并向基站请求分配ip地址
	char CGACT_0[] ="AT+CGACT=0\r";							//关闭PDP
	char QILOCIP[] ="AT+QILOCIP\r";							//获取分配到的IP地址
	char QICLOSE[] = "AT+QICLOSE\r";
	char CIPCLOSE_0[] = "AT+CIPCLOSE=0\r";
	char CIPCLOSE_1[] = "AT+CIPCLOSE=1\r";
	char CIPCLOSE_2[] = "AT+CIPCLOSE=2\r";
	char CIMI[] = "AT+CIMI\r";								//获取设备的IMSI号指令
	char ATE_0[] = "ATE0\r";								//关闭回显
	char CSQ[] = "AT+CSQ\r\n";								//获得信号强度
	//AT命令是否执行成功的标志
	char OK[] = "OK";
	char ERROR[] = "ERROR";

//=====================================================================
//函数名称：uecom_init
//函数返回：0：成功
//      1：AT指令发送失败；2：GPRS模块的波特率设置失败；3：关闭回显失败；4：IMEI查询失败；
//      5：IMSI查询失败；6：设置模块的工作类型为B类失败；7：2：设置联网方式为net失败；8：激活PDP失败；
//      9：获得模块分得的IP地址失败；10：AT指令返回OK，但是并没有获取到IP地址；11:建立TCP连接失败
//参数说明：dest：返回数据的地址，总长74字节，IMEI,15个字节，0~14；IMSI,15个字节，15~29；
//            firmVer，25个字节，30~54；signalPower，2个字节，55~56；bsNum，2个字节，57~58；
//            UEIP，15个字节，59~73
//       IP:管理服务器IP地址
//       PORT:管理服务器端口号
//功能概要：uecom模块初始化
//内部调用：uart_init，uecom_sendCmd，uecom_getIMEI，uecom_getIMSI，
//  	    uecom_link，uecom_config
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT)
{
	//变量声明和赋初值
	uint_16 i;
	uint_8 ret_val ;
	uint_8 flag;

	ret_val  = 0;
	//（1）使用串口与通信模组通信，首先初始化串口
	uart_init(UART_UE, 115200);      //初始化串口1，波特率为115200

	//（2）测试与GPRS模块的通讯
	for(i = 0; i<3; i++)  //最多测试3次
	{
		if(uecom_sendCmd(AT,AT_re_buf) == 0)
		{
			break;
		}
	}
	if(i == 3) goto uecom_init_err1;//通讯失败
	//（3）设置GPRS模块的波特率为115200
	if(uecom_sendCmd(IPR,AT_re_buf)) goto uecom_init_err2;//GPRS模块的波特率设置失败
	//（4）关闭回显（发送AT命令之后，GPRS模块不会把原来的命令返回）
	if(uecom_sendCmd(ATE_0,AT_re_buf)) goto uecom_init_err3;//关闭回显失败
	//（5）查询模块的IMEI,并将IMEI号放在dest数组的前15个字节
	if(uecom_getIMEI(dest)) goto uecom_init_err4;     //IMEI查询失败
	//（6）查询模块的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
	if(uecom_getIMSI(dest+15)) goto uecom_init_err5;  //IMSI查询失败
	//（7）向基站请求联网，附着核心网
	flag = uecom_link(dest+30);
	if(flag) goto uecom_init_err6;   //附着核心网失败
	//（8）建立TCP连接
	flag = uecom_config(IP,PORT);
	if(flag) goto uecom_init_err7;   //建立TCP连接失败

	//至此，没有失败退出，成功！
	ret_val = 0;
	goto uecom_init_exit;        //成功，ret_val=0

	 //错误退出
uecom_init_err1:
	ret_val = 1;                 //AT指令发送失败
	goto uecom_init_exit;
uecom_init_err2:
	ret_val = 2;                 //GPRS模块的波特率设置失败
	goto uecom_init_exit;
uecom_init_err3:
	ret_val = 3;                 //关闭回显失败
	goto uecom_init_exit;
uecom_init_err4:
	ret_val = 4;                 //IMEI查询失败
	goto uecom_init_exit;
uecom_init_err5:
	ret_val = 5;                 //IMSI查询失败
	goto uecom_init_exit;
uecom_init_err6:
	ret_val = 5 + flag;          //附着核心网失败
	goto uecom_init_exit;
uecom_init_err7:
	ret_val = 10 + flag;         //建立TCP连接失败
	goto uecom_init_exit;
	//退出处
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功
//     1：发送失败
//参数说明： data:待发送数据缓存区，传入参数
//		 length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去
//内部调用：uint_to_str，uart_send_string，delay_ms，uart_sendN，
//       uart_disable_re_int，uart_re1，uart_send1，uart_enable_re_int
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
	//变量声明，赋初值
	uint_8 ch,flag,k,l;
	uint_8 ret_val;
	uint_8 dataLen[5];
	uint_8 QISEND[20] = "";   			//设置发送的字节数
	uint_8 buf[50];
	uint_16 oldLen;

	//（1）将需要发送的数据长度发给GPRS模块，并开启发送模式
	uint_to_str(length,dataLen);
	strcat(QISEND,"AT+QISEND=");		//获取需要的AT指令
	strcat(QISEND, dataLen);
	strcat(QISEND, "\r\n");
	uart_send_string(UART_UE, QISEND);	//指定发送的数据字节个数，并进入发送模式
	delay_ms(100);						//等待发送模式成功开启
//	uart_sendN(UART_2,length,data);		//调试时使用
	uart_sendN(UART_UE,length,data);

	//（2）判断发送是否成功，采用查询方式，不使用串口中断
	k = 0;
	l = 0;
	uart_disable_re_int(UART_UE);			//关闭串口UART_UE的中断
	while(1)
	{
		ch = uart_re1(UART_UE, &flag);  	//调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			buf[k++] = ch;
			buf[k] = '\0';
			//uart_send1(UART_2,ch);			//调试时使用
			//判断返回字符串中是否有"OK"
			if(strstr(buf,"OK") != NULL) 	//有"OK"，表示正确返回
			{
				ret_val=0;
				goto uecom_send_exit;
			}
			if(strstr(buf,"ERROR") != NULL) goto uecom_send_err1;//返回"ERROR"，返回错误返回
		}
		else
		{
			l++;
			if (l>=100) goto uecom_send_err2;//超时，没有返回
		}
	}

uecom_send_err1:
	ret_val = 1;
	goto uecom_send_exit;//返回"ERROR"，返回错误返回
uecom_send_err2:
	ret_val = 2;
	goto uecom_send_exit;//超时，没有返回

uecom_send_exit:
	uart_enable_re_int(UART_UE);			//打开串口中断
	return ret_val;
}

//=====================================================================
//函数名称：uecom_recv
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：未接收到
//参数说明：*data:将接收到的数据存放到该数组串中
//功能概要：将数据接收缓冲区的数据读出，并存入data数组之中
//=====================================================================
uint_8 uecom_recv(uint_16 *dataLen, uint_8 *data)
{
	//变量声明和赋初值
	uint_16 i,l;
	uint_8 ret_val;
	uint_8 flag,ch;
	//（1）开始使用查询方式获得串口UART_UE接收到的数据
	uart_disable_re_int(UART_UE);			//关闭串口UART_UE的中断
	l = 0;
	i = 0;
	while(1)
	{
		//接收一个字节
		ch = uart_re1(UART_UE, &flag);  	//调用接收一个字节的函数，清接收中断位
		if(flag)   //有数据
		{
			data[i++] = ch;
		}
		else
		{
			l++;
			if (l>=1000)
				break;
		}
	}
	*dataLen = i;
	if(i == 0)
	{
		ret_val = 1;
		goto uecom_recv_exit;
	}
	else
	{
		ret_val = 0;
		goto uecom_recv_exit;
	}

uecom_recv_exit:
	uart_enable_re_int(UART_UE);
	return ret_val;
}

uint_8 uecom_deInit()
{
	uart_enable_re_int(UART_UE);
	PORTE_PCR0 = PORT_PCR_MUX(0x0);  	 //关闭UART1_TXD
	PORTE_PCR1 = PORT_PCR_MUX(0x0);   	 //关闭UART1_RXD
}

//=====================================================================
//函数名称：uecom_link
//函数返回：0：成功与基站建立连接；
//      1：设置模块的工作类型为B类失败；2：设置联网方式为net失败；
//      3：激活PDP失败；
//      4：获得模块分得的IP地址失败；5：AT指令返回OK，但是并没有获取
//         到IP地址
//参数说明：*dest：存放基站给本模块的ip地址，格式为字符串，中间用"."
//         连接，dest指向的数组至少要有15个字节
//功能概要：建立与基站的连接
//内部调用：uecom_sendCmd，delay_ms，uart_send_string，uecom_getUEIP
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_link(uint_8 *dest)
{
	//变量声明、赋初值
	uint_8 i,index,ret_val,flag;
	uint_8 *str;

	//（1）设置模块的工作类型为B类
	if(uecom_sendCmd(CGCLASS_B,AT_re_buf)) goto uecom_link_err1;//设置模块的工作类型为B类失败
	//（2）设置联网方式为net
	if(uecom_sendCmd(CGDCONT,AT_re_buf)) goto uecom_link_err2;//设置联网方式为net失败
	//（3）激活PDP，并向基站请求分配ip地址
	if(uecom_sendCmd(CGACT_1,AT_re_buf))
	{
		if(uecom_sendCmd(CGACT_0,AT_re_buf))
		{
			delay_ms(1000);
			uecom_sendCmd(CGACT_0,AT_re_buf);
		}
		for(i=0; i<10; i++)
		{
			if(uecom_sendCmd(CGACT_1,AT_re_buf))
			{
				delay_ms(2000);
				uart_send_string(UART_1, CGACT_0);
				delay_ms(2000);
			}
			else
			{
				break;
			}
		}
		if(i == 10) goto uecom_link_err3;//激活PDP失败
	}
	//（4）获得模块分得的IP地址
	flag = uecom_getUEIP(dest);
	if(flag) goto uecom_link_err4;//获得模块分得的IP地址失败

	//至此，没有失败退出，成功！
	ret_val = 0;
	goto uecom_link_exit;

	//错误退出
uecom_link_err1:
    ret_val = 1;                   //设置模块的工作类型为B类失败
    goto uecom_link_exit;
uecom_link_err2:
	ret_val = 2;                   //设置联网方式为net失败
	goto uecom_link_exit;
uecom_link_err3:
	ret_val = 3;                   //激活PDP失败
	goto uecom_link_exit;
uecom_link_err4:
	ret_val = 3 + flag;            //获得模块分得的IP地址失败
	goto uecom_link_exit;
    //退出处
uecom_link_exit:
	return ret_val;
}

//=====================================================================
//函数名称：uecom_config
//函数返回：  0：成功建立TCP连接；
//       1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址
//        port:待连接服务器的端口号
//功能概要：与指定的服务器端口建立TCP连接
//内部调用：uecom_sendCmd，uart_send_string，delay_ms
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port)
{
	//变量声明和赋初值
	uint_8 flag,i,k,ch;
	uint_8 ret_val;
	uint_8 configFlag;
	uint_8 QIOPEN[50] = "";				//建立TCP连接的地址和端口
	uint_8 buf[50];

	//（1）组建AT指令
	strcat(QIOPEN,"AT+QIOPEN=\"TCP\",\"");
	strcat(QIOPEN,ip);
	strcat(QIOPEN,"\",\"");
	strcat(QIOPEN,port);
	strcat(QIOPEN,"\"\r\n");
	//（2）建立TCP连接
	flag = uecom_sendCmd(QIOPEN,AT_re_buf);
	if(flag)
	{
		uart_send_string(UART_UE, QICLOSE);
		delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_0);
		delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_1);
		delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_2);
		delay_ms(500);
		flag = uecom_sendCmd(QIOPEN,AT_re_buf);
		if(flag)
		{
			ret_val = 1;
			goto uecom_config_exit;
		}
	}
	ret_val = 0;
	//退出处
uecom_config_exit:
	return ret_val;
}

//==================================================================
//函数名称：uecom_getIMEI
//函数返回：0：获取IMEI成功
//      1：获取IMEI失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMEI(uint_8 *dest)
{
	//变量声明和赋初值
	uint_8 ret_val;
	uint_8 i,index;

	dest[15] = 0;				//字符串的结尾符

	//（1）获取IMEI号
	if(uecom_sendCmd(CGSN,AT_re_buf))
	{
		ret_val = 1;
		goto uecom_getIMEI_exit;                //获取IMEI号失败
	}
	//（2）将IMEI号存储在dest数组中
	i = 0;
	index = 0;
	while( (AT_re_buf[i] < '0') || (AT_re_buf[i] > '9'))//去除缓冲区中不是数字的部分
	{
		i++;
	}
	while(AT_re_buf[i] != '\n')
	{
		if(AT_re_buf[i]>='0' && AT_re_buf[i]<='9')
		{
			dest[index++] =AT_re_buf[i];
		}
		i++;
	}
	ret_val = 0;
	//退出处
uecom_getIMEI_exit:
	return ret_val;
}

//==================================================================
//函数名称：uecom_getIMSI
//函数返回：0：获取IMSI成功
//      1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMSI(uint_8 *dest)
{
	//（变量声明和赋初值
	uint_8 ret_val;
	uint_8 i,k,j;

	dest[14] = 0;

	//（1）获取IMSI号
	for(i = 0;i < 3;i++)
	{
		if(!uecom_sendCmd(CIMI,AT_re_buf))
		{
			break;

		}
		else
		{
			if(i < 3)
			{
				Delay_ms(3000);
				continue;
			}
			ret_val = 1;
			goto uecom_getIMSI_exit;     //获取IMSI号失败
		}
	}

	//（2）将IMSI号存储在dest数组中
	i = 0;
	k = 0;
	while(AT_re_buf[i++] != '\n'); // && AT_re_buf[i] != '\r')
	//	{
	//		++i;
	//	}
	while(AT_re_buf[i] != '\n' && AT_re_buf[i] != '\r')
	{
		if( (AT_re_buf[i] >= '0' && AT_re_buf[i] <= '9'))
		{
			dest[k++] = AT_re_buf[i++];
		}
		else
		{
			i++;
		}
	}
	ret_val = 0;
	//退出处
uecom_getIMSI_exit:
	return ret_val;
}

//===================================================================
//函数名称：uecom_getFirmVer
//函数返回：0：获取固件版本号成功
//      1：获取固件版本号失败
//参数说明：firmVer：存放返回的固件版本号，25位
//功能概要：获取固件版本号，用于统一接口，并没有实现
//==================================================================
uint_8 uecom_getFirmVer(uint_8 *firmVer)
{
	uint_8 i;
	for(i = 0; i<25; i++)
	{
		firmVer[i] = 0;
	}
}

//=====================================================================
//函数名称：uecom_getSignalPower
//函数返回：0：获取基站信号强度成功
//      1：获取基站信号强度失败
//参数说明：signalPower：存放返回的基站信号强度号，2位
//功能概要：获取基站信号强度
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getSignalPower(uint_16 *signalPower)
{
	//变量声明和赋初值
	uint_8 i,ret_val;

	//（1）获取信号强度
	if( uecom_sendCmd(CSQ, AT_re_buf))
	{
		ret_val = 1;
		goto uecom_getSignalPower_exit;
	}

	//（2）提取出信号强度，转为int_16格式，并赋给*signalPower
	i =0;
	while(AT_re_buf[i++] != ':');
	i++;//略过空格
	if(AT_re_buf[i] >= '0' && AT_re_buf[i] <= '9')
	{
		*signalPower = (AT_re_buf[i]-'0');
		i++;
	}
	if(AT_re_buf[i] >= '0' && AT_re_buf[i] <= '9')
	{
		*signalPower = *signalPower*10 + (AT_re_buf[i]-'0');
	}
	ret_val = 0;
	//退出处
uecom_getSignalPower_exit:
	return ret_val;
}

//=====================================================================
//函数名称：uecom_getUEIP
//函数返回：0：获取基站分配UE的IP地址成功
//      1：获取基站分配UE的IP地址失败
//参数说明：ip：存放返回基站分配UE的IP地址，15位
//功能概要：获取基站分配UE的IP地址
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getUEIP(uint_8 *ip)
{
	//变量声明和赋初值
	uint_8 i, index;
	uint_8 ret_val;

	//（1）将ip数组中的内容清空
	for(i = 0; i<=15; i++)
	{
		ip[i] = 0;
	}
	//（2）获得模块分得的IP地址
	if(uecom_sendCmd(QILOCIP,AT_re_buf)) goto uecom_getUEIP_err1;

	//（3）读取获得的IP地址
	index = 0;//记录接收到的ip地址的位数，防止返回OK，却没有ip地址的情况
	i = 0;
	//寻找到AT_return_buff中第一个数字的下标
	while(AT_re_buf[i] < '0' || AT_re_buf[i] > '9')
	{
		i++;
	}
	//（4）将读取到的ip地址存储到输出数组dest中
	while((AT_re_buf[i] >= '0' && AT_re_buf[i] <= '9') || AT_re_buf[i] == '.')
	{
		ip[index++] = AT_re_buf[i++];
	}
	if(index == 0)	goto uecom_getUEIP_err2;			//虽然返回OK，但是，并没有获取到IP地址


	//至此，没有失败退出，成功！
	ret_val = 0;
	goto uecom_getUEIP_exit;        //成功，ret_val=0

uecom_getUEIP_err1:
	ret_val = 1;
	goto uecom_getUEIP_exit;//获得模块分得的IP地址失败
uecom_getUEIP_err2:
	ret_val = 2;
	goto uecom_getUEIP_exit;//AT指令返回OK，但是并没有获取到IP地址
	//退出处
uecom_getUEIP_exit:
	return ret_val;
}

//=====================================================================
//函数名称：uecom_getBSNum
//函数返回：0：获取基站号成功
//       1：获取基站号失败
//参数说明：bsNum：存放返回的基站号，2位
//功能概要：获取基站号
//=====================================================================
uint_8 uecom_getBSNum(uint_32 *bsNum)
{
	*bsNum = 0;
	return 0;
}



//=====================================================================
//===================以下为内部函数========================================
//=====================================================================

//=====================================================================
//函数名称：uecom_sendCmd
//函数返回：0：指令发送成功;1：指令发送失败
//参数说明：cmd：需要发送的AT指令的首地址;buf[]:传地址，带回AT指令返回结果
//功能概要：发送AT指令并获取返回信息
//内部调用：strTrim
//修改日期：2017.06.30，WYH,2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_8 buf[])
{
	//变量声明和赋初值
	uint_8 i,ch;
	uint_32 j,k,l;
	uint_8 flag,reflag;

	//（1）关闭串口中断
	uart_disable_re_int(UART_UE);

	//（2）向UE最多发送3次AT指令，UE返回“OK”即成功则跳出循环
	for(i = 0 ; i <3 ; i++)
	{
//		LCDShowRunMsg('H',9,i);            		//UE初始化成功提示
//		for(j = 0;j < 1100;j++)	buf[j] = 0;  	//清缓冲区
		ATSendFlag = 1;
		//uart_send_string(UART_2,cmd);			//串口调试时使用
		uart_send_string(UART_UE,cmd);    		//通过串口发送AT指令
		k=0;
		l=0;
		//循环等待串口接收到的数据
		while(1)
		{
			//接收一个字节
			ch = uart_re1(UART_UE, &flag);  	//调用接收一个字节的函数，清接收中断位

			if(flag)   //有数据
			{
				buf[k++] = ch;
				buf[k] = '\0';
				//判断返回字符串中是否有"OK"
				if(strstr(buf,"OK") != NULL)  	//有"OK"，表示正确返回
				{
					reflag=0;
					//uart_send_string(UART_2,buf);
//					strTrim(buf);              	//去除返回字符串中的'\r'和'\n'
					goto uecom_sendCmd_exit;
				}
				if  (strstr(buf,"ERROR") != NULL) break;
			}
			else
			{
				l++;
				if (l>=100)  break;
			}
		}
	}
	//（3）3次AT指令未成功
	reflag=1;	//错误，返回标志1
uecom_sendCmd_exit:
	//（4）开放UE接收中断
	uart_enable_re_int(UART_UE);
	delay_ms(100);								//防止连续发送AT指令导致模块死机
	return reflag;
}

//======================================================================
//函数名称：delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void delay_ms(uint_16 mseconds)
{
	uint_16 i;
	uint_32 j;
	for(i = 0; i<mseconds; i++)
	{
		for(j = 0; j<4200; j++);
	}
}

//======================================================================
//函数名称：uint_to_str
//函数返回：无
//参数说明：ch:带转换的正整数		*str：存放转换后的字符串
//功能概要：将无符号整数转换为字符串
//======================================================================
void uint_to_str(uint_32 ch,char *str)
{
	uint_8 i=0;
	char tmp[5];
	uint_32 n;
	if(ch == 0)
	{
		tmp[i++] = '0';
	}
	else
	{
		n = ch % 10;
		while(n>0)
		{
			tmp[i++] = n + '0';
			ch /= 10;
			n = ch % 10;
		}
	}
	tmp[i] = '\0';
	for(i = 0;i < strlen(tmp);i++)
	{
		str[i] = tmp[strlen(tmp)-i-1];
	}
	str[i] = '\0';
}
