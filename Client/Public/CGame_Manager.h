#pragma once
#ifdef _DEBUG

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CGame_Manager)

private:
	struct SELECTED_CHAR_DESC
	{
		CHAR_NAME eCharName{ CHAR_NAME::CHARNAME_END };
		wstring wstrSkinName = L"Default";
	};

public:

private:
	CGame_Manager();
	virtual ~CGame_Manager() = default;

public:
	void Set_SelectChar(CHAR_NAME eCharName);
	void Set_SelectSkin(const wstring& wstrSkinName);

	CHAR_NAME Get_SelectedChar();

private:
	SELECTED_CHAR_DESC m_tSelectedChar{};

protected:
	virtual void Free() override;
};

NS_END

#endif
