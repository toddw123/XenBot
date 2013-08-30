#ifndef __PACKET_H__
#define __PACKET_H__

#include <Windows.h>
#include <stdio.h>

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

class Packet{
private:
	uint8 op;
	uint8* data;
	uint16 len;
	uint16 index;
public:
	Packet(uint8 opcode, uint16 length);

	Packet operator<<(uint8 byte);
	Packet operator<<(uint16 bytes);
	Packet operator<<(uint32 bytes);

	uint8 operator[](int i);

	operator uint8();
	operator uint8*();
	operator int();
};


#endif