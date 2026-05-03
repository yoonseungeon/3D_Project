#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CCharData_Manager final : public CBase
{
	DECLARE_SINGLETON(CCharData_Manager)

public:
	struct SKIN_INFO_DESC
	{
		wstring wstrSkinName;
		SKIN_CLASS eSkinClass{};
		_uint iSkinIdx{};
	};

	struct CHAR_INFO_DESC
	{
		wstring wstrCharacterName;
		CHAR_NAME eCharName{};

		wstring wstrPickTag;

		wstring wstrSkinTag;
		wstring wstrSkinPath;

		wstring wstrFullSkinTag;
		wstring wstrFullSkinPath;

		wstring wstrProfileTag;
		wstring wstrProfilePath;

		vector<SKIN_INFO_DESC> Skins;
	};

private:
	CCharData_Manager();
	virtual ~CCharData_Manager() = default;

private:
	HRESULT Initialize();

public:
	CHAR_INFO_DESC* Get_CharInfo(CHAR_NAME eCharName);

private:
	unordered_map<CHAR_NAME, CHAR_INFO_DESC> m_CharSkins;

protected:
	virtual void Free() override;
};

NS_END
