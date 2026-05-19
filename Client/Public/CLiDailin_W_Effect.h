#pragma once

#include "Client_Defines.h"
#include "CPartEffect.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Player;

class CLiDailin_W_Effect final : public CPartEffect
{
public:
	struct LIDAILIN_W_EFFECT_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CLiDailin_W_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLiDailin_W_Effect(const CLiDailin_W_Effect& Prototype);
	virtual ~CLiDailin_W_Effect() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Active_Effect_W(CBody_Player* pBody_Player, _float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float3 m_vColor{};
	_float m_fScale{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLiDailin_W_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END