#pragma once

#include "Client_Defines.h"
#include "CUI_Btn.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CLobbyTabBtn final : public CUI_Btn
{
public:
	struct CLOBBY_TAB_BTN_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		function<void(STAGE)> funcLobbyCallBack{};
		STAGE eNextStage{};
	};

protected:
	CLobbyTabBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobbyTabBtn(const CLobbyTabBtn& Prototype);
	virtual ~CLobbyTabBtn() = default;

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
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fFillX{ 0.f };
	_float m_fFillCenterY{ 0.f };
	function<void(STAGE)> m_funcLobbyCallBack{};
	STAGE m_eNextStage{};

	_float m_fFontColor{};
	_float m_fFontRatio{};

public:
	static CLobbyTabBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END