#include "frame.h"

uint_8 frameHead[2] = "V!";//帧头
uint_8 frameTail[2] = "S$";//帧尾

//=====================================================================
//函数名称：frameCreate
//功能概要：实现创建数据帧指针
//参数说明：imsi：发送数据的设备的IMSI号
//函数返回：不为0：成功
//=====================================================================
uint_8* frameCreate()
{
	uint_16 mframeLen = sizeof(struct comData);
	return (uint_8 *)malloc(mframeLen + 19);
}

//=====================================================================
//函数名称：frameEncode
//功能概要：实现待发送数据的组帧,将待发送数据加上帧头、帧长、帧尾以及校验信息
//       frame=帧头+IMSI+帧长+data+CRC校验码+帧尾
//参数说明：imsi：发送数据的设备的IMSI号
//       data:待组帧的结构体数据
//       frame:组帧之后的待发送数据帧，用于发送
//       frameLen:组帧后的数据长度，用于发送
//函数返回：数据长度
//=====================================================================
uint_16 frameEncode(struct comData data, uint_8 _type, uint_8 *frame)
{
	/**
	 * 数据格式，共计19+N字节：
	 * （1）帧头head（2字节）
	 * （2）请求设备requestId（4字节）
	 * （3）节点nodeId（4字节）
	 * （4）帧类型type（1字节）
	 * （5）设备帧号frameId（4字节）
	 * （6）具体数据body（N字节）
	 * （7）校验verifys（2字节）
	 * （8）帧尾tail（2字节）
	 */

	uint_32 requestId = 1; // （2）请求设备requestId
	uint_32 nodeId = 2; // （3）节点nodeId
	uint_8 type = _type; // （4）帧类型type
	uint_32 frameId = 4; // （5）设备帧号frameId
	uint_8 *frameData = 0; // （6）存放数据的指针
	uint_16 crc; // （7）存放CRC的值

	uint_16 len = 0; // 当前帧的填充索引
	uint_16 length = 0; // 数据帧长度length

	uint_8 CRC[2];

	//（1）存放帧头head，2字节
	frame[len++] = frameHead[0]; // 0 len = 1
	frame[len++] = frameHead[1]; // 1 len = 2

	//（2）存放请求设备requestId，4字节
	frame[len++] = requestId >> 24; // 2 len = 3
	frame[len++] = (requestId >> 16) & 0xFF; // 3 len = 4
	frame[len++] = (requestId >> 8) & 0xFF; // 4 len = 5
	frame[len++] = requestId & 0xFF; // 5 len = 6

	//（3）存放节点nodeId，4字节
	frame[len++] = nodeId >> 24; // 6 len = 7
	frame[len++] = (nodeId >> 16) & 0xFF; // 7 len = 8
	frame[len++] = (nodeId >> 8) & 0xFF; // 8 len = 9
	frame[len++] = nodeId & 0xFF; // 9 len = 10

	//（4）存放帧类型type，1字节
	frame[len++] = type; // 10 len = 11

	//（5）存放设备帧号frameId，4字节
	frame[len++] = frameId >> 24; // 11 len = 12
	frame[len++] = (frameId >> 16) & 0xFF; // 12 len = 13
	frame[len++] = (frameId >> 8) & 0xFF; // 13 len = 14
	frame[len++] = frameId & 0xFF; // 14 len = 15

	//（6）存放具体数据body，N字节
	length = sizeof(struct comData);
	frameData = (uint_8 *)(&data);
	// 加len的原因是，前面的数据长度len
	memcpy(frame + len, frameData, length);
	len += length; // 修改当前帧的填充索引 len = 15 + N

	//（7）存放CRC，帧头不算，数据总长度len-2，2字节
	crc = crc16(frame + 2, len - 2);
	//CRC校验码
	frame[len++] = crc >> 8; // 15 + N len = 16 + N
	frame[len++] = crc & 0xFF; // 16 + N len = 17 + N

	//（8）存放帧尾tail，2字节
	frame[len++] = frameTail[0]; // 17 + N len = 18 + N
	frame[len++] = frameTail[1]; // 18 + N len = 19 + N

	return len;
}

//=====================================================================
//函数名称：frameDecode
//功能概要：数据解帧,校验接收到的数据的帧头、帧尾、CRC校验字节，失败反馈错误信息；
//       成功则只留下数据部分data=frame-帧头-IMSI-帧长-CRC校验码-帧尾；
//参数说明：frame:需要解帧的数据
//      cdata：解帧后的结构体数据
//函数返回：0：成功
//=====================================================================
uint_8 frameDecode(uint_8 *frame, struct comData *cdata)
{
	uint_8 CRC[2];
	uint_16 i,crc,dataLen;

	//校验帧头
	if(frame[0] != frameHead[0] || frame[1] != frameHead[1])//帧头不对
		return 1;

	dataLen = (frame[17]<<8)|frame[18];
	//CRC校验
	crc = crc16(frame+2,dataLen+17);
	CRC[0] = (crc>>8)&0xff;
	CRC[1] = crc&0xff;

	if(CRC[0] != frame[dataLen+19] || CRC[1] != frame[dataLen+20])//CRC检验错误
		return 2;
	//校验帧尾
	if(frame[dataLen+21] != frameTail[0] || frame[dataLen+22] != frameTail[1])//帧尾出错
		return 3;

	memcpy(cdata,frame+19,dataLen);

	return 0;
}
