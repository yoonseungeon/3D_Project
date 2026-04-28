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

	_bool Craft_Item(_int iItemId);

	const vector<INVENTORY_SLOT>& Get_InventoryVec() const { return m_Inventory; }
	_uint Get_ChangeInvenFlag() const { return m_iInvenChangeFlag; }

	const vector<INVENTORY_SLOT>& Get_CanCraftItemsVec() const { return m_CanCraftItems; }
	_uint Get_ChangeCraftFlag() const { return m_iCraftChangeFlag; }

private:
	vector<INVENTORY_SLOT> m_Inventory;
	_uint m_iInvenChangeFlag{};
	_uint iUseSlotCnt{};

	vector<INVENTORY_SLOT> m_CanCraftItems;
	_uint m_iCraftChangeFlag{};

	CItem_Manager* m_pItem_Manager{};

private:
	_int FindItemSlot(_int iItemId);

	void Add_InvenChangeFlag() { ++m_iInvenChangeFlag; }
	void Add_CraftChangeFlag() { ++m_iCraftChangeFlag; }

	void Reset_CraftItems();
	_bool Can_Craft(_uint iItemIdx);

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