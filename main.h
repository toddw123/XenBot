#ifndef __MAIN_H__
#define __MAIN_H__


#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <queue>
#include <hash_map>
#include <vector>
using namespace std;



#include "detours\detours.h"
#pragma comment(lib, "detours\\detours.lib")

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;

//These are the function prototypes
typedef int (__thiscall* tDecrypt)(void* pthis, uint8* packet, int length);
typedef int (__thiscall* tEncrypt)(void* pthis, uint8* packet, int len);
typedef int (__cdecl *tSendPacket)(uint8 opcode, uint8* packet, int len);

tDecrypt oDecrypt = NULL;
tEncrypt oEncrypt = NULL;
tSendPacket oSendPacket = NULL;

int __fastcall encryptHook(void* pthis, void* unk, uint8* packet, int len);
int __fastcall decryptHook(void* pthis, void* unk, uint8* packet, int len);


void createConsole();

struct PDATA{
	uint8* packet;
	int len;
};

queue<PDATA> inPackets;
DWORD WINAPI InPacketsProcesser(LPVOID lpBuffer);
BOOL bRunning = false;
BOOL bShowLog = true;

void handleUpdatePacket(uint8* data, int len);

struct InventorySlot{
	uint16 itemId;
	uint16 flags; //Usually the quantity of the item
};

struct DropScreen{
	InventorySlot slots[50];
	uint32 money;
	uint8 unk;
	uint8 unk2;
	uint16 checksum;
};

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

BOOL DLL_EXPORT APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__