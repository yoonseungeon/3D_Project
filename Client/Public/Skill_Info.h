#pragma once

struct COOL_INFO
{
	float fMaxCoolDown{};
	float fCurCoolDown{};
	float fAccCoolDown{};

	bool bCoolWait{};


	void Update_Cool(float fTimeDelta)
	{
		if (fAccCoolDown > 0.f && bCoolWait == false)
		{
			fAccCoolDown -= fTimeDelta;
			if (fAccCoolDown < 0.f) {
				fAccCoolDown = 0.f;
			}
		}
	}
};

struct STACK_COOL_INFO : public COOL_INFO
{
	float fMaxSubCoolDown{};
	float fCurSubCoolDown{};
	float fAccSubCoolDown{};

	unsigned int fStack{};

	void Update_SubCool(float fTimeDelta)
	{
		if (fAccSubCoolDown > 0.f)
		{
			fAccSubCoolDown -= fTimeDelta;
			if (fAccSubCoolDown < 0.f) {
				fAccSubCoolDown = 0.f;

				fAccCoolDown = fCurCoolDown;
				fStack = 0;
			}
		}
	}
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