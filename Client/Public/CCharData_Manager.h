#pragma once
#ifdef _DEBUG

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)

class CCharData_Manager final : public CBase
{
	DECLARE_SINGLETON(CCharData_Manager)

public:
	struct SKIN_DESC
	{
		wstring wstrSkinName;
		_uint iSkinIdx{};
	};

	struct CHAR_SKIN_DESC
	{
		CHAR_NAME eCharName{};
		vector<SKIN_DESC> Skins;

		wstring wstrSkinSmallTexTag;
	};

public:
	const CHAR_SKIN_DESC& Get_CharSkinInfo(CHAR_NAME eCharName);

private:
	CCharData_Manager();
	virtual ~CCharData_Manager() = default;

public:
	HRESULT Initialize();

private:
	unordered_map<CHAR_NAME, CHAR_SKIN_DESC> m_CharSkins;

protected:
	virtual void Free() override;
};

NS_END

#endif
