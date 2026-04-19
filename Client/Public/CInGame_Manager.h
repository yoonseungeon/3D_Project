#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CLumia_Ground;

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

	void Set_PlayerPos(const _float3& vPlayerPos) { m_vPlayerPos = vPlayerPos; }
	const _float3& Get_PlayerPos() { return m_vPlayerPos; }

private:
	CLumia_Ground* m_pMap_Lumia{ nullptr };

	_float3 m_vPlayerPos{};

protected:
	virtual void Free() override;
};

NS_END