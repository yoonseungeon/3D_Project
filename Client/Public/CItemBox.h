#pragma once

#include "Client_Defines.h"
#include "CItemSpawner.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CItemBox final : public CItemSpawner
{
public:
	struct ITEMBOX_DESC : public CItemSpawner::ITEMSPAWNER_DESC
	{
	};

protected:
	CItemBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemBox(const CItemBox& Prototype);
	virtual ~CItemBox() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutLine() override;

private:
	_uint m_iMaxSize{ 10 };

private:
	HRESULT Ready_Components(wstring wstrModelPrototypeTag);
	HRESULT Bind_ShaderResources();

	HRESULT Generate_Item();

public:
	static CItemBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END