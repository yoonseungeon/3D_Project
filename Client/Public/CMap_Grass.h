#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CInGame_Manager;

class CMap_Grass final : public CGameObject
{
public:
	struct MAP_GRASS_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrPrototype_Component_Tag;
	};

protected:
	CMap_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Grass(const CMap_Grass& Prototype);
	virtual ~CMap_Grass() = default;

private:
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

	wstring m_wstrPrototype_Component_Tag;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMap_Grass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END