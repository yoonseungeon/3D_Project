#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_StatPanel final : public CUI_Default
{
public:
	struct CUI_STATPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_StatPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StatPanel(const CUI_StatPanel& Prototype);
	virtual ~CUI_StatPanel() = default;

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
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

public:
	static CUI_StatPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END