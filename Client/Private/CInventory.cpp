#include "CInventory.h"

#include "CItem_Manager.h"

CInventory::CInventory()
	: m_pItem_Manager(CItem_Manager::GetInstance())
{
	Safe_AddRef(m_pItem_Manager);
}

HRESULT CInventory::Initialize()
{
	Inventory.resize(10);

	return S_OK;
}

_bool CInventory::Add_Item(_int iItemId)
{
	_int iFrontIdx = { -1 };

	for (_uint i = 0; i < Inventory.size(); ++i)
	{

		if (iFrontIdx == -1 && Inventory[i].iItemId == -1)
		{
			iFrontIdx = i;
		}


		if (Inventory[i].iItemId == iItemId)
		{
			const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemId);
			
			if (pItemDesc == nullptr)
			{
				MSG_BOX("No ItemInfo In CItem_Manger: CInventory");
				return false;
			}

			if (Inventory[i].iItemCnt < pItemDesc->iMaxCnt)
			{
				++(Inventory[i].iItemCnt);
				return true;
			}
		}
	}

	if (iFrontIdx != -1)
	{
		Inventory[iFrontIdx].iItemId = iItemId;
		Inventory[iFrontIdx].iItemCnt = 1;
		return true;
	}

	return false;
}

_bool CInventory::Decrease_Item(_uint iInvenIdx)
{
	if (iInvenIdx >= Inventory.size()) {
		MSG_BOX("Inventory Out Of Range: CInventory");
		return false;
	}

	INVENTORY_SLOT& tSlot = Inventory[iInvenIdx];

	if (tSlot.iItemId == -1)
	{
		return false;
	}

	--(tSlot.iItemCnt);

	if (tSlot.iItemCnt == 0)
	{
		tSlot.iItemId = -1;
	}

	return true;
}

_int CInventory::FindItem(_int iItemId)
{
	for (_int i = 0; i < Inventory.size(); ++i)
	{
		if (Inventory[i].iItemId == iItemId)
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
