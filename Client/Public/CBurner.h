#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CBurner final : public CPartObject
{
public:
	struct BURNER_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketBoneMatrix{ nullptr };
	};

protected:
	CBurner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBurner(const CBurner& Prototype);
	virtual ~CBurner() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom{ nullptr };
	CMyModel* m_pModelCom{ nullptr };

private:
	const _float4x4* m_pSocketBoneMatrix{ nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBurner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END