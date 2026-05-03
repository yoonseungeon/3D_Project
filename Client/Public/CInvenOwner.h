#pragma once

#include "Client_Defines.h"
#include "CContainerObject.h"

NS_BEGIN(Client)

class CInventory;

class CInvenOwner abstract : public CContainerObject
{
protected:
	CInvenOwner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenOwner(const CInvenOwner& Prototype);
	virtual ~CInvenOwner() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// TransformCom
	const CTransform* Get_TransformCom() { return m_pTransformCom; }

	// Inventory
	const CInventory* Get_Inventory() const { return m_pInvetory; }
	CInventory* Get_Inventory() { return m_pInvetory; }

protected:
	CInventory* m_pInvetory{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END