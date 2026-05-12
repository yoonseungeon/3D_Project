#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CRapier final : public CPartObject
{
public:
	struct RAPIER_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketBoneMatrix{ nullptr };
	};

protected:
	CRapier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRapier(const CRapier& Prototype);
	virtual ~CRapier() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;

private:
	CShader* m_pShaderCom{ nullptr };
	CMyModel* m_pModelCom{ nullptr };

private:
	const _float4x4* m_pSocketBoneMatrix{ nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRapier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END