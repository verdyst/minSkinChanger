#include "pch.h"
#include "skinchanger.h"
#include "DefEnums.h"
#include "fastAlgo.h"

#define knifeLength 6

short knives[knifeLength]{ WEAPON_KNIFE_T, 42, WEAPON_KNIFE_KARAMBIT, WEAPON_KNIFE_M9_BAYONET, WEAPON_KNIFE_BAYONET, WEAPON_KNIFE_FLIP };


void updateModelIDs(int modelIDs[])
{
	modelIDs[0] = g_vars.modelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	modelIDs[1] = g_vars.modelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	modelIDs[2] = g_vars.modelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	modelIDs[3] = g_vars.modelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
}


bool isKnife(short wepID)
{
	//poly_root_solver();
	for (int i = 0; i < knifeLength; i++)
	{
		if (wepID == knives[i])
			return true;
	}

	return false;
}



// knife id == weaponID

void skinChanger()
{
	float wear = 0.0000001f; // fac new

	int knifeID = WEAPON_KNIFE_KARAMBIT; // m9 bayonet
	int knifeIDOffset = knifeID < 10 ? 0 : 1;
	int modelIndex = 0;

	//std::cout << "[+] localPlayer : " << std::hex << g_vars.localEnt << std::endl;
	if (g_vars.localEnt)
	{
		for (int i = 0; i < 8; i++)
		{
			int cWep = *(int*)(g_vars.localEnt + offset.m_hMyWeapons + i * 0x4) & 0xfff; // get current weapon (entnum) takes first 1.5 bytes

			cWep = *(int*)(offset.dwEntityList + (cWep - 1) * 0x10); // find the weapon in the Entity list
			// cWep is a pointer to a weapon



			if (cWep != 0) // if weapon is valid
			{
				
				short cWepID = *(short*)(cWep + offset.m_iItemDefinitionIndex); // get weapon ID (ak is 7)

				
				if (g_vars.paintKits[cWepID] != 0)
				{

					
					*(int*)(cWep + g_vars.m_iItemIDHigh) = -1; // force game to use fallback values
					*(int*)(cWep + g_vars.m_nFallbackPaintKit) = g_vars.paintKits[cWepID]; // set paintkit
					*(float*)(cWep + g_vars.m_flFallbackWear) = .00000001; // was not changing because it was typecasted as an int, being truncated down each time.
				}

				if (isKnife(cWepID) && g_vars.knife != 0) // make an isKnife function
				{
					//std::cout << std::hex << "[+] knife : " << cWep << std::endl;
					g_vars.knifeAddr = cWep;
					*(int*)(cWep + g_vars.m_iItemIDHigh) = -1;
					*(int*)(cWep + offset.m_iItemDefinitionIndex) = g_vars.knife;
					*(int*)(cWep + offset.m_nModelIndex) = g_vars.modelIndex;
					*(int*)(cWep + offset.m_iViewModelIndex) = g_vars.modelIndex;
					*(int*)(cWep + offset.m_iEntityQuality) = 3;
					*(int*)(cWep + g_vars.m_nFallbackPaintKit) = g_vars.knife_skin;
				}
			}
		}

		
		int activeWeapon = *(int*)(g_vars.localEnt + offset.m_hActiveWeapon) & 0xFFF;

		activeWeapon = *(int*)(offset.dwEntityList + (activeWeapon - 1) * 0x10);

		if (!activeWeapon) // returns 0
			return;

		short cWepID = *(short*)(activeWeapon + offset.m_iItemDefinitionIndex);

		if (!isKnife(cWepID)) return;

		int weaponViewModelID = *(int*)(activeWeapon + offset.m_iViewModelIndex);

		uintptr_t knifeViewModel = *(uintptr_t*)(g_vars.localEnt + offset.m_hViewModel) & 0xFFF;

		knifeViewModel = *(uintptr_t*)(offset.dwEntityList + (knifeViewModel - 1) * 0x10);


		if (!knifeViewModel) return;


		*(uintptr_t*)(knifeViewModel + offset.m_nModelIndex) = g_vars.modelIndex;

		int sequence = *(int*)(g_vars.localEnt + offset.m_nSequenceOffset);

		//int originalActivity = g_vars.localEnt->GetSequenceActivity(sequence);
	}
}