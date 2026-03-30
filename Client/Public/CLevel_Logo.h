#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class CUI_Image;

class CLevel_Logo : public CLevel
{
private:
	enum LogoStage { LS_1, LS_2, LS_3, LS_END };

private:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

private:
	HRESULT Initialize();

public:
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CUI_Image*> m_FadeImages[LS_END];

	LogoStage m_eLogoStage{ LS_1 };
	_float m_fAccTime{};

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END