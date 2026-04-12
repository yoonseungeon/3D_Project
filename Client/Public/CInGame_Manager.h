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

private:
	CLumia_Ground* m_pMap_Lumia{ nullptr };

protected:
	virtual void Free() override;
};

NS_END