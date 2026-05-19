#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CLumia_Ground;
class CAbstractPlayer;
class CUI_GameResult;

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

	void Set_Player(CAbstractPlayer* pPlayer);
	CAbstractPlayer* Get_Player() { return m_pPlayer; }
	void Release_Player();

	_float3 Get_PlayerPos();

	void Set_GameResultUI(CUI_GameResult* pGameResult);
	void Release_GameResultUI();

	void Update_End(_float fTimeDelta);

	void Add_EnemyCount() { ++ m_iEnemyCount; }
	void Sub_EnemyCount() { -- m_iEnemyCount; }
	_uint Get_EnemyCount() { return m_iEnemyCount; }

	_bool Get_GameEnd() { return m_bGameEnd; }

private:
	CLumia_Ground* m_pMap_Lumia{ nullptr };
	CAbstractPlayer* m_pPlayer{ nullptr };
	CUI_GameResult* m_pGameResultUI{ nullptr };

	_uint m_iEnemyCount{};

	_bool m_bGameEnd{};

	_float m_fMaxWaitGameEndTime{};
	_float m_fAccWaitGameEndTime{};

protected:
	virtual void Free() override;
};

NS_END