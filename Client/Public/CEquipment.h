#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CItem_Manager;

class CEquipment final : public CBase
{
private:
	CEquipment();
	virtual ~CEquipment() = default;

private:
	HRESULT Initialize(ITEM_TYPE eWeaponType);

public:
	const vector<EQUIPMENT_SLOT>& Get_EquipmentsVec() const { return m_Equipments; }
	_uint Get_ChangeEquipmentChangeFlag() const { return m_iEquipmentChangeFlag; }

	_int Equip_Item(_int iItemIndex);

private:
	CItem_Manager* m_pItem_Manager{};

	vector<EQUIPMENT_SLOT> m_Equipments;
	_uint m_iEquipmentChangeFlag{};

	ITEM_TYPE m_eWeaponItemType{};

private:
	_int Find_Slot(ITEM_TYPE eItemType);
	void Add_EquipmentChangeFlag() { ++m_iEquipmentChangeFlag; }

private:

public:
	static CEquipment* Create(ITEM_TYPE eWeaponType);
protected:
	virtual void Free();
};

NS_END