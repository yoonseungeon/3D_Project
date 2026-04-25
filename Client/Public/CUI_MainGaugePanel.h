#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_MainGaugePanel final : public CUI_Default
{
public:
	struct CUI_MAINGAUGE_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
		MAINGAUGE_TYPE eMainGaugeType{};
	};

protected:
	CUI_MainGaugePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MainGaugePanel(const CUI_MainGaugePanel& Prototype);
	virtual ~CUI_MainGaugePanel() = default;

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
	HRESULT Ready_Layer_MainGaugeBar(const _wstring& strLayerTag);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	MAINGAUGE_TYPE m_eMainGaugeType{};

	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

public:
	static CUI_MainGaugePanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END