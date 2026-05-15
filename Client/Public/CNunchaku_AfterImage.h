#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
class CTexture;
NS_END

NS_BEGIN(Client)

class CNunchaku_AfterImage final : public CGameObject
{
public:
	struct NUNCHAKU_AFTERIMAGE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iTexIdx{};
	};

protected:
	CNunchaku_AfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNunchaku_AfterImage(const CNunchaku_AfterImage& Prototype);
	virtual ~CNunchaku_AfterImage() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Compute_CombinedMatrix(const _float4x4* vParentMatrix);

	virtual void Set_IsInactive(_bool bIsInactive) override;
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	void Set_EffectTransform(_uint iCurAni, _uint iATKCount = 1);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float4x4 m_CombinedWorldMatrix{};

	_uint m_iCurAni{};
	_uint m_iATKCount{};

	_float m_fAlpha{ 1.f };
	_uint m_iTexIdx{};

	//
	_float4 m_vDebugQuat = _float4(0.530984f, -0.493545f, 0.559280f, 0.402088f);
	_float4 m_vDebugPos = _float4(0.056480f, 0.096296f, 0.131294f, 1.f);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNunchaku_AfterImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END