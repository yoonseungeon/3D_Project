#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(Client)

class CExplosion final : public CGameObject
{
protected:
	CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosion(const CExplosion& Prototype);
	virtual ~CExplosion() = default;

private:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END