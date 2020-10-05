#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "string.h"

#define REG_HOLDING_START 0X0001 
#define REG_HOLDING_NREGS 0X1000
extern u16 Tx[1025];
extern u16 Rx[1025];

union {
	float a;
	unsigned short b[2];
}fp;

eMBErrorCode    eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNRegs)
{
 return MB_ENOERR;
}

/* 读写保持寄存器功能函数这个是被mbfuncholding.c文件中的eMBFuncReadHoldingRegister()
	 函数等调用，用户自己修改以返回用户需要的地址上的数据 */
eMBErrorCode    eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                 USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	
	u16 i=0;
	if( ((u16)usAddress >= REG_HOLDING_START) \
		&& ((usAddress + usNRegs) <= REG_HOLDING_START + REG_HOLDING_NREGS) )
	{
		//memcpy(Rx,Tx,sizeof(Rx[1024]));
		i = usAddress;
		switch(eMode)
		{
			case MB_REG_READ:
				while(usNRegs > 0){
					/**pucRegBuffer++ = (u8)(Tx[i] >> 8) ;
					*pucRegBuffer++ = (u8)(Tx[i] &0xff);*/
					
					fp.a = 100.0f;
					*pucRegBuffer++ = (u8)(fp.b[0] >> 8);
					*pucRegBuffer++ = (u8)(fp.b[0]&0xff);
					*pucRegBuffer++ = (u8)(fp.b[1] >> 8) ;
					*pucRegBuffer++ = (u8)(fp.b[1] &0xff);
					//
					i++;
					//usNRegs--;
					i++;
					usNRegs--;
				}
			break;
			case MB_REG_WRITE:
				if(usNRegs > 0)
				{
					while(usNRegs > 0){
						Rx[i] = *pucRegBuffer++ << 8;
						Rx[i] |= *pucRegBuffer++ ;
						i++;
						usNRegs--;
					}
				}
			break;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}
																
eMBErrorCode    eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNCoils, eMBRegisterMode eMode )
{
	return MB_ENOERR;
}

eMBErrorCode    eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                  USHORT usNDiscrete )
{
	return MB_ENOERR;
}

