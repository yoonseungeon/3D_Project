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
	void Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir);

private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	static CPicking_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END