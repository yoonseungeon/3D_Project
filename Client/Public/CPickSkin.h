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
class CGame_Manager;

class CPickSkin final : public CUI_Btn
{
private:
	enum TEX_STATE { HOVER, SELECTED, NONE };

public:
	struct CPICKSKIN_DESC : public CUI_Btn::CUI_BTN_DESC
	{

	};

private:
	CPickSkin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPickSkin(const CPickSkin& Prototype);
	virtual ~CPickSkin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_Skin(LEVEL eTexPrototypeLV, const wstring& wstrTexturePrototypeTag);
	virtual void Set_IsInactive(_bool bIsInactive) override;
	void Set_SkinIdx(_uint iSkinIdx);

	void Set_Deselect();
	void Set_Select();
	void Set_SkinName(const wstring& wstrSkinName);

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

	TEX_STATE m_eCurTexState{ TEX_STATE::NONE };

	CUI_Image* m_pSkin{};

	_bool m_bIsSelected{};

	wstring m_wstrSkinName;

public:
	static CPickSkin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END