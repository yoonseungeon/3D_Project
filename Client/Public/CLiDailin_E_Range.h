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

class CLiDailin_E_Range final : public CPartEffect
{
public:
	struct LIDAILIN_E_RANGE_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CLiDailin_E_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLiDailin_E_Range(const CLiDailin_E_Range& Prototype);
	virtual ~CLiDailin_E_Range() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_E_Range(CBody_Player* pBody_Player, _float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

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
	static CLiDailin_E_Range* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END