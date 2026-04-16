#pragma once

#include "Engine_Defines.h"
#include "LiDailin_Info.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

enum COMMAND_TYPE
{
	NONE,
	MOVE,
	ATTACK,
	ATTACK_Q,
	ATTACK_W,
	ATTACK_E,
	ATTACK_R,

};

struct COMMAND
{
	COMMAND_TYPE eCommandType{};
	_float3 vTargetPos{};
	CGameObject* pGameObject{};
};

struct COOL_INFO
{
	_float fMaxCoolDown{};
	_float fCurCoolDown{};
	_float fAccCoolDown{};

	_bool bChanneling{};
};

struct STACK_COOL_INFO {
	_float fMaxCoolDown{};
	_float fCurCoolDown{};
	_float fAccCoolDown{};

	_float fMaxSubCoolDown{};
	_float fCurSubCoolDown{};
	_float fAccSubCoolDown{};

	_uint fStack{};
};