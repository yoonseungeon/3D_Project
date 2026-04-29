#include "CEquipment.h"

#include "CItem_Manager.h"

CEquipment::CEquipment()
	: m_pItem_Manager(CItem_Manager::GetInstance())
{
	Safe_AddRef(m_pItem_Manager);
}

HRESULT CEquipment::Initialize(ITEM_TYPE eWeaponType)
{
	m_eWeaponItemType = eWeaponType;

	m_Equipments =
	{
		{-1, eWeaponType},
		{-1, ITEM_TYPE::CLOTH},
		{-1, ITEM_TYPE::HEAD},
		{-1, ITEM_TYPE::ARM},
		{-1, ITEM_TYPE::LEG},

	};

	return S_OK;
}

_bool CEquipment::Equip_Item(_int iItemIndex, _int& iPreItemIndex)
{
	iPreItemIndex = -1;

	const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemIndex);
	if (pItemDesc == nullptr)
	{
		MSG_BOX("No ItemInfo In CItem_Manger: CEquipment");
		return false;
	}

	_int iSlotIndex = Find_Slot(pItemDesc->eType);

	if (iSlotIndex == -1) {
		return false;
	}

	iPreItemIndex = m_Equipments[iSlotIndex].iItemId;
	m_Equipments[iSlotIndex].iItemId = iItemIndex;
	
	Add_EquipmentChangeFlag();

	return true;
}

_int CEquipment::Find_Slot(ITEM_TYPE eItemType)
{
	for (_uint i = 0; i < m_Equipments.size(); ++i)
	{
		if (m_Equipments[i].eItemType == eItemType)
			return i;
	}

	return -1;
}

CEquipment* CEquipment::Create(ITEM_TYPE eWeaponType)
{
	CEquipment* pInstance = new CEquipment();

	if (FAILED(pInstance->Initialize(eWeaponType)))
	{
		MSG_BOX("Failed to Created: CEquipment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEquipment::Free()
{
	Safe_Release(m_pItem_Manager);

	__super::Free();
}
