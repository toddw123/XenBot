#include "Player.h"
#include "Packet.h"
#include "UnitMap.h"

void Player::Initialize(DWORD addr)
{
	oSendPacket = (tSendPacket)addr;
}
void Player::LoginData(InitialLoginData ild)
{
	p_id = ild.serverId;
}
void Player::Update(UpdatePacketSelf upt)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			p_update = upt;
		}
		__finally {
			if(!ReleaseMutex(ghMutex))
			{
				printf("Failed to release mutex: %d\n", GetLastError());
			}
		}
		break; 
	case WAIT_ABANDONED:
		break;
	}
}
void Player::ParseQuestText(uint8* packet)
{
	InitialQuestData iqd = *(InitialQuestData*)&packet[2];
	QuestText *qt = new QuestText[iqd.strings];
	qt = (QuestText*)&packet[4];

	for(int i = 0; i < iqd.strings; i++)
	{
		if(strcmp((char*)qt[i].text, "~OK") == 0)
			QuestFlags |= OK;
		else if(strcmp((char*)qt[i].text, "~Thanks") == 0)
			QuestFlags |= THANKS;
		else if(strcmp((char*)qt[i].text, "~Guardian task") == 0)
			QuestFlags |= GUARDIANS;
	}

	delete[] qt;
}
void Player::ParseQuestLog(QuestLog ulog)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutexLog, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			p_log = ulog;
		}
		__finally {
			if(!ReleaseMutex(ghMutexLog))
			{
				printf("Failed to release mutex: %d\n", GetLastError());
			}
		}
		break; 
	case WAIT_ABANDONED:
		break;
	}
}
UpdatePacketSelf Player::GetUpdateInfo()
{
	UpdatePacketSelf ret;
	DWORD dwWaitResult; 	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			ret = p_update;
		}
		__finally {
			if(!ReleaseMutex(ghMutex))
			{
				printf("Failed to release mutex: %d\n", GetLastError());
			}
		}
		break; 
	case WAIT_ABANDONED:
		break;
	}

	return ret;
}

QuestLog Player::GetQuestLog()
{
	QuestLog ret;
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutexLog, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			ret = p_log;
		}
		__finally {
			if(!ReleaseMutex(ghMutexLog))
			{
				printf("Failed to release mutex: %d\n", GetLastError());
			}
		}
		break; 
	case WAIT_ABANDONED:
		break;
	}
	return ret;
}


//Buff
void Player::Turi(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0x35 0x00 0xD2 0x2C 0x00 0x00
	CastSpell(SELF, (0x0034 + level), p_id, 0);
}
void Player::Extrudere(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0x40 0x00 0xD2 0x2C 0x00 0x00
	CastSpell(SELF, (0x003F + level), p_id, 0);
}
void Player::Velocitas(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x50 0x00 0x59 0x00 0xD2 0x2C 0x16 0x1F
	CastSpell(CAST, (0x0058 + level), p_id, p_update.positionY);
}
void Player::Robur(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x50 0x00 0x81 0x00 0xD2 0x2C 0x16 0x1F
	CastSpell(CAST, (0x0080 + level), p_id, p_update.positionY);
}

//Helping/Aid
void Player::Mederi(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x50 0x00 0x01 0x00 0xD2 0x2C 0x15 0x1F
	CastSpell(CAST, (0x0000 + level), p_id, p_update.positionY);
}
void Player::NocteVisus()
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0x29 0x00 0xD2 0x2C 0x19 0x1F 
	CastSpell(SELF, 0x0029, p_id, 0);
}
void Player::ContraVisus()
{
	//0xD2 0x2C 0x00 0x00 0x50 0x00 0xA6 0x00 0xD2 0x2C 0x11 0x1F
	CastSpell(SELF, 0x00A6, p_id, 0);
}

//Damage
void Player::GlacialisViscera(uint8 level, uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0x8C 0x00 0xB7 0x04 0x4E 0x1F
	CastSpell(ATK, (0x008B + level), x, y);
}
void Player::Fulgur(uint8 level, uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0xA7 0x00 0xAD 0x04 0x58 0x1F
	CastSpell(ATK, (0x00A6 + level), x, y);
}
void Player::GlacialisVentus(uint8 level, uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0xB4 0x00 0xAC 0x04 0x5A 0x1F
	CastSpell(ATK, (0x00B3 + level), x, y);
}
void Player::FragorIpsum(uint8 level)
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0xFA 0x00 0xD2 0x2C 0x5A 0x1F
	CastSpell(SELF, (0x00F9 + level), p_id, 0);
}
void Player::Freeze(uint8 level, uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0x4A 0x01 0xAA 0x04 0x50 0x1F
	CastSpell(ATK, (0x0049 + level), x, y);
}
void Player::Sidus(uint8 level, uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0x55 0x01 0xB5 0x04 0x59 0x1F
	CastSpell(ATK, (0x0154 + level), x, y);
}

//Others
void Player::MutatioNimbus()
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0x5F 0x00 0xD2 0x2C 0x3A 0x1F
	CastSpell(SELF, 0x005F, p_id, 0);
}
void Player::PergereTrans(uint16 x, uint16 y)
{
	//0xD2 0x2C 0x00 0x00 0x4C 0x00 0xBF 0x00 0xA6 0x04 0x3E 0x1F
	CastSpell(ATK, 0x00BF, x, y);
}
void Player::PraesentisTrans(uint8 mark)
{
	//0xAE 0x00 0x00 0x00 0x31 0x00 0x1B 0x01 0x00 0x00 0x00 0x00
	CastSpell((0x0030 + mark), 0x011B, 0, 0);
}
void Player::Star()
{
	//0xD2 0x2C 0x00 0x00 0x4E 0x00 0x24 0x01 0xD2 0x2C 0x3E 0x1F
	CastSpell(SELF, 0x0124, p_id, 0);
}

void Player::CastSpell(uint16 type, uint16 id, uint16 x, uint16 y)
{
	Packet spell(0x08, 12); 
	spell << p_id;
	spell << (uint16)0;
	spell << (uint16)type;
	spell << (uint16)id;
	spell << (uint16)x;
	spell << (uint16)y;

	//Send Packet
	oSendPacket(spell, spell, spell);

	free((uint8*)spell);
}

void Player::StopAction()
{
	//0xAE 0x00 0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	Packet stop(0x0C, 12);
	stop << p_id;
	stop << (uint16)0;
	stop << (uint16)0x0007;

	oSendPacket(stop, stop, stop);

	free((uint8*)stop);
}

void Player::SwitchWeapons()
{
	//0xAE 0x00 0x00 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	Packet swap(0x0C, 12);
	swap << p_id;
	swap << (uint16)0;
	swap << (uint16)0x0001;

	oSendPacket(swap, swap, swap);

	free((uint8*)swap);
}

void Player::Walk(uint16 x, uint16 y)
{
	//0xAE 0x00 0x00 0x00 0x64 0x2E 0x00 0x00 0x42 0x31 0x00 0x00 0x4D 0xA1 0x00 0x00
	Packet move(0x01, 16);
	move << p_id;
	move << (uint16)0;
	move << x;
	move << (uint16)0;
	move << y;
	move << (uint16)0;
	move << genMoveChecksum(p_id, x, y, MOVE);

	oSendPacket(move, move, move);

	free((uint8*)move);
}

void Player::Attack(uint16 who, uint16 y)
{
	//0xAE 0x00 0x00 0x00 0xAB 0x1A 0x00 0x00 0xB6 0x30 0x00 0x00 0x41 0x50 0x00 0x00
	Packet move(0x01, 16);
	move << p_id;
	move << (uint16)0;
	move << who;
	move << (uint16)0;
	move << y;
	move << (uint16)0;
	move << genMoveChecksum(p_id, who, y, ATTACK);

	oSendPacket(move, move, move);

	free((uint8*)move);
}

void Player::WalkTo(uint16 x, uint16 y)
{
	UpdatePacketSelf ups = GetUpdateInfo();
	while(!CheckRadius(ups.positionX, ups.positionY, x, y, 40))
	{
		uint16 movex, movey;
		if(x > ups.positionX) //Moving to the right
		{
			int m = rand()%20+80;
			if(ups.positionX + m < x)
				movex = ups.positionX + m;
			else
				movex = x;
		}
		else //Moving to the left
		{
			int m = rand()%20+80;
			if(ups.positionX - m > x)
				movex = ups.positionX - m;
			else
				movex = x;
		}
		if(y > ups.positionY) //Moving up
		{
			int m = rand()%20+80;
			if(ups.positionY + m < y)
				movey = ups.positionY + m;
			else
				movey = y;
		}
		else //Moving down
		{
			int m = rand()%20+80;
			if(ups.positionY - m > y)
				movey = ups.positionY - m;
			else
				movey = y;
		}

		Walk(movex, movey);
		Sleep(300);

		ups = GetUpdateInfo();
	}
}

void Player::Say(char *chat, int len)
{
	//0xAE 0x00 0x00 0x00 0x61 0x62 0x63 0x00 0x2D 0x5B 0x41 0x00 0x94 0xFA 0x18 0x00 0x87 0x9D 0x37 0x73 0x10 0x5B 0x04 0x08 0xA2 0x02 0x00 0x00 0x58 0x01 0x00 0x00 0x7C 0xFB 0x18 0x00 0x40 0x9D 0x37 0x73 0x10 0x5B 0x04 0x08 0x84 0x00 0x00 0x00 0xA8 0xFB 0x18 0x00 0x5C 0x9D 0x37 0x73 0x70 0x91 0x9B 0x08 0xA2 0x02 0x00 0x00 0x58 0x01 0x00 0x00 : 68
	Packet msg(0x0A, 68);
	msg << p_id;
	msg << (uint16)0;
	for(int i = 0; i < len; i++)
		msg << (uint8)chat[i];

	oSendPacket(msg, msg, msg);

	free((uint8*)msg);
}

uint16 Player::genMoveChecksum(uint16 id, uint16 x, uint16 y, uint8 type)
{
	uint16 checksum = 0;
	checksum += id;
	checksum += x;
	checksum += y;
	checksum += type;
	checksum = type | (checksum << 8);
	return checksum;
}

BOOL Player::CheckRadius(int px, int py, int mx, int my, int r)
{
	if( ((px - mx) * (px - mx)) + ((py - my) * (py - my)) <= (r * r))
		return true;
	return false;
}

void Player::PlayerCharge()
{
	PraesentisTrans(10);
	Sleep(1000);

	Walk(60*20+10, 398*20+10);
	Sleep(1000);

	Turi(9);
	Sleep(800);
	Extrudere(8);
	Sleep(800);
	Velocitas(2);
	Sleep(800);
	Robur(9);
	Sleep(800);
	ContraVisus();
	Sleep(800);

	Walk(60*20+10, 398*20+10);
	Sleep(1000);
}

void Player::StartGuardianTask()
{
	UpdatePacketSelf ups = GetUpdateInfo();
	QuestLog qlog;

	RequestQuestLog();
	Sleep(1000);

	QuestFlags = 0;

	qlog = GetQuestLog();

	if(qlog.curquest != 0 && qlog.curkills != qlog.reqkills)
		return;

	while(!CheckRadius(ups.positionX, ups.positionY, 594*20, 630*20, 40))
	{
		PraesentisTrans(3);
		Sleep(700);
		ups = GetUpdateInfo();
	}

	MutatioNimbus();
	Sleep(400);

	WalkTo(623*20, 630*20);

	StopAction();
	Sleep(400);

	Unit *quester = NULL;
	while((quester = UnitMap::LocateUnit(89, 98, 0, 0, 66)) == NULL)
		Sleep(500);
	
	while(QuestFlags == 0)
	{
		Attack(quester->id, quester->movement.positionY);
		Sleep(1500);
	}

	if(QuestFlags & THANKS)
	{
		//Say Thanks
		QuestFlags = 0;
		Say("Thanks", 6);
		Sleep(800);

		while(QuestFlags == 0)
		{
			Attack(quester->id, quester->movement.positionY);
			Sleep(1000);
		}
	}

	if(QuestFlags & GUARDIANS)
	{
		//Say "Guardian task"
		QuestFlags = 0;
		Say("Guardian task", 13);
		Sleep(1000);
		Say("OK", 2);
		Sleep(800);
		QuestFlags = 0;
	}
}

void Player::KillGuardians()
{
	_POINT mark4[16] = {{1134, 1455},{1132, 1447},{1136, 1445},{1141, 1448},{1143, 1454},{1144, 1459},{1144, 1465},{1144, 1470},{1143, 1473},{1138, 1473},{1134, 1472},{1130, 1471},{1129, 1467},{1129, 1462},{1128, 1457},{1130, 1453}};
	_POINT mark5[22] = {{1135, 1513},{1133, 1518},{1130, 1521},{1131, 1525},{1135, 1526},{1140, 1525},{1142, 1522},{1144, 1517},{1145, 1514},{1145, 1510},{1145, 1506},{1145, 1502},{1141, 1499},{1136, 1498},{1130, 1498},{1126, 1500},{1125, 1504},{1126, 1507},{1125, 1511},{1125, 1515},{1126, 1518},{1129, 1518}};
	_POINT mark6[18] = {{1172, 1457},{1169, 1454},{1166, 1452},{1162, 1455},{1162, 1458},{1162, 1462},{1163, 1465},{1165, 1468},{1169, 1468},{1172, 1468},{1176, 1468},{1179, 1468},{1182, 1466},{1182, 1462},{1182, 1458},{1181, 1454},{1179, 1452},{1175, 1453}};
	RequestQuestLog();
	Sleep(2000);
	while(GetQuestLog().curkills != GetQuestLog().reqkills)
	{
		PraesentisTrans(4);
		Sleep(700);

		TransAndMultiPath(mark4, 16);

		RequestQuestLog();
		Sleep(500);
		if(GetQuestLog().curkills == GetQuestLog().reqkills)
			break;

		PraesentisTrans(5);
		Sleep(700);

		TransAndMultiPath(mark5, 22);

		RequestQuestLog();
		Sleep(500);
		if(GetQuestLog().curkills == GetQuestLog().reqkills)
			break;

		PraesentisTrans(6);
		Sleep(700);

		TransAndMultiPath(mark6, 18);

		RequestQuestLog();
		Sleep(500);
		if(GetQuestLog().curkills == GetQuestLog().reqkills)
			break;

		/*PraesentisTrans(7);
		Sleep(700);

		while(UnitMap::UnitsAlive(p_update.positionX, p_update.positionY, 140) > 4)
		{
			FragorIpsum(8);
			Sleep(650);
			Star();
			Sleep(400);
		}

		RequestQuestLog();

		if(GetQuestLog().curkills == GetQuestLog().reqkills)
			break;*/

		PraesentisTrans(10);
		Sleep(25000);
	}

	PraesentisTrans(10);
	Sleep(2000);
}

void Player::RequestQuestLog()
{
	//0xAE 0x00 0x00 0x00 0x04 0x00 0x00 0x00 0x0F 0x35 0x00 0x00
	Packet log(0x25, 12);
	log << p_id;
	log << (uint16)0;
	log << (uint16)0x0004;

	oSendPacket(log, log, log);

	free((uint8*)log);
}

void Player::TransAndMultiPath(_POINT* path, int size)
{
	UpdatePacketSelf ups = GetUpdateInfo();
	for(int i = 0; i < size; i++)
	{
		RequestQuestLog();
		Sleep(100);
		if(UnitMap::UnitsAlive(ups.positionX, ups.positionY, 90) > 0)
		{
			Star(); Sleep(300);
			FragorIpsum(8); Sleep(600);
		}

		PergereTrans(path[i].x*20+10, path[i].y*20+10); Sleep(300);
		ups = GetUpdateInfo();
		if(GetQuestLog().curkills == GetQuestLog().reqkills)
			break;
	}
}