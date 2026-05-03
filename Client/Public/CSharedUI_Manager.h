#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CSharedUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CSharedUI_Manager)

private:
	CSharedUI_Manager();
	virtual ~CSharedUI_Manager() = default;

private:
	HRESULT Initialize();

	HRESULT Ready_Layer_UI_ItemBox_Inventory(const _wstring& strLayerTag);

private:
	CGameInstance* m_pGameInstance{ nullptr };


protected:
	virtual void Free() override;
};

NS_END