#pragma once
#ifdef _DEBUG

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CMap_Lumia;

class CInGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CInGame_Manager)

private:
	CInGame_Manager();
	virtual ~CInGame_Manager() = default;

public:
	void Set_Map(CMap_Lumia* pMap_Lumia);
	void Release_Map();

	_float3 MapPIcking();

private:
	CMap_Lumia* m_pMap_Lumia{ nullptr };

protected:
	virtual void Free() override;
};

NS_END

#endif
