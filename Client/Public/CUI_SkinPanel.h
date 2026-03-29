#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CPickSkin;
class CUI_Image;
class CGame_Manager;
class CCharData_Manager;

class CUI_SkinPanel : public CUI_Default
{
public:
	struct CUI_SKINPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CUI_SkinPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkinPanel(const CUI_SkinPanel& Prototype);
	virtual ~CUI_SkinPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	vector<CPickSkin*> m_PickSkins;
	CUI_Image* m_pFullSkin{};

	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};

	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	CGame_Manager* m_pGame_Manaer{};
	CCharData_Manager* m_pCharData_Manager{};

	CHAR_NAME m_eCurChar{ CHAR_NAME::CHARNAME_END };
	CHAR_NAME m_ePreChar{ CHAR_NAME::CHARNAME_END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_CUI_Image(const _wstring& strLayerTag);
	HRESULT Ready_Layer_SkinSlot(const _wstring& strLayerTag);

public:
	static CUI_SkinPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END