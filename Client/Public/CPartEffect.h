#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CPartEffect abstract : public CPartObject
{
public:
	struct PARTEFFECT_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketBoneMatrix{ nullptr };
		_uint iTexIdx{};
	};

protected:
	CPartEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartEffect(const CPartEffect& Prototype);
	virtual ~CPartEffect() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_IsInactive(_bool bIsInactive) override;
	virtual void Set_EffectTransform(_uint iCurAni, _uint iATKCount = 1);
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

protected:
	const _float4x4* m_pSocketBoneMatrix{ nullptr };

protected:
	_uint m_iCurAni{};
	_uint m_iATKCount{};

	_float m_fAlpha{ 1.f };
	_uint m_iTexIdx{};

	_float4 m_vDebugQuat = _float4(-0.931036f, -0.013712f, 0.364343f, -0.015443f);
	_float4 m_vDebugPos = _float4(0.f, 1.f, 0.f, 1.f);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END