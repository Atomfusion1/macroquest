/*
 * MacroQuest2: The extension platform for EverQuest
 * Copyright (C) 2002-2019 MacroQuest Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "pch.h"
#include "MQ2DataTypes.h"

namespace mq {
namespace datatypes {

enum class PetBuffMembers
{
	Caster,
	Duration,
};

MQ2PetBuffType::MQ2PetBuffType() : MQ2Type("petbuff")
{
	ScopedTypeMember(PetBuffMembers, Caster);
	ScopedTypeMember(PetBuffMembers, Duration);
}

bool MQ2PetBuffType::GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest)
{
	EQ_Spell* pSpell = static_cast<EQ_Spell*>(VarPtr.Ptr);
	if (!pSpell)
		return false;

	if (!pPetInfoWnd)
		return false;

	MQTypeMember* pMember = FindMember(Member);
	if (!pMember)
	{
		MQVarPtr spellVar;
		spellVar.Ptr = VarPtr.Ptr;

		return pSpellType->GetMember(spellVar, Member, Index, Dest);
	}

	switch (static_cast<PetBuffMembers>(pMember->ID))
	{
	case PetBuffMembers::Caster:
		if (CXStr* pWhoCast = pPetInfoWnd->WhoCast.FindFirst(pSpell->ID))
		{
			strcpy_s(DataTypeTemp, pWhoCast->c_str());
			Dest.Type = pStringType;
			Dest.Ptr = &DataTypeTemp[0];
			return true;
		}
		return false;

	case PetBuffMembers::Duration: {
		// Find the index of this spell id.
		int buffIndex = -1;
		for (int index = 0; index < NUM_BUFF_SLOTS; ++index)
		{
			if (pPetInfoWnd->Buff[index] == pSpell->ID)
			{
				buffIndex = index;
				break;
			}
		}
		if (buffIndex == -1)
			return false;
		Dest.UInt64 = pPetInfoWnd->PetBuffTimer[buffIndex];
		Dest.Type = pTimeStampType;
		return true;
	}

	default: break;
	}

	return false;
}

bool MQ2PetBuffType::ToString(MQVarPtr VarPtr, char* Destination)
{
	MQVarPtr spellVar;
	spellVar.Ptr = VarPtr.Ptr;

	return pSpellType->ToString(spellVar, Destination);
}

//----------------------------------------------------------------------------

enum class PetMembers
{
	Buff,
	Combat,
	GHold,
	Hold,
	ReGroup,
	Stance,
	Stop,
	Target,
	Taunt,
	BuffDuration
};

MQ2PetType::MQ2PetType() : MQ2Type("pet")
{
	ScopedTypeMember(PetMembers, Buff);
	ScopedTypeMember(PetMembers, Combat);
	ScopedTypeMember(PetMembers, GHold);
	ScopedTypeMember(PetMembers, Hold);
	ScopedTypeMember(PetMembers, ReGroup);
	ScopedTypeMember(PetMembers, Stance);
	ScopedTypeMember(PetMembers, Stop);
	ScopedTypeMember(PetMembers, Target);
	ScopedTypeMember(PetMembers, Taunt);
	ScopedTypeMember(PetMembers, BuffDuration);
}

bool MQ2PetType::GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest)
{
	SPAWNINFO* pSpawn = reinterpret_cast<SPAWNINFO*>(VarPtr.Ptr);
	if (!pSpawn)
		return false;

	MQTypeMember* pMember = MQ2PetType::FindMember(Member);
	if (!pMember)
	{
		if (!pSpawn->SpawnID)
			return false;

		return pSpawnType->GetMember(VarPtr, Member, Index, Dest);
	}

	if (!pPetInfoWnd)
		return false;

	switch (static_cast<PetMembers>(pMember->ID))
	{
	case PetMembers::Buff:
		Dest.Type = pSpellType;
		if (!Index[0])
			return false;

		if (IsNumber(Index))
		{
			int nBuff = GetIntFromString(Index, 0) - 1;
			if (nBuff < 0)
				return false;

			if (nBuff >= NUM_BUFF_SLOTS)
				return false;

			if (pPetInfoWnd->Buff[nBuff] == -1 || pPetInfoWnd->Buff[nBuff] == 0)
				return false;

			if (Dest.Ptr = GetSpellByID(pPetInfoWnd->Buff[nBuff]))
			{
				Dest.Type = pPetBuffType;
				return true;
			}
		}
		else
		{
			for (int nBuff = 0; nBuff < NUM_BUFF_SLOTS; nBuff++)
			{
				if (SPELL* pSpell = GetSpellByID(pPetInfoWnd->Buff[nBuff]))
				{
					if (!_strnicmp(Index, pSpell->Name, strlen(Index)))
					{
						Dest.DWord = nBuff + 1;
						Dest.Type = pIntType;
						return true;
					}
				}
			}
		}
		return false;

	case PetMembers::BuffDuration:
		Dest.UInt64 = 0;
		Dest.Type = pTimeStampType;
		if (!Index[0])
			return false;

		if (IsNumber(Index))
		{
			int nBuff = GetIntFromString(Index, 0) - 1;
			if (nBuff < 0)
				return false;

			if (nBuff >= NUM_BUFF_SLOTS)
				return false;

			if (pPetInfoWnd->Buff[nBuff] == -1 || pPetInfoWnd->Buff[nBuff] == 0)
				return false;

			Dest.UInt64 = pPetInfoWnd->PetBuffTimer[nBuff];
			return true;
		}
		else
		{
			for (int nBuff = 0; nBuff < NUM_BUFF_SLOTS; nBuff++)
			{
				if (SPELL* pSpell = GetSpellByID(pPetInfoWnd->Buff[nBuff]))
				{
					if (!_strnicmp(Index, pSpell->Name, strlen(Index)))
					{
						Dest.UInt64 = pPetInfoWnd->PetBuffTimer[nBuff];
						return true;
					}
				}
			}
		}
		return false;

	case PetMembers::Combat:
		Dest.Set(pSpawn->WhoFollowing != nullptr);
		Dest.Type = pBoolType;
		return true;

	case PetMembers::GHold:
		Dest.Set(pPetInfoWnd->GHold);
		Dest.Type = pBoolType;
		return true;

	case PetMembers::Hold:
		Dest.Set(pPetInfoWnd->Hold);
		Dest.Type = pBoolType;
		return true;

	case PetMembers::ReGroup:
		Dest.Set(pPetInfoWnd->ReGroup);
		Dest.Type = pBoolType;
		return true;

	case PetMembers::Stance:
		if (pPetInfoWnd->Follow)
			strcpy_s(DataTypeTemp, "FOLLOW");
		else
			strcpy_s(DataTypeTemp, "GUARD");
		Dest.Ptr = &DataTypeTemp[0];
		Dest.Type = pStringType;
		return true;

	case PetMembers::Stop:
		Dest.Set(pPetInfoWnd->Stop);
		Dest.Type = pBoolType;
		return true;

	case PetMembers::Target:
		Dest.Type = pSpawnType;
		if (Dest.Ptr = pSpawn->WhoFollowing)
		{
			return true;
		}
		return false;

	case PetMembers::Taunt:
		Dest.Set(pPetInfoWnd->Taunt);
		Dest.Type = pBoolType;
		return true;
	}

	return false;
}

bool MQ2PetType::ToString(MQVarPtr VarPtr, char* Destination)
{
	if (!VarPtr.Ptr)
		return false;

	SPAWNINFO* pSpawn = static_cast<SPAWNINFO*>(VarPtr.Ptr);
	strcpy_s(Destination, MAX_STRING, pSpawn->Name);
	return true;
}

void MQ2PetType::InitVariable(MQVarPtr& VarPtr)
{
	// FIXME: Do not allocate a SPAWNINFO
	VarPtr.Ptr = new SPAWNINFO();

	// FIXME: Do not ZeroMemory a SPAWNINFO
	ZeroMemory(VarPtr.Ptr, sizeof(SPAWNINFO));
}

void MQ2PetType::FreeVariable(MQVarPtr& VarPtr)
{
	// FIXME: Do not allocate a SPAWNINFO
	SPAWNINFO* pSpawn = static_cast<SPAWNINFO*>(VarPtr.Ptr);
	delete pSpawn;
}

bool MQ2PetType::FromData(MQVarPtr& VarPtr, MQTypeVar& Source)
{
	if (Source.Type == pSpawnType)
	{
		memcpy(VarPtr.Ptr, Source.Ptr, sizeof(SPAWNINFO));
		return true;
	}
	else
	{
		if (SPAWNINFO* pOther = (SPAWNINFO*)GetSpawnByID(Source.DWord))
		{
			memcpy(VarPtr.Ptr, pOther, sizeof(SPAWNINFO));
			return true;
		}
	}
	return false;
}

bool MQ2PetType::FromString(MQVarPtr& VarPtr, const char* Source)
{
	if (SPAWNINFO* pOther = (SPAWNINFO*)GetSpawnByID(GetIntFromString(Source, 0)))
	{
		memcpy(VarPtr.Ptr, pOther, sizeof(SPAWNINFO));
		return true;
	}
	return false;
}

}} // namespace mq::datatypes

