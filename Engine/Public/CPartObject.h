#pragma once

#include "CGameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJECT_DESC
	{
		const _float4x4* pParentMatrix{};
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	const _float4x4*	m_pParentMatrix{};
	// 부모 행렬 곱해진 World 행렬
	_float4x4			m_CombinedWorldMatrix{};

protected:
	// 부모와 자식 행렬 곱하는 함수.(m_CombinedWorldMatrix 계산)
	void XM_CALLCONV Compute_CombinedWorldMatrix(_fmatrix ChildWorldMatrix);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END