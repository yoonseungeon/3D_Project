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
class CUI_ItemBoxSlot;
class CItemBox;
class CUI_Image;

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

	virtual void Set_IsInactive(_bool bIsInactive) override;

	void PopUp_ItemBoxUI(CItemBox* pItemBox);
	void PopDown_ItemBoxUI();


private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	_float m_fTextPosX{};
	_float m_fTextPosY{};

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CItem_Manager* m_pCItem_Manager{};

	vector<CUI_ItemBoxSlot*> m_Slots;
	vector<CUI_Image*> m_EmptySlots;
	CUI_Image* m_pDecoImage{};

	_uint iSlotCnt{ 10 };
	CItemBox* m_pItemBox{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_UI_ItemBoxSlot(const _wstring& strLayerTag);
	HRESULT Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc);
	HRESULT EmptySlot_Creator(const _wstring& strLayerTag, _float fScaleX, _float fScaleY, _float fPosX, _float fPosY);

	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

	void Sync_ItemBoxSlot();

public:
	static CUI_ItemBoxPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END