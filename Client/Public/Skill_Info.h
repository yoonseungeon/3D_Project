#pragma once

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

enum class COOLDOWN_TYPE
{
	NORMAL,
	STACK
};

enum class SKILL_SLOT
{
	Q,
	W,
	E,
	R
};

struct SKILL_DESC
{
	SKILL_SLOT eSkillSlot{};
	COOLDOWN_TYPE eCoolDownType{};
};