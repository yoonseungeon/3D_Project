#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Client)

class CInGameCharHUD_Empty : public CUI_Default
{
private:
	CInGameCharHUD_Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInGameCharHUD_Empty(const CInGameCharHUD_Empty& Prototype);
	virtual ~CInGameCharHUD_Empty() = default;

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
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_Inventory(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI_Equipment(const _wstring& strLayerTag);

private:
	_float m_fScaleRatioY{ 0.125f };
	_float m_fPosRatioY{ -0.436945f };

public:
	static CInGameCharHUD_Empty* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END