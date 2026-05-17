#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CEffect abstract : public CGameObject
{
public:
	struct EFFECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iTexIdx{};
	};

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Compute_CombinedMatrix(const _float4x4* vParentMatrix);

	virtual void Set_IsInactive(_bool bIsInactive) override;
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	virtual void Set_EffectTransform(_uint iCurAni, _uint iATKCount = 1);

protected:
	_float4x4 m_CombinedWorldMatrix{};

	_uint m_iCurAni{};
	_uint m_iATKCount{};

	_float m_fAlpha{ 1.f };
	_uint m_iTexIdx{};

	_float4 m_vDebugQuat = _float4(0.379001f, 0.629112f, 0.425090f, 0.529031f);
	_float4 m_vDebugPos = _float4(0.113678f, 1.445772f, 0.451558f, 1.f);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END