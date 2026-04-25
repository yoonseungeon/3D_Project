#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_MainGaugeBar final : public CUI_Default
{
public:
	struct CUI_MAINGAUGEBAR_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
		MAINGAUGE_TYPE eGaugeType{ MAINGAUGE_TYPE::MP };
	};

protected:
	CUI_MainGaugeBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MainGaugeBar(const CUI_MainGaugeBar& Prototype);
	virtual ~CUI_MainGaugeBar() = default;

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

	void Reset_Gauge();

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fFillX{ 0.5f };
	_float m_fFillY{ 1.f };

	_int m_iCurData{ -1 };
	_int m_iMaxData{ -1 };

	_float m_fData{ -1.f };

	MAINGAUGE_TYPE m_eGaugeType{ MAINGAUGE_TYPE::MP };

	_float m_fScaleRatioY{};

	_float m_fImagePosX{};
	_float m_fImagePosY{};

	wstring m_wstrSlash{ L"/" };
	wstring m_wstrCur{ L"" };
	wstring m_wstrMax{ L"" };

	_float m_fAdjustFontSize{};

	_float m_fDefaultStartAdjustLeft{};
	_float m_fDefaultStartAdjustRight{};

	_float m_fTextGapX{};

	_float m_fLeftNumStart{};
	_float m_fRightNumStart{};

public:
	static CUI_MainGaugeBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END