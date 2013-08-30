#include "Packet.h"

Packet::Packet(uint8 opcode, uint16 length)
{
	index = 0;
	op = opcode;
	len = length;
	data = new uint8[len];
	memset(data, 0x00, len);
}

Packet Packet::operator<<(uint8 byte)
{
	data[index] = byte;
	index++;
	return *this;
}
Packet Packet::operator<<(uint16 bytes)
{
	data[index] = (uint8)bytes;
	index++;
	data[index] = (uint8)(bytes >> 8);
	index++;
	return *this;
}
Packet Packet::operator<<(uint32 bytes)
{
	data[index] = (uint8)bytes;
	index++;
	data[index] = (uint8)(bytes >> 8);
	index++;
	data[index] = (uint8)(bytes >> 16);
	index++;
	data[index] = (uint8)(bytes >> 24);
	index++;
	return *this;
}
uint8 Packet::operator[](int i)
{
	return data[i];
}

Packet::operator uint8()
{
	return op;
}
Packet::operator uint8*()
{
	return data;
}
Packet::operator int()
{
	return len;
}
