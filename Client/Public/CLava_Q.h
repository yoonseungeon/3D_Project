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

class CLava_Q final : public CPartEffect
{
public:
	struct LAVA_Q_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CLava_Q(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLava_Q(const CLava_Q& Prototype);
	virtual ~CLava_Q() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_SpinEffect(CBody_Player* pBody_Player, _float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_bool m_bSetPos{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLava_Q* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END