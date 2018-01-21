#include "timeStamp.h"

//=====================================================================
//函数名称：timeChange
//函数返回：无
//参数说明：ticks：时间戳
//       time：解析后的日期，例:19700101000000(1970-01-01 00:00:00)
//功能概要：时间戳转成字符表示日期
//=====================================================================
void timeChange(uint_64 ticks,uint_8 *time)
{
	struct tm* curTime;
	uint_8 s[100],i;
	time_t t;

	t = (time_t)ticks;
	curTime = gmtime(&t);
	strftime(s,sizeof(s),"%Y%m%d%H%M%S",curTime);

	for(i = 0;i < 14;i++)
	{
		time[i] = s[i] - '0';
	}
}
