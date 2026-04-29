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
	_bool bResult = Add_Item_CraftNoReset(iItemId, iItemCount);

	return bResult;
}

_bool CInventory::Subtract_ItemByItemId(_int iItemId, _uint iItemCount)
{
	_bool bResult = Subtract_ItemByItemId_CraftNoReset(iItemId, iItemCount);

	return bResult;
}

_bool CInventory::Subtract_ItemBySlotIndex(_uint iSlotIndex, _uint iItemCount)
{
	_bool bResult = Subtract_ItemBySlotIndex_CraftNoReset(iSlotIndex, iItemCount);

	return bResult;
}

_int CInventory::FindItemSlotByItemId(_int iItemId)
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

_int CInventory::FindItemIdBySlotIndex(_uint iSlotIndex)
{
	if (iSlotIndex >= m_Inventory.size())
	{
		return -1;
	}

	return m_Inventory[iSlotIndex].iItemId;
}

_bool CInventory::Add_Item_CraftNoReset(_int iItemId, _uint iItemCount)
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

	Add_InvenChangeFlag();

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
		++iUseSlotCnt;
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

	// 올 일 없음
	return false;
}

_bool CInventory::Subtract_ItemByItemId_CraftNoReset(_int iItemId, _uint iItemCount)
{
	_int iIndex = FindItemSlotByItemId(iItemId);

	if (iIndex == -1) {
		return false;
	}

	// 여기도 최대 한 슬롯에 있는 개수만큼만 뺌. 수정 필요
	return Subtract_ItemBySlotIndex_CraftNoReset(iIndex, iItemCount);
}

_bool CInventory::Subtract_ItemBySlotIndex_CraftNoReset(_uint iSlotIndex, _uint iItemCount)
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
		--iUseSlotCnt;
	}

	Add_InvenChangeFlag();
	return true;
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
