#pragma once

enum class SKILL_ICON_TYPE
{
	NORMAL,
	STAKC
};

struct COOL_INFO
{
	float fMaxCoolDown{};
	float fCurCoolDown{};
	float fAccCoolDown{};

	bool bCoolWait{};
};

struct STACK_COOL_INFO : public COOL_INFO
{
	float fMaxSubCoolDown{};
	float fCurSubCoolDown{};
	float fAccSubCoolDown{};

	unsigned int fStack{};
};

enum class ICON_TYPE
{
	Q,
	W,
	E,
	R
};