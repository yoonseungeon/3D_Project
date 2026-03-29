#pragma once

enum class CHAR_NAME { LIDAILIN, HYUNWOO, CHARNAME_END };

enum class SKIN_CLASS { NORMAL, RARE, EPIC, SKIN_CLASS_END };

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

	const SKIN_META* pSkins;
	size_t SkinCnt;
};

constexpr SKIN_META LiDailinSkins[]
{
	{L"LiDailin", SKIN_CLASS::NORMAL, 0},
	{L"Dragon_Dailin", SKIN_CLASS::RARE, 1},
};

constexpr SKIN_META HyunwooSkins[]
{
	{L"Hyunwoo", SKIN_CLASS::NORMAL, 0},
};

constexpr CHARACTER_META Characters[]
{
	{L"LiDailin" , CHAR_NAME::LIDAILIN, L"Prototype_Texture_CharLobbyPick_LiDailin", L"Prototype_Texture_PickLiDailinSkin", L"PickLiDailinSkin%d.png", LiDailinSkins, 2},
	{L"Hyunwoo" , CHAR_NAME::HYUNWOO, L"Prototype_Texture_CharLobbyPick_Hyunwoo", L"Prototype_Texture_PickHyunwooSkin", L"PickHyunwooSkin%d.png", HyunwooSkins, 1},
};