#pragma once

#include "Client_Defines.h"
#include "CPartEffect.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Player;

class CLiDailinSlash2 final : public CPartEffect
{
public:
	struct LIDAILIN_SLASH2_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CLiDailinSlash2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLiDailinSlash2(const CLiDailinSlash2& Prototype);
	virtual ~CLiDailinSlash2() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_SpinEffect(CBody_Player* pBody_Player);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };
	CTexture* m_pTexNoise{ nullptr };

	_float m_fProgressRatio{};
	_float m_fDiscardRatio{};
	_float3 m_vStartColor{};
	_float3 m_vEndColor{};
	_bool m_bFlipX{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLiDailinSlash2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END