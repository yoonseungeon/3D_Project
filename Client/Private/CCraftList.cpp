#include "CCraftList.h"

#include "CItem_Manager.h"

#include "CInventory.h"
#include "CEquipment.h"

CCraftList::CCraftList()
	: m_pItem_Manager(CItem_Manager::GetInstance())
{
	Safe_AddRef(m_pItem_Manager);
}

HRESULT CCraftList::Initialize()
{

	return S_OK;
}

void CCraftList::Sync_CraftList(const CInventory* pInventory, const CEquipment* pEquipment)
{
	m_TotalItems.clear();

	const vector<INVENTORY_SLOT> Inventory = pInventory->Get_InventoryVec();

	for (const INVENTORY_SLOT& tIInvenSlot : Inventory)
	{
		if (tIInvenSlot.iItemId != -1) {
			m_TotalItems.push_back(tIInvenSlot);
		}
	}

	const vector<EQUIPMENT_SLOT> Equipment = pEquipment->Get_EquipmentsVec();

	for (const EQUIPMENT_SLOT& tEquipmentSlot : Equipment)
	{
		if (tEquipmentSlot.iItemId != -1) {
			m_TotalItems.emplace_back(INVENTORY_SLOT{ tEquipmentSlot.iItemId, 1 });
		}
	}

	Reset_CraftItems();
}

_bool CCraftList::Craft_Item(_int iItemId, CInventory* pInventory, CEquipment* pEquipment)
{
	// 조합 아이템이 똑같은 거 두 개인 경우는 없음 -> 구현 x

	if (iItemId == -1)
	{
		return false;
	}

	const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemId);
	if (pItemDesc == nullptr)
	{
		MSG_BOX("No ItemInfo In CItem_Manger: CCraftList");
		return false;
	}

	vector<_int> InventoryIndex;
	vector<_int> EquipmentIndex;

	// 재료 있는지 검사
	for (_int i = 0; i < sizeof(pItemDesc->materials) / sizeof(_int); ++i)
	{
		_int iItemIdx = pItemDesc->materials[i];

		if (iItemIdx == -1)
		{
			continue;
		}

		_int iInvenSlotIndex = pInventory->FindItemSlotByItemId(iItemIdx);
		if (iInvenSlotIndex != -1)
		{
			InventoryIndex.push_back(iInvenSlotIndex);
			continue;
		}

		_int iEquipmentSlotIndex = pEquipment->Find_SlotByItemId(iItemIdx);
		if (iEquipmentSlotIndex != -1)
		{
			EquipmentIndex.push_back(iEquipmentSlotIndex);
			continue;
		}

		return false;
	}

	_bool bInvenAddResult = pInventory->Add_Item(iItemId, pItemDesc->iCraftCnt);

	if (bInvenAddResult == false)
	{
		return false;
	}

	for (_int iSlotIndex : InventoryIndex)
	{
		pInventory->Subtract_ItemBySlotIndex(iSlotIndex);
	}

	for (_int iSlotIndex : EquipmentIndex)
	{
		_int iDummy{};
		pEquipment->Unequip_ItemBySlotIndex(iSlotIndex, iDummy);
	}

	Sync_CraftList(pInventory, pEquipment);
	return true;
}

void CCraftList::Reset_CraftItems()
{
	Add_CraftChangeFlag();
	m_CanCraftItems.clear();

	unordered_set<_uint> CandidateItems;

	for (_uint i = 0; i < m_TotalItems.size(); ++i)
	{
		_int iItemIdx = m_TotalItems[i].iItemId;

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

_bool CCraftList::Can_Craft(_uint iItemIdx)
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

		for (_uint j = 0; j < m_TotalItems.size(); ++j)
		{
			if (m_TotalItems[j].iItemId == tItem_Desc->materials[i])
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

CCraftList* CCraftList::Create()
{
	CCraftList* pInstance = new CCraftList();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CCraftList");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCraftList::Free()
{
	Safe_Release(m_pItem_Manager);

	__super::Free();
}
