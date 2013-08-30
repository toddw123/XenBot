//UnitMap.cpp
#include "UnitMap.h"

HANDLE UnitMap::ghMutex; 
hash_map<int, Unit*> UnitMap::map;

void UnitMap::Initialize()
{
	ghMutex = CreateMutex(NULL, FALSE, NULL);
}

void UnitMap::Destroy()
{
	CloseHandle(ghMutex);
}

void UnitMap::SetAllOutOfRange()
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			hash_map<int, Unit*>::iterator iter;
			for(iter = map.begin(); iter != map.end(); iter++)
				iter->second->inrange = false;
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


void UnitMap::SetUnitInRange(uint16 id)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			if(map[id] == NULL)
			{
				Unit* tmp = new Unit;
				tmp->id = id;
				map[id] = tmp;
			}
			map[id]->inrange = true;
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

void UnitMap::UpdateUnitsMovement(uint16 id, UpdatePacketUnitMovement movement)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			map[id]->movement = movement;
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

void UnitMap::UpdateUnitsAuras(uint16 id, UpdatePacketUnitAuras auras)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			map[id]->auras = auras;
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

void UnitMap::UpdateUnitsModel(uint16 id, UpdatePacketUnitModels models)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			map[id]->models = models;
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

void UnitMap::UpdateUnitsAnim(uint16 id, UpdatePacketUnitAnim anim)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			map[id]->anim = anim;
			if(anim.anim == 21 || anim.anim == 45)
                  map[id]->alive = false;
               else
                  map[id]->alive = true;
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

void UnitMap::UpdateUnitsSpellEffect(uint16 id, UpdatePacketUnitSpellEffect spelleffect)
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			map[id]->spelleffect = spelleffect;
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

void UnitMap::CleanUnits()
{
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			hash_map<int, Unit*>::iterator iter;
			hash_map<int, Unit*>::iterator start = map.begin();
			hash_map<int, Unit*>::iterator end = map.end();
			for(iter = start; iter != end; iter++)
			{
				if(!iter->second->inrange)
				{
					free(map[iter->first]);
					map.erase(iter->first);
				}
			}
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

void UnitMap::OutputUnitMap()
{
	DWORD dwWaitResult; 

	FILE* out = fopen("UnitMap.log", "a+");
	fprintf(out, "-----------------------------------------------------------------------\n");
	fprintf(out, "| Units On Screen: %i                                                 |\n", map.size());
	fprintf(out, "-----------------------------------------------------------------------\n");
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			hash_map<int, Unit*>::iterator iter;
			for(iter = map.begin(); iter != map.end(); iter++)
			{
				fprintf(out, "ID: %i | Model: %i | Weapon: %i | Sheild: %i | Helmet: %i | Colorbits: %i | X/Y: %i, %i\n", iter->second->id, iter->second->models.model, iter->second->models.weapon, iter->second->models.shield, iter->second->models.helmet, iter->second->models.colorbits, iter->second->movement.positionX, iter->second->movement.positionY);
			}
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
	fprintf(out, "\n");
	fclose(out);
}

Unit* UnitMap::LocateUnit(uint16 model, uint16 weapon, uint8 sheild, uint8 helmet, uint16 colorbits)
{
   DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			hash_map<int, Unit*>::iterator iter;
			for(iter = map.begin(); iter != map.end(); iter++)
			{
				if(iter->second->models.model == model 
					&& iter->second->models.weapon == weapon
					&& iter->second->models.shield == sheild
					&& iter->second->models.helmet == helmet
					&& iter->second->models.colorbits == colorbits)
					return iter->second;
			}
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

	return NULL;
}

BOOL CheckRadius(int px, int py, int mx, int my, int r)
{
	if( ((px - mx) * (px - mx)) + ((py - my) * (py - my)) <= (r * r))
		return true;
	return false;
}

int UnitMap::UnitsAlive(int x, int y, int r)
{
	int ret = 0;
	DWORD dwWaitResult; 
	
	dwWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0:
		__try {
			hash_map<int, Unit*>::iterator iter;
			for(iter = map.begin(); iter != map.end(); iter++)
			{
				if(iter->second->alive && iter->second->models.model == 54 && CheckRadius(iter->second->movement.positionX, iter->second->movement.positionY, x, y, r))
					ret++;
			}
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