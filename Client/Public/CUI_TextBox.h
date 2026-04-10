#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

NS_BEGIN(Client)

class CUI_TextBox final : public CUI_Image
{

public:
	struct CUI_TEXTBOX_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
		wstring wstrText;

		_float fOffsetX{};
		_float fOffsetY{};

		_float fTextureSize{ 1.f };
	};

protected:
	CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextBox(const CUI_TextBox& Prototype);
	virtual ~CUI_TextBox() = default;

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
	RECT m_rcTextRange{};
	_float		m_fHalfSizeX{};
	_float		m_fHalfSizeY{};
	_float m_fTextureSize{ 1.f };
	wstring m_wstrText;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_TextBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END