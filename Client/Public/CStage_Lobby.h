#pragma once
#include "CStage.h"

NS_BEGIN(Client)

class CStage_Lobby : public CStage
{
private:
	CStage_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStage_Lobby() = default;

public:
	virtual void Enable_Stage() override;
	virtual void Disable_Stage() override;

private:
	virtual HRESULT Initialize(function<void(STAGE)> funcBtnCallBack) override;

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Btn(const _wstring& strLayerTag);

public:
	static CStage_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, function<void(STAGE)> funcBtnCallBack);
protected:
	virtual void Free() override;
};

NS_END