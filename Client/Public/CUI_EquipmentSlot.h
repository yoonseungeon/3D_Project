#pragma once

#include "Client_Defines.h"
#include "CUI_Btn.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_ItemImage;
class CUI_Image;

class CUI_EquipmentSlot final : public CUI_Btn
{

public:
	enum EQUIPMENT_SLOT_TYPE { WEAPON, CLOTH, HEAD, ARM, LEG, EQUIPMENT_SLOT_TYPE_END };

public:
	struct CUI_EQUIPMENT_SLOT_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		EQUIPMENT_SLOT_TYPE eSlotType{ EQUIPMENT_SLOT_TYPE_END };
	};


protected:
	CUI_EquipmentSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_EquipmentSlot(const CUI_EquipmentSlot& Prototype);
	virtual ~CUI_EquipmentSlot() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Sync_Slot_Bg_Item(_int iItemId, _uint iItemCnt = 1);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;

	HRESULT Ready_Layer_UI_EquipmentBg(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_EquipmentItemBg(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_EquipmentItem(const _wstring& strLayerTag);

	void Set_ItemBg(_int iItemId);
	void Set_Item(_int iItemId, _uint iItemCnt = 1);

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

	EQUIPMENT_SLOT_TYPE m_eSlotType{ EQUIPMENT_SLOT_TYPE_END };

public:
	static CUI_EquipmentSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END