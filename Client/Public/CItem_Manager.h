#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CItem_Manager final : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

private:
	CItem_Manager();
	virtual ~CItem_Manager() = default;

public:
	const ITEM_DESC* Find_ItemInfo(_uint iItemIdx) const;

	const unordered_map<_uint, ITEM_DESC>& Get_ItemInfos() const { return m_ItemInfos; }

	const vector<_uint>* Get_CanCraftItemCandidates(_uint iItemIdx) const;

	_float Get_CurItemCraftTime(_uint iItemIdx);

private:
	HRESULT Initialize();

	HRESULT Set_ItemInfos();
	HRESULT Set_Reversematerials();
	HRESULT Set_ItemCraftTime();

private:
	unordered_map<_uint, ITEM_DESC> m_ItemInfos;
	unordered_map<_int, vector<_uint>> m_Reversematerials;

	_float m_fItemCraftTimes[static_cast<int>(ITEM_GRADE::ITEM_GRADE_END)];

protected:
	virtual void Free() override;
};

NS_END
