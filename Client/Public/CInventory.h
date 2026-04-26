#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CItem_Manager;

class CInventory final : public CBase
{
private:
	CInventory();
	virtual ~CInventory() = default;

private:
	HRESULT Initialize();

public:
	_bool Add_Item(_int iItemId);
	_bool Decrease_Item(_uint iInvenIdx);

	const vector<INVENTORY_SLOT>& Get_InventoryVec() const { return m_Inventory; }

	_uint Get_ChangeFlag() const { return m_iChangeFlag; }

private:
	_int FindItem(_int iItemId);

	void Add_ChangeFlag() { ++m_iChangeFlag; }

private:
	vector<INVENTORY_SLOT> m_Inventory;

	CItem_Manager* m_pItem_Manager{};

	_uint m_iChangeFlag{};

public:
	static CInventory* Create();
protected:
	virtual void Free();
};

NS_END