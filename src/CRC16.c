/*
 * CRC16.c
 *
 *  Created on: 24 мая 2018 г.
 *      Author: bekir
 */

#include "CRC16.h"

unsigned short CRC16(unsigned char* buf, int len)
{
unsigned short crc=0xFFFF;
 for (int pos=0; pos<len; pos++)
 {
 crc^=*buf++;
  for (int i=8; i!=0; i--)
  {
    if ((crc&0x0001)!=0)
    {
    crc>>=1;
    crc^=0xA001;
    }
    else crc>>=1;
  }
 }
buf-=len;
return crc;
}



