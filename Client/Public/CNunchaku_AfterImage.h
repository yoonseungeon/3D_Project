#pragma once

#include "Client_Defines.h"
#include "CEffect.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
class CTexture;
NS_END

NS_BEGIN(Client)

class CNunchaku_AfterImage final : public CEffect
{
public:
	struct NUNCHAKU_AFTERIMAGE_DESC : public CEffect::EFFECT_DESC
	{
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

	virtual void Set_EffectTransform(_uint iCurAni, _uint iATKCount = 1) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

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