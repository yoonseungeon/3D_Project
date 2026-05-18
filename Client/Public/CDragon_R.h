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

class CDragon_R final : public CPartEffect
{
public:
	struct DRAGON_R_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CDragon_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDragon_R(const CDragon_R& Prototype);
	virtual ~CDragon_R() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Active_Dragon_R(CBody_Player* pBody_Player, _float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };
	CTexture* m_pMaskComColor{ nullptr };
	CTexture* m_pMaskComDisapper{ nullptr };

	_bool m_bSetPos{};
	_float m_fAccTime{};
	_float m_fMaxTime{};

	_int m_iCurAniIndex{ -1 };
	_bool m_bIsActiveOnce{};

	_float m_fProgressRatio{};

	_float3 m_vColor{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDragon_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END