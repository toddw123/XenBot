//UnitMap.h
#ifndef __UNITMAP_H__
#define __UNITMAP_H__

#include <windows.h>
#include <stdio.h>
#include <hash_map>
#include <vector>
using namespace std;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

struct UpdatePacketUnitMovement
{
   uint16 positionX;
   uint16 positionY;
   uint8 rotation;
};

struct UpdatePacketUnitAuras
{
   uint16 auraflags; //(&0x08 = invis)
   uint8 auraflagsextra; //calv head glow, only time seen in use
};

struct UpdatePacketUnitModels
{
   uint16 model;
   uint16 weapon;
   uint8 shield;
   uint8 helmet;
   uint16 colorbits;
};

struct UpdatePacketUnitAnim
{
   uint8 anim;
};

struct UpdatePacketUnitSpellEffect
{
   uint8 spelleff;
};

struct UpdatePacketUnitChat
{
   uint8 stringsize;
};

struct Unit
{
   uint16 id;
   UpdatePacketUnitMovement movement;
   UpdatePacketUnitAuras auras;
   UpdatePacketUnitModels models;
   UpdatePacketUnitAnim anim;
   UpdatePacketUnitSpellEffect spelleffect;


   BOOL inrange;
   BOOL alive;
};

class UnitMap{
private:
	static HANDLE ghMutex; 
	static hash_map<int, Unit*> map;
public:
	static void Initialize();
	static void Destroy();
	static void SetAllOutOfRange();
	static void CheckExsists(uint16 id);
	static void SetUnitInRange(uint16 id);
	static void UpdateUnitsMovement(uint16 id, UpdatePacketUnitMovement movement);
	static void UpdateUnitsAuras(uint16 id, UpdatePacketUnitAuras auras);
	static void UpdateUnitsModel(uint16 id, UpdatePacketUnitModels models);
	static void UpdateUnitsAnim(uint16 id, UpdatePacketUnitAnim anim);
	static void UpdateUnitsSpellEffect(uint16 id, UpdatePacketUnitSpellEffect spelleffect); 
	static void CleanUnits();

	static void OutputUnitMap();

	static Unit* LocateUnit(uint16 model, uint16 weapon, uint8 sheild, uint8 helmet, uint16 colorbits);
	static int UnitsAlive(int x, int y, int r);
};


#endif