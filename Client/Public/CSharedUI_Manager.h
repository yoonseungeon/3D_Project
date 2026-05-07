#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUI_ItemBoxPanel;
class CInvenOwner;

class CSharedUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CSharedUI_Manager)

private:
	CSharedUI_Manager();
	virtual ~CSharedUI_Manager() = default;

private:
	HRESULT Initialize();

public:
	void PopUp_ItemBoxUI(CInvenOwner* InvenOwner);
	void PopDown_ItemBoxUI();

private:
	CGameInstance* m_pGameInstance{ nullptr };

	CUI_ItemBoxPanel* m_pItemBoxPanel{};

private:
	HRESULT Ready_Layer_UI_ItemBox_Inventory(const _wstring& strLayerTag);

protected:
	virtual void Free() override;
};

NS_END