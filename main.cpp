#include "main.h"
#include "UnitMap.h"
#include "Player.h"

typedef int (__cdecl* tUL)();
tUL g_updateLightHook = (tUL)0x431A00;

float* LocX = (float*)0x10F9D04;
float* LocY = (float*)0x24CC578;

int start = 1;

int __cdecl UpdateLightHook()
{
   int ret = g_updateLightHook();
   float* test = (float*)0x2EE9670;
   *test = 1;
   return ret;
}

LONG WINAPI UnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo)
{
	FILE* out = fopen("errors.log", "a+");
	fprintf(out, "Error!\n");
	fclose(out);

	return EXCEPTION_CONTINUE_SEARCH;
}

Player *plr;

DWORD WINAPI bot(LPVOID lpBuffer);

//These two functions have been posted across forums everywhere, not sure who i could give credit to on these
bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(;*szMask;++szMask,++pData,++bMask)
		if(*szMask=='x' && *pData!=*bMask)   return 0;
	return (*szMask) == NULL;
}
DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for(DWORD i=0; i<dwLen; i++)
		if (Compare((BYTE*)(dwAddress+i),bMask,szMask))  return (DWORD)(dwAddress+i);
	return 0;
}

BOOL key[256];
DWORD WINAPI InputHandler(LPVOID lpBuffer)
{
   while(true)
   {
      if(GetAsyncKeyState(VK_F11) & 0x8000)
      {
		  key[VK_F11] = true;
      }
	  else
	  {
		  if(key[VK_F11])
		  {
			  CreateThread(0, 0, bot, 0, 0, 0);
			  key[VK_F11] = false;
		  }
	  }
      Sleep(10);
   }
   return 0;
}


BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);

		createConsole();

		UnitMap::Initialize();

		uint8 SendPacketNeedle[] = {0x81, 0x3D, 0x00, 0x00, 0x00, 0x03, 0x00, 0x96, 
									0x00, 0x00, 0x0F, 0x85, 0xAC, 0x00, 0x00, 0x00,
									0xA1, 0x00, 0x00, 0x00, 0x01, 0xC1, 0xE0, 0x06, 
									0x0F, 0xB7, 0x88, 0x00, 0x00, 0x00, 0x00, 0x56};
		char SendPacketMask[] = "xx???x?x??xxxxxxx???xxxxxxx???xx";

		uint8 EncryptNeedle[] = {0x8B, 0x44, 0x24, 0x04, 0x56, 0x8B, 0xF1, 0x89, 
								 0x46, 0x10, 0x80, 0x30, 0xAA, 0x8B, 0x46, 0x10,
								 0x80, 0x38, 0xAA, 0x0F, 0x84, 0xA5, 0x00, 0x00, 
								 0x00, 0x57, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x25};
		char EncryptMask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxx????xx";

		uint8 DecryptNeedle[] = {0x56, 0x8B, 0x74, 0x24, 0x08, 0x89, 0x71, 0x10, 
								 0x0F, 0xB6, 0x16, 0x0F, 0xB6, 0x46, 0x01, 0x03, 
								 0xC2, 0x8B, 0x51, 0x28, 0x25, 0xFF, 0x00, 0x00, 
								 0x00, 0x89, 0x41, 0x04, 0x0F, 0xB6, 0x04, 0x10};
		char DecryptMask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


		//dwModuleStartAddr is set as the encrypt address for 1.191
		//sendpacket/encrypt/decrypt should never be before this address ever again
		//Between 1.145 & 1.191, addresses shifted about 0xB320 
		DWORD  dwModuleStartAddr = (DWORD)GetModuleHandle("Xenimus.exe") + 0x26600;

		DWORD addrSendPacket	= FindPattern(dwModuleStartAddr, 0xA000, SendPacketNeedle, SendPacketMask);
		oSendPacket = (tSendPacket)addrSendPacket;

		DWORD addrEncrypt		= FindPattern(dwModuleStartAddr, 0xA000, EncryptNeedle, EncryptMask);
		oEncrypt = (tEncrypt)addrEncrypt;

		DWORD addrDecrypt		= FindPattern(dwModuleStartAddr, 0xA000, DecryptNeedle, DecryptMask);
		oDecrypt = (tDecrypt)addrDecrypt;


		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		printf("creating encrypt detour %x -> %x\n", addrEncrypt, encryptHook);
		DetourAttach(&(PVOID&)oEncrypt, encryptHook);
		printf("creating decrypt detour %x -> %x\n", addrDecrypt, decryptHook);
		DetourAttach(&(PVOID&)oDecrypt, decryptHook);
		printf("creating UpdateLight detour %x -> %x\n", g_updateLightHook, UpdateLightHook);
		DetourAttach(&(PVOID&)g_updateLightHook, UpdateLightHook);
		DetourTransactionCommit();

		bRunning = true;
		CreateThread(0, 0, InPacketsProcesser, 0, 0, 0);
		CreateThread(0, 0, InputHandler, 0, 0, 0);

		plr = new Player;
		plr->Initialize(addrSendPacket);
	}
	if(fdwReason == DLL_PROCESS_DETACH)
	{
		bRunning = false;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oEncrypt, encryptHook);
		DetourDetach(&(PVOID&)oDecrypt, decryptHook);
		DetourDetach(&(PVOID&)g_updateLightHook, UpdateLightHook);
		DetourTransactionCommit();

		delete plr;
		UnitMap::Destroy();
    }
    return TRUE;
}

int __fastcall encryptHook(void *pthis, void* unk, uint8* packet, int len)
{
	if(packet[0] == 0x25 && packet[6] == 0x04) //Quest Log Request
	{
		if(packet[10] == 0x00 && packet[11] == 0x00) //Almost always means its sent by the program
			bShowLog = false;
	}

	return oEncrypt(pthis, packet, len);
}

int __fastcall decryptHook(void *pthis, void* unk, uint8* packet, int len)
{
	int decryptres = oDecrypt(pthis, packet, len);

	if (packet[0] == 0x00)
		return decryptres;

	if(packet[0] == 0x1E)
	{
		if(!bShowLog)
		{
			packet[0] = 0xFF;
			bShowLog = true;
		}

		plr->ParseQuestLog(*(QuestLog*)&packet[2]);

		return decryptres;
	}

	PDATA data;
	data.packet = new uint8[len];
	memcpy(data.packet, packet, len);
	data.len = len;
	inPackets.push(data);



	return decryptres;
}


DWORD WINAPI InPacketsProcesser(LPVOID lpBuffer)
{
	while(bRunning)
	{
		if(!inPackets.empty()) //If there are packets in the queue
		{
			uint8* packet = inPackets.front().packet;

			//Do whatever with packet here
			if(packet[0] == 0x1F)
			{
				plr->LoginData(*(InitialLoginData*)&packet[2]);
			}
			else if(packet[0] == 0x03)
			{
				handleUpdatePacket(packet, inPackets.front().len);
			}
			else if(packet[0] == 0x42) //Quest Packet
			{
				plr->ParseQuestText(packet);
			}

			free(packet);
			inPackets.pop();
		}
		Sleep(10);
	}
	return 0;
}

void createConsole()
{
	AllocConsole();
	int hCrt, i;
	FILE *hf;
	hCrt = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	hf = _fdopen( hCrt, "w" );
	*stdout = *hf;
	i = setvbuf( stdout, NULL, _IONBF, 0 );
}

void handleUpdatePacket(uint8* data, int len)
{
	UpdatePacketSelf update = *(UpdatePacketSelf*)&data[2];
	plr->Update(update);

	int offset = 24;

	if(update.numInRangeDynamicObjects > 0)
	{
		for(int i = 0; i < update.numInRangeDynamicObjects; i++)
		{
			if(data[offset] == 0x01) offset += 8;
			else if(data[offset] == 0x02) offset += 12;
			else if(data[offset] == 0x03) offset += 6;
		}
	}

	if(update.numInRangeSpellEffects > 0)
	{
		for(int x = 0; x < update.numInRangeSpellEffects; x++)
		{
			if(data[offset] >= 240) offset += 12;
			else offset += 8;
		}
	}

	UnitMap::SetAllOutOfRange();
	if(update.numInRangeUnits > 0)
	{
		for(int i = 0; i < update.numInRangeUnits; i++)
		{
			uint16 id = (data[offset+1] << 8) + data[offset];
			uint16 trueid = id & 0x7FFF;
			UnitMap::SetUnitInRange(trueid);
			offset += 2;

			if(id & 0x8000)
				continue;
			else
			{
				uint8 updateflag = data[offset];
				offset += 1;

				if(updateflag & 0x01)
				{
					UnitMap::UpdateUnitsMovement(trueid, *(UpdatePacketUnitMovement*)&data[offset]); 
					offset += 5;
				}
				if(updateflag & 0x02)
				{
					UnitMap::UpdateUnitsAuras(trueid, *(UpdatePacketUnitAuras*)&data[offset]);
					offset += 3;
				}
				if(updateflag & 0x04)
				{
					UnitMap::UpdateUnitsModel(trueid, *(UpdatePacketUnitModels*)&data[offset]);
					offset += 8;
				}
				if(updateflag & 0x08)
				{
					UnitMap::UpdateUnitsAnim(trueid, *(UpdatePacketUnitAnim*)&data[offset]);
					offset += 1;
				}
				if(updateflag & 0x10)
				{
					UnitMap::UpdateUnitsSpellEffect(trueid, *(UpdatePacketUnitSpellEffect*)&data[offset]);
					offset += 1;
				}
				if(updateflag & 0x20)
				{
					offset += 1 + data[offset];
				}
			}
		}
	}
	UnitMap::CleanUnits();
	//UnitMap::OutputUnitMap();
}

DWORD WINAPI bot(LPVOID lpBuffer)
{
	while(true)
	{
		plr->StartGuardianTask();
		plr->PlayerCharge();
		plr->KillGuardians();
	}
	return 0;
}
