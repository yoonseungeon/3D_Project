#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CItem_Manager;

class CInventory final : public CBase
{
public:
	struct INVENTORY_SLOT
	{
		_int iItemId{ -1 };
		_uint iItemCnt{};
	};

private:
	CInventory();
	virtual ~CInventory() = default;

private:
	HRESULT Initialize();

public:
	_bool Add_Item(_int iItemId);
	_bool Decrease_Item(_uint iInvenIdx);

private:
	_int FindItem(_int iItemId);

private:
	vector<INVENTORY_SLOT> Inventory;

	CItem_Manager* m_pItem_Manager{};

public:
	static CInventory* Create();
protected:
	virtual void Free();
};

NS_END