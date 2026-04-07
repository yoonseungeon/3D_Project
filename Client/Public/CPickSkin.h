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
class CUI_MaskImage;
class CGame_Manager;
class CCharData_Manager;

class CPickSkin final : public CUI_Btn
{
private:
	enum TEX_STATE { HOVER, SELECTED, NONE };

public:
	struct CPICKSKIN_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		function<void(const wstring, const _uint)> funcSetFullSkin{};
	};

private:
	CPickSkin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPickSkin(const CPickSkin& Prototype);
	virtual ~CPickSkin() = default;

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
	void Reset_Skin(LEVEL eTexPrototypeLV, const wstring& wstrTexturePrototypeTag);
	virtual void Set_IsInactive(_bool bIsInactive) override;

	void Set_eCharName(CHAR_NAME eCharName);
	void Set_SkinIdx(_uint iSkinIdx);
	void Set_SkinName(const wstring& wstrSkinName);

	void Set_Deselect();
	void Set_Select();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CGame_Manager* m_pGame_Manager{};
	CCharData_Manager* m_pCharData_Manager{};

	TEX_STATE m_eCurTexState{ TEX_STATE::NONE };

	CUI_MaskImage* m_pSkin{};
	CUI_Image* m_pSlotBg{};

	_bool m_bIsSelected{};

	CHAR_NAME m_eCharName{ CHAR_NAME::CHARNAME_END };
	wstring m_wstrSkinName;
	_uint m_iSkinIdx{};

	function<void(const wstring, const _uint)> m_funcSetFullSkin{};

public:
	static CPickSkin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END