#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CWaterPlan final : public CGameObject
{
public:
	struct WATERPLAN_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

protected:
	CWaterPlan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterPlan(const CWaterPlan& Prototype);
	virtual ~CWaterPlan() = default;

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
	CMyModel* m_pModelCom = { nullptr };

	_float m_fAlpha{};
	_float3 m_vColor{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWaterPlan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END