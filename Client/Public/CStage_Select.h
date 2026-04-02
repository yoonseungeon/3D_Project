#pragma once
#include "CStage.h"

NS_BEGIN(Client)

class CStage_Select : public CStage
{
	enum VIEW_TYPE { PICK, SKIN, MAP, VIEW_TYPE_END };

private:
	CStage_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStage_Select() = default;

public:
	virtual void Enable_Stage() override;
	virtual void Disable_Stage() override;

	void Change_SelectMap();

private:
	virtual HRESULT Initialize(function<void(STAGE)> funcBtnCallBack) override;

private:
	vector<CGameObject*> m_UIs[VIEW_TYPE::VIEW_TYPE_END];

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);


	HRESULT Ready_Layer_PickPanel(const _wstring& strLayerTag);
	HRESULT Ready_Layer_SkinPanel(const _wstring& strLayerTag);

	HRESULT Ready_Map();
	HRESULT Ready_Map_Layer_UI_Image(const _wstring& strLayerTag);
	HRESULT Ready_Map_Layer_MapPanel(const _wstring& strLayerTag);

public:
	static CStage_Select* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, function<void(STAGE)> funcBtnCallBack);
protected:
	virtual void Free() override;
};

NS_END