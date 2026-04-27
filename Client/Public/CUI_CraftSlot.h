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
class CItem_Manager;

class CUI_CraftSlot final : public CUI_Btn
{
public:
	struct CUI_CRAFTSLOT_DESC : public CUI_Btn::CUI_BTN_DESC
	{
	};

protected:
	CUI_CraftSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CraftSlot(const CUI_CraftSlot& Prototype);
	virtual ~CUI_CraftSlot() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_CraftItem(_int iItemId, _int iCraftCnt);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;
	void Execute_Btn();

	HRESULT Ready_Layer_UI_CraftItem(const _wstring& strLayerTag);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CUI_ItemImage* m_pItemImage{};

	CItem_Manager* m_pCItem_Manager{};

	_int m_iCurItemId{};
	_uint m_iCraftCnt{};
	wstring m_wstrCraftCnt{ L"" };

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	_float m_fImagePosX{};
	_float m_fImagePosY{};

public:
	static CUI_CraftSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END