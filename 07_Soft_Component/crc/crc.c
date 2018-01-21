#include "crc.h"

uint_16 crc16(uint_8 *ptr,uint_16 len)
{
	uint_16 i,j,tmp,crc16;

	crc16 = 0xffff;
	for(i = 0;i < len;i++)
	{
		crc16 = ptr[i]^crc16;
		for(j = 0;j< 8;j++)
		{
			tmp = crc16 & 0x0001;
			crc16 = crc16>>1;
			if(tmp)
				crc16 = crc16^0xa001;
		}
	}

	return crc16;
}
