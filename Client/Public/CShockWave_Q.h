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

class CShockWave_Q final : public CPartEffect
{
public:
	struct SHOKEWAVE_Q_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CShockWave_Q(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShockWave_Q(const CShockWave_Q& Prototype);
	virtual ~CShockWave_Q() = default;

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
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom{ nullptr };

	_float m_fProgressRatio{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShockWave_Q* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END