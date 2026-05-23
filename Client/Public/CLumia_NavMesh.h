#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CLumia_NavMesh final : public CGameObject
{
public:
	struct LUMIA_NAVMESH_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

protected:
	CLumia_NavMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLumia_NavMesh(const CLumia_NavMesh& Prototype);
	virtual ~CLumia_NavMesh() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CLumia_NavMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END