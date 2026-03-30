#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END