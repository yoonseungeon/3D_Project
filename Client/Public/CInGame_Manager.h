#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CLumia_Ground;
class CPlayer;

class CInGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CInGame_Manager)

private:
	CInGame_Manager();
	virtual ~CInGame_Manager() = default;

public:
	void Set_Map(CLumia_Ground* pMap_Lumia);
	void Release_Map();

	_float3 MapPIcking();

	void Set_Player(CPlayer* pPlayer);
	CPlayer* Get_Player() { return m_pPlayer; }
	void Release_Player();

	_float3 Get_PlayerPos();

private:
	CLumia_Ground* m_pMap_Lumia{ nullptr };
	CPlayer* m_pPlayer{ nullptr };

protected:
	virtual void Free() override;
};

NS_END