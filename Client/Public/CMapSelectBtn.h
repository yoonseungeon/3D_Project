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

class CMapSelectBtn final : public CUI_Btn
{
private:
	enum TEX_STATE { HOVER, NONE };

public:
	struct CMAPSELECTBTN_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		MAP_NAME eMapName{};
	};

private:
	CMapSelectBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapSelectBtn(const CMapSelectBtn& Prototype);
	virtual ~CMapSelectBtn() = default;

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
	void Set_Deselect();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader*		m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture*		m_pTextureCom{ nullptr };

	TEX_STATE m_eCurTexState{ TEX_STATE::NONE };

	MAP_NAME m_eMapName{};
	CUI_Image* m_pSelectImage{};

	_bool m_bIsSelected{};

	CGame_Manager* m_pGame_Manager{};

public:
	static CMapSelectBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END