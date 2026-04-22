#pragma once

enum class CHAR_NAME { LIDAILIN, HYUNWOO, CHARNAME_END };

enum class SKIN_CLASS { COMMON, UNCOMMON, RARE, EPIC };

struct SKIN_META
{
	const wchar_t* SkinName;
	SKIN_CLASS eSkinClass;
	unsigned int SkinIdx;
};

struct CHARACTER_META
{
	const wchar_t* CharacterName;
	CHAR_NAME eCharacterName;

	const wchar_t* PickTag;

	const wchar_t* SkinTag;
	const wchar_t* SkinPath;

	const wchar_t* FullSkinTag;
	const wchar_t* FullSkinPath;

	const wchar_t* InGameProfileTag;
	const wchar_t* InGameProfilePath;


	const SKIN_META* pSkins;
	size_t SkinCnt;
};

constexpr SKIN_META LiDailinSkins[]
{
	{L"LiDailin", SKIN_CLASS::COMMON, 0, },
	{L"Dragon_Dailin", SKIN_CLASS::RARE, 1},
};

constexpr SKIN_META HyunwooSkins[]
{
	{L"Hyunwoo", SKIN_CLASS::COMMON, 0},
};

constexpr CHARACTER_META Characters[]
{
	{L"LiDailin" , CHAR_NAME::LIDAILIN, L"Prototype_Tex_Pick_LiDailin", L"Prototype_Tex_PickSkin_LiDailin", L"PickLiDailinSkin%d.dds", L"Prototype_Tex_Full_LiDailin", L"FullSkinLiDailin%d.png", L"Prototype_Tex_CharProfile_LiDailin", L"CharProfile_LiDailin%d.png", LiDailinSkins, 2},
	{L"Hyunwoo" , CHAR_NAME::HYUNWOO, L"Prototype_Tex_Pick_Hyunwoo", L"Prototype_Tex_PickSkin_Hyunwoo", L"PickHyunwooSkin%d.dds", L"Prototype_Tex_Full_Hyunwoo", L"FullSkinHyunwoo%d.png", L"Prototype_Tex_CharProfile_Hyunwoo", L"CharProfile_Hyunwoo%d.png", HyunwooSkins, 1},
};