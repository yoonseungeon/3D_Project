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

_bool CInventory::Add_Item(_int iItemId)
{
	_int iFrontIdx = { -1 };

	for (_uint i = 0; i < m_Inventory.size(); ++i)
	{

		if (iFrontIdx == -1 && m_Inventory[i].iItemId == -1)
		{
			iFrontIdx = i;
		}


		if (m_Inventory[i].iItemId == iItemId)
		{
			const ITEM_DESC* pItemDesc = m_pItem_Manager->Find_ItemInfo(iItemId);
			
			if (pItemDesc == nullptr)
			{
				MSG_BOX("No ItemInfo In CItem_Manger: CInventory");
				return false;
			}

			if (m_Inventory[i].iItemCnt < pItemDesc->iMaxCnt)
			{
				++(m_Inventory[i].iItemCnt);
				Add_ChangeFlag();
				return true;
			}
		}
	}

	if (iFrontIdx != -1)
	{
		m_Inventory[iFrontIdx].iItemId = iItemId;
		m_Inventory[iFrontIdx].iItemCnt = 1;
		Add_ChangeFlag();
		return true;
	}

	return false;
}

_bool CInventory::Decrease_Item(_uint iInvenIdx)
{
	if (iInvenIdx >= m_Inventory.size()) {
		MSG_BOX("m_Inventory Out Of Range: CInventory");
		return false;
	}

	Add_ChangeFlag();

	INVENTORY_SLOT& tSlot = m_Inventory[iInvenIdx];

	if (tSlot.iItemId == -1)
	{
		return false;
	}

	--(tSlot.iItemCnt);

	if (tSlot.iItemCnt == 0)
	{
		tSlot.iItemId = -1;
	}

	Add_ChangeFlag();
	return true;
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
