#pragma once

#include "Client_Defines.h"
#include "CItemSpawner.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CItemBox_Collectible final : public CItemSpawner
{
public:
	struct ITEMBOX_COLLECTIBLE_DESC : public CItemSpawner::ITEMSPAWNER_DESC
	{
		_int iItemId{ -1 };
	};

protected:
	CItemBox_Collectible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemBox_Collectible(const CItemBox_Collectible& Prototype);
	virtual ~CItemBox_Collectible() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_int Get_ItemId() { return m_iItemId; }

private:
	_int m_iItemId{ -1 };

private:
	HRESULT Ready_Components(wstring wstrModelPrototypeTag);
	HRESULT Bind_ShaderResources();

public:
	static CItemBox_Collectible* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END