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

class CFiora_W_1 final : public CPartEffect
{
public:
	struct FIORA_W_1_DESC : public CPartEffect::PARTEFFECT_DESC
	{
	};

protected:
	CFiora_W_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFiora_W_1(const CFiora_W_1& Prototype);
	virtual ~CFiora_W_1() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom[2]{};

	_float3 m_vColor[2]{};
	_float m_fAlpha[2]{};

	_uint m_iBlendState[2]{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFiora_W_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END