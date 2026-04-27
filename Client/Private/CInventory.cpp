#include "CInventory.h"

#include "CItem_Manager.h"

CInventory::CInventory()
	: m_pItem_Manager(CItem_Manager::GetInstance())
{
	Safe_AddRef(m_pItem_Manager);
}

HRESULT CInventory::Initialize()
{
	m_Inventory.resize(10);

	return S_OK;
}

_bool CInventory::Add_Item(_int iItemId, _uint iItemCount)
{
	if (iItemId == -1 || iItemCount == 0)
	{
		return false;
	}

	const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemId);
	if (pItemDesc == nullptr)
	{
		MSG_BOX("No ItemInfo In CItem_Manger: CInventory");
		return false;
	}
	
	const _uint iItemSlotPerMaxCount = pItemDesc->iMaxCnt;

	vector<_uint> EmptySlots;
	vector<_uint> SameItemSlots;

	_uint iSameSlotLeftCount{};

	for (_uint i = 0; i < m_Inventory.size(); ++i)
	{
		if (m_Inventory[i].iItemId == -1)
		{
			EmptySlots.push_back(i);
		}

		if (m_Inventory[i].iItemId == iItemId && m_Inventory[i].iItemCnt < iItemSlotPerMaxCount)
		{
			SameItemSlots.push_back(i);
			iSameSlotLeftCount += iItemSlotPerMaxCount - m_Inventory[i].iItemCnt;
		}
	}

	_uint iTotalLeftCount = static_cast<_uint>(EmptySlots.size()) * iItemSlotPerMaxCount + iSameSlotLeftCount;

	if (iTotalLeftCount < iItemCount)
	{
		return false;
	}

	Add_ChangeFlag();

	for (_uint iItemSlotIndex : SameItemSlots)
	{
		_uint iCurSlotLeftCount = iItemSlotPerMaxCount - m_Inventory[iItemSlotIndex].iItemCnt;
		if (iCurSlotLeftCount < iItemCount)
		{
			iItemCount -= iCurSlotLeftCount;
			m_Inventory[iItemSlotIndex].iItemCnt = iItemSlotPerMaxCount;
		}
		else
		{
			m_Inventory[iItemSlotIndex].iItemCnt += iItemCount;
			return true;
		}
	}

	for (_uint iItemSlotIndex : EmptySlots)
	{
		++iFullSlotCnt;
		m_Inventory[iItemSlotIndex].iItemId = iItemId;

		if (iItemSlotPerMaxCount < iItemCount)
		{
			iItemCount -= iItemSlotPerMaxCount;
			m_Inventory[iItemSlotIndex].iItemCnt = iItemSlotPerMaxCount;
		}
		else
		{
			m_Inventory[iItemSlotIndex].iItemCnt = iItemCount;
			return true;
		}
	}	

	return false;
}

_bool CInventory::Decrease_ItemByItemId(_int iItemId, _uint iItemCount)
{
	_int iIndex = FindItem(iItemId);

	if (iIndex == -1) {
		return false;
	}

	return Decrease_ItemBySlotIndex(iIndex, iItemCount);
}

_bool CInventory::Decrease_ItemBySlotIndex(_uint iSlotIndex, _uint iItemCount)
{
	if (iSlotIndex >= m_Inventory.size()) {
		MSG_BOX("m_Inventory Out Of Range: CInventory");
		return false;
	}

	INVENTORY_SLOT& tSlot = m_Inventory[iSlotIndex];

	if (tSlot.iItemId == -1)
	{
		return false;
	}

	if (tSlot.iItemCnt < iItemCount)
	{
		return false;
	}

	tSlot.iItemCnt -= iItemCount;

	if (tSlot.iItemCnt == 0)
	{
		tSlot.iItemId = -1;
		--iFullSlotCnt;
	}

	Add_ChangeFlag();
	return true;
}

_bool CInventory::Craft_Item(_int iItemId)
{
	// 조합 아이템이 똑같은 거 두 개인 경우는 없음 -> 구현 x

	if (iItemId == -1)
	{
		return false;
	}

	const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemId);
	if (pItemDesc == nullptr)
	{
		MSG_BOX("No ItemInfo In CItem_Manger: CInventory");
		return false;
	}

	// 재료 있는지 검사
	for (_int i = 0; i < sizeof(pItemDesc->materials) / sizeof(_int); ++i)
	{
		_int iItemIdx = pItemDesc->materials[i];

		if (iItemIdx == -1)
			continue;

		if (FindItem(iItemIdx) == -1)
			return false;
	}

	for (_int i = 0; i < sizeof(pItemDesc->materials) / sizeof(_int); ++i)
	{
		_int iItemIdx = pItemDesc->materials[i];

		if (iItemIdx == -1)
			continue;		

		Decrease_ItemByItemId(iItemIdx, 1);
	}

	return Add_Item(iItemId, pItemDesc->iCraftCnt);
}

_int CInventory::FindItem(_int iItemId)
{
	for (_int i = 0; i < m_Inventory.size(); ++i)
	{
		if (m_Inventory[i].iItemId == iItemId)
		{
			return i;
		}
	}
	return -1;
}

CInventory* CInventory::Create()
{
	CInventory* pInstance = new CInventory();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CInventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory::Free()
{
	Safe_Release(m_pItem_Manager);

    __super::Free();
}
