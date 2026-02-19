#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

// Client에서 상속받을 예정. 생성자, 소멸자 호출 필요 따라서 ENGINE_DLL 붙여줘야 함.
class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice	= { nullptr };
	ID3D11DeviceContext*	m_pContext	= { nullptr };

protected:
	virtual void Free() override;
};

NS_END