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

	if (bResult == true) {
		Reset_CraftItems();
	}

	return bResult;
}

_bool CInventory::Subtract_ItemByItemId(_int iItemId, _uint iItemCount)
{
	_bool bResult = Subtract_ItemByItemId_CraftNoReset(iItemId, iItemCount);

	if (bResult == true) {
		Reset_CraftItems();
	}

	return bResult;
}

_bool CInventory::Subtract_ItemBySlotIndex(_uint iSlotIndex, _uint iItemCount)
{
	_bool bResult = Subtract_ItemBySlotIndex_CraftNoReset(iSlotIndex, iItemCount);

	if (bResult == true) {
		Reset_CraftItems();
	}

	return bResult;
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

		if (FindItemSlot(iItemIdx) == -1)
			return false;
	}

	// 재료 있으면
	for (_int i = 0; i < sizeof(pItemDesc->materials) / sizeof(_int); ++i)
	{
		_int iItemIdx = pItemDesc->materials[i];

		if (iItemIdx == -1)
			continue;		

		Subtract_ItemByItemId_CraftNoReset(iItemIdx, 1);
	}

	// 삽입 실패시 재료 아이템이 사라짐
	return Add_Item(iItemId, pItemDesc->iCraftCnt);
}

_int CInventory::FindItemSlot(_int iItemId)
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

void CInventory::Reset_CraftItems()
{
	Add_CraftChangeFlag();
	m_CanCraftItems.clear();

	unordered_set<_uint> CandidateItems;

	for (_uint i = 0; i < m_Inventory.size(); ++i)
	{
		_int iItemIdx = m_Inventory[i].iItemId;

		if (iItemIdx == -1)
		{
			continue;
		}

		const vector<_uint>* CanCraftItemCandidates = m_pItem_Manager->Get_CanCraftItemCandidates(iItemIdx);

		if (CanCraftItemCandidates == nullptr)
		{
			continue;
		}

		// 중복 제거
		for (_int iItemIdx : *CanCraftItemCandidates)
		{
			CandidateItems.insert(iItemIdx);
		}
	}

	for (_int iItemIdx : CandidateItems)
	{
		if (Can_Craft(iItemIdx) == true)
		{
			const ITEM_DESC* tItemDesc = m_pItem_Manager->Find_ItemInfo(iItemIdx);
			m_CanCraftItems.emplace_back(INVENTORY_SLOT{ iItemIdx, tItemDesc->iCraftCnt });
		}
	}

	std::sort(m_CanCraftItems.begin(), m_CanCraftItems.end(),
		[this](const INVENTORY_SLOT& tLeft, const INVENTORY_SLOT& tRight)->_bool
		{
			const ITEM_DESC* tLeftItemDesc = m_pItem_Manager->Find_ItemInfo(tLeft.iItemId);
			const ITEM_DESC* tRightItemDesc = m_pItem_Manager->Find_ItemInfo(tRight.iItemId);

			return tLeftItemDesc->eGrade > tRightItemDesc->eGrade;
		}
	);
}

_bool CInventory::Can_Craft(_uint iItemIdx)
{
	// 같은 아이템이 제작에 두 개 필요한 아이템은 없음 -> 체크 로직 없음

	const ITEM_DESC* tItem_Desc = m_pItem_Manager->Find_ItemInfo(iItemIdx);

	if (tItem_Desc == nullptr)
	{
		MSG_BOX("No ITEM_DESC In CItem_Manager: CUI_Inventory");
		return false;
	}

	for (_uint i = 0; i < sizeof(tItem_Desc->materials) / sizeof(_int); ++i)
	{
		if (tItem_Desc->materials[i] == -1)
		{
			continue;
		}

		_bool bFind{};

		for (_uint j = 0; j < m_Inventory.size(); ++j)
		{
			if (m_Inventory[j].iItemId == tItem_Desc->materials[i])
			{
				bFind = true;
				break;
			}
		}

		if (bFind == false)
		{
			return false;
		}
	}

	return true;
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
	_int iIndex = FindItemSlot(iItemId);

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
