#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CLumia_Structure final : public CGameObject
{
public:
	struct LUMIA_STRUCTURE_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

protected:
	CLumia_Structure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLumia_Structure(const CLumia_Structure& Prototype);
	virtual ~CLumia_Structure() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float3 Picking();

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CLumia_Structure* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

NS_END