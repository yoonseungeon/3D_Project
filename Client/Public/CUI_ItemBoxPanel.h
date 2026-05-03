#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CItem_Manager;

class CUI_ItemBoxPanel : public CUI_Default
{
public:
	struct CUI_ITEMBOXPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{

	};

private:
	CUI_ItemBoxPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemBoxPanel(const CUI_ItemBoxPanel& Prototype);
	virtual ~CUI_ItemBoxPanel() = default;

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
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CItem_Manager* m_pCItem_Manager{};

	vector<INVENTORY_SLOT> m_ItemBoxInventory;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


	HRESULT Ready_Layer_UI_InventorySlot(const _wstring& strLayerTag);
	HRESULT Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc);

	HRESULT Initialize_Inventory();
	void Sync_InventorySlot();

public:
	static CUI_ItemBoxPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END