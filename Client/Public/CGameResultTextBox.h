#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)
class CGameResultTextBox final : public CUI_Default
{
public:
	struct GAMERESULT_TEXTBOX_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CGameResultTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameResultTextBox(const CGameResultTextBox& Prototype);
	virtual ~CGameResultTextBox() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void GameResultStart();
	void GameResultReset();

	void Set_Ratio(_float fRatio) { m_fRatio = fRatio; }

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fRatio{};

	_float m_fScaleY{};
	
	wstring m_wstrText;
	_bool m_bRenderText{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Update_GameResult();
	HRESULT RenderText();

public:
	static CGameResultTextBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END