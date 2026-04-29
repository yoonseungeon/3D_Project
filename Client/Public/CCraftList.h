#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CItem_Manager;
class CInventory;
class CEquipment;

class CCraftList final : public CBase
{
private:
	CCraftList();
	virtual ~CCraftList() = default;

private:
	HRESULT Initialize();

public:
	void Sync_CraftList(const CInventory* pInventory, const CEquipment* pEquipment);

	_bool Craft_Item(_int iItemId, CInventory* pInventory, CEquipment* pEquipment);

	const vector<INVENTORY_SLOT>& Get_CanCraftItemsVec() const { return m_CanCraftItems; }
	_uint Get_ChangeCraftFlag() const { return m_iCraftChangeFlag; }

private:
	vector<INVENTORY_SLOT> m_TotalItems;

	vector<INVENTORY_SLOT> m_CanCraftItems;
	_uint m_iCraftChangeFlag{};


	CItem_Manager* m_pItem_Manager{};

private:
	void Add_CraftChangeFlag() { ++m_iCraftChangeFlag; }

	void Reset_CraftItems();
	_bool Can_Craft(_uint iItemIdx);

public:
	static CCraftList* Create();
protected:
	virtual void Free();
};

NS_END