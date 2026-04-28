#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Client)

class CUI_InventorySlot;
class CUI_CraftSlot;

class CUI_Craft : public CUI_Default
{
private:
	CUI_Craft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Craft(const CUI_Craft& Prototype);
	virtual ~CUI_Craft() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<INVENTORY_SLOT> m_UICanCraftItems;
	_uint m_iCraftChangeFlag{};

	vector<CUI_CraftSlot*> m_Slots;

private:
	HRESULT Initialize_CraftSlot();
	HRESULT Ready_Layer_UI_CraftSlot(const _wstring& strLayerTag);
	HRESULT Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc);

	void Sync_CanCraftItems();
	void Reset_CraftItemSlot(const vector<INVENTORY_SLOT>& UICanCraftItems);

public:
	static CUI_Craft* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END