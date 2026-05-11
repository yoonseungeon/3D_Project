#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CGameInstance;
class CCollider;

class CPicking_Manager final : public CBase
{
private:
	CPicking_Manager();
	virtual ~CPicking_Manager() = default;

private:
	HRESULT Initialize();

public:
	void Update_Picking_Manager();
	void Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir);
	_bool Picking_Object(COLLISION_RAY_INFO& tOutColInfo);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	_float4 m_vWorldRayPos{};
	_float4 m_vWroldRayDir{};

public:
	static CPicking_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END