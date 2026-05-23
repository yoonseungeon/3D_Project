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

class CSplitGround final : public CGameObject
{
public:
	struct SPLIT_GROUND_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrPrototype_Component_Tag;
	};

protected:
	CSplitGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSplitGround(const CSplitGround& Prototype);
	virtual ~CSplitGround() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnMouse_Over_All(const COLLISION_RAY_INFO& tRayInfo) override;

public:
	_bool Picking(_float3& vOutPos);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };

	wstring m_wstrPrototype_Component_Tag;

	CInGame_Manager* m_pInGame_Manager{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CSplitGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

NS_END