#pragma once

#include "Client_Defines.h"
#include "CUI_Btn.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Image;
class CUI_ItemImage;

class CUI_InventorySlot final : public CUI_Btn
{
public:
	struct CUI_INVENTORYSLOT_DESC : public CUI_Btn::CUI_BTN_DESC
	{
	};

protected:
	CUI_InventorySlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InventorySlot(const CUI_InventorySlot& Prototype);
	virtual ~CUI_InventorySlot() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Sync_Slot_Bg_Item(_int iItemId, _uint iItemCnt);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_UI_InventoryItemBg(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_InventoryItem(const _wstring& strLayerTag);

	virtual void BtnClick() override;

	void Set_ItemBg(_int iItemId);
	void Set_Item(_int iItemId, _uint iItemCnt);

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CUI_Image* m_pItemBg{};
	CUI_ItemImage* m_pItemImage{};

	_uint iItemCnt{};

public:
	static CUI_InventorySlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END