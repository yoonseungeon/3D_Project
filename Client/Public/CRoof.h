#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CRoof final : public CGameObject
{
public:
	struct ROOF_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrModelTag;
	};

protected:
	CRoof(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRoof(const CRoof& Prototype);
	virtual ~CRoof() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };

	wstring m_wstrModelTag;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CRoof* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

NS_END