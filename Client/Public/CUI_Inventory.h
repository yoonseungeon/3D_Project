#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Client)

class CUI_InventorySlot;
class CItem_Manager;
class CUI_Craft;

class CUI_Inventory : public CUI_Default
{
private:
	CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Inventory(const CUI_Inventory& Prototype);
	virtual ~CUI_Inventory() = default;

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
	vector<CUI_InventorySlot*> m_Slots;

	vector<INVENTORY_SLOT> m_UIInventory;
	_uint m_iInvenChangeFlag{};
	CUI_Craft* m_pCraft{};

	CItem_Manager* m_pCItem_Manager{};

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_InventorySlot(const _wstring& strLayerTag);
	HRESULT Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc);
	HRESULT Ready_Layer_UI_Craft(const _wstring& strLayerTag);

	HRESULT Initialize_Inventory();
	void Sync_Inventory();
	void Sync_InventorySlot();

public:
	static CUI_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END