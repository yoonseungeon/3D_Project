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
	_bool Add_Item(_int iItemId, _uint iItemCount = 1);
	_bool Subtract_ItemByItemId(_int iItemId, _uint iItemCount = 1);
	_bool Subtract_ItemBySlotIndex(_uint iSlotIndex, _uint iItemCount = 1);

	_int FindItemSlotByItemId(_int iItemId);
	_int FindItemIdBySlotIndex(_uint iSlotIndex);
	_int FindItemIdBySlotIndex(_uint iSlotIndex, _uint& iOutItemCnt);

	const vector<INVENTORY_SLOT>& Get_InventoryVec() const { return m_Inventory; }
	_uint Get_ChangeInvenFlag() const { return m_iInvenChangeFlag; }
	size_t Get_InventorySize() { return m_Inventory.size(); }

	void PullSlots();

private:
	vector<INVENTORY_SLOT> m_Inventory;
	_uint m_iInvenChangeFlag{};
	_uint iUseSlotCnt{};

	CItem_Manager* m_pItem_Manager{};

private:
	void Add_InvenChangeFlag() { ++m_iInvenChangeFlag; }

private:
	_bool Add_Item_CraftNoReset(_int iItemId, _uint iItemCount = 1);
	_bool Subtract_ItemByItemId_CraftNoReset(_int iItemId, _uint iItemCount = 1);
	_bool Subtract_ItemBySlotIndex_CraftNoReset(_uint iSlotIndex, _uint iItemCount = 1);

public:
	static CInventory* Create();
protected:
	virtual void Free();
};

NS_END